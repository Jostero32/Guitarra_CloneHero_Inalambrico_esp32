/*
 * Guitarra inalámbrica para Clone Hero con ESP32 (Bluetooth LE) — versión joystick
 *
 * Variante pensada para teléfonos Android. La versión original enviaba los 9
 * pulsadores como botones HID 1 a 9, y Android traduce esos números a teclas
 * fijas: el 3 llega como BUTTON_C, el 6 como BUTTON_Z y el 9 como BUTTON_L2.
 * Muchos juegos de Android ignoran esas teclas, por eso algunos botones «no
 * existían» en el teléfono.
 *
 * Aquí la guitarra se anuncia como un control con joystick:
 *   - El rasgueo mueve el eje Y del joystick y la cruceta (hat / D-pad),
 *     igual que las guitarras de Xbox 360 que Clone Hero ya conoce.
 *   - Los trastes, el botón extra y el menú usan solo botones que Android
 *     reconoce siempre: A, B, X, Y, L1, R1, Select y Start.
 *
 * Librería: ESP32-BLE-Gamepad (lemmingDev) 0.8.x, que instala NimBLE-Arduino.
 *
 * Nota de hardware: GPIO34 y GPIO35 son solo de entrada y NO tienen pull-up
 * interna. Esos dos botones necesitan una resistencia de 10 kΩ a 3V3.
 *
 * El botón 10 usa GPIO23. No use GPIO12: es pin de arranque y, con la pull-up
 * interna activa, un reinicio por watchdog lo deja en alto y el ESP32 entra en
 * un ciclo de reinicios con «csum err».
 */

#include <Arduino.h>
#include <BleGamepad.h>  // https://github.com/lemmingDev/ESP32-BLE-Gamepad

// Nombre distinto al de la versión de botones, para que el teléfono no
// reutilice el emparejamiento anterior (que guarda el descriptor HID viejo).
BleGamepad bleGamepad("Guitarra Azul Joystick");

// Tipo de dispositivo HID. CONTROLLER_TYPE_GAMEPAD hace que Android traduzca
// los botones a A, B, X, Y, L1, Select y Start. Con CONTROLLER_TYPE_JOYSTICK
// los botones llegan como BUTTON_1..BUTTON_16, que muchos juegos tampoco leen;
// cámbielo solo si su teléfono funciona mejor así.
const uint8_t TIPO_CONTROL = CONTROLLER_TYPE_GAMEPAD;

// Números de botón HID y la tecla en que los convierte Android.
const uint8_t BTN_A = 1;
const uint8_t BTN_B = 2;
const uint8_t BTN_X = 4;
const uint8_t BTN_Y = 5;
const uint8_t BTN_L1 = 7;
const uint8_t BTN_R1 = 8;
const uint8_t BTN_SELECT = 11;
const uint8_t BTN_START = 12;
const uint8_t NUM_BOTONES_HID = 12;

enum Funcion : uint8_t
{
    TRASTE,           // Botón normal
    RASGUEO_ARRIBA,   // Eje Y al mínimo y cruceta arriba
    RASGUEO_ABAJO     // Eje Y al máximo y cruceta abajo
};

struct Entrada
{
    uint8_t pin;
    Funcion funcion;
    uint8_t boton;  // Botón HID (solo se usa con TRASTE)
    const char *nombre;
};

// Asignación de pines. Si sus cables están en otro orden, cambie aquí el pin
// de cada fila; el resto del programa no necesita cambios.
const Entrada entradas[] = {
    {13, TRASTE, BTN_A, "Verde"},
    {14, TRASTE, BTN_B, "Rojo"},
    {27, TRASTE, BTN_Y, "Amarillo"},
    {26, TRASTE, BTN_X, "Azul"},
    {25, TRASTE, BTN_L1, "Naranja"},
    {33, RASGUEO_ARRIBA, 0, "Rasgueo arriba"},
    {32, RASGUEO_ABAJO, 0, "Rasgueo abajo"},
    {35, TRASTE, BTN_START, "Start"},    // Pull-up externa de 10 kΩ
    {34, TRASTE, BTN_SELECT, "Select"},  // Pull-up externa de 10 kΩ
    {23, TRASTE, BTN_R1, "Extra"},       // P. ej. Star Power
};
const uint8_t NUM_ENTRADAS = sizeof(entradas) / sizeof(entradas[0]);

const int16_t EJE_MIN = -32767;
const int16_t EJE_CENTRO = 0;
const int16_t EJE_MAX = 32767;

uint8_t estadosPrevios[NUM_ENTRADAS];
bool rasgueoArriba = false;
bool rasgueoAbajo = false;
bool estabaConectado = false;

void actualizarRasgueo()
{
    // Si por rebote se leen las dos direcciones a la vez, queda centrado.
    if (rasgueoArriba && !rasgueoAbajo)
    {
        bleGamepad.setY(EJE_MIN);
        bleGamepad.setHat1(HAT_UP);
    }
    else if (rasgueoAbajo && !rasgueoArriba)
    {
        bleGamepad.setY(EJE_MAX);
        bleGamepad.setHat1(HAT_DOWN);
    }
    else
    {
        bleGamepad.setY(EJE_CENTRO);
        bleGamepad.setHat1(HAT_CENTERED);
    }
}

const char *motivoReinicio()
{
    switch (esp_reset_reason())
    {
    case ESP_RST_POWERON: return "encendido";
    case ESP_RST_EXT: return "pin EN";
    case ESP_RST_SW: return "reinicio por software";
    case ESP_RST_PANIC: return "fallo del programa (panic)";
    case ESP_RST_INT_WDT: return "watchdog de interrupciones";
    case ESP_RST_TASK_WDT: return "watchdog de tareas";
    case ESP_RST_WDT: return "otro watchdog";
    case ESP_RST_DEEPSLEEP: return "salida de deep sleep";
    case ESP_RST_BROWNOUT: return "caída de voltaje (brownout)";
    default: return "desconocido";
    }
}

void setup()
{
    Serial.begin(115200);
    delay(200);
    Serial.printf("\nMotivo del último reinicio: %s\n", motivoReinicio());

    for (uint8_t i = 0; i < NUM_ENTRADAS; i++)
    {
        // En GPIO34/35 INPUT_PULLUP no tiene efecto: usan la pull-up externa.
        pinMode(entradas[i].pin, INPUT_PULLUP);
        estadosPrevios[i] = HIGH;
    }

    BleGamepadConfiguration config;
    config.setControllerType(TIPO_CONTROL);
    config.setAutoReport(false);  // Se envía un solo reporte por cambio
    config.setButtonCount(NUM_BOTONES_HID);
    config.setHatSwitchCount(1);
    // Solo el joystick izquierdo (X e Y). Menos ejes = menos entradas fantasma
    // al asignar controles en Clone Hero.
    config.setWhichAxes(true, true, false, false, false, false, false, false);
    config.setAxesMin(EJE_MIN);
    config.setAxesMax(EJE_MAX);
    bleGamepad.begin(&config);

    // Joystick centrado desde el inicio.
    bleGamepad.setX(EJE_CENTRO);
    bleGamepad.setY(EJE_CENTRO);
    bleGamepad.setHat1(HAT_CENTERED);

    Serial.println("Guitarra joystick lista, esperando conexión Bluetooth...");
}

void loop()
{
    const bool conectado = bleGamepad.isConnected();
    bool cambio = false;

    if (conectado && !estabaConectado)
    {
        // Al conectar se manda el estado actual para que el teléfono no se
        // quede con valores de una sesión anterior.
        cambio = true;
        Serial.println("Conectado");
    }
    else if (!conectado && estabaConectado)
    {
        Serial.println("Desconectado");
    }
    estabaConectado = conectado;

    // Los pulsadores se leen y se muestran por el monitor serie aunque no haya
    // teléfono conectado, para poder identificar los pines de la guitarra.
    for (uint8_t i = 0; i < NUM_ENTRADAS; i++)
    {
        const uint8_t estado = digitalRead(entradas[i].pin);
        if (estado == estadosPrevios[i]) continue;

        estadosPrevios[i] = estado;
        cambio = true;
        const bool presionado = (estado == LOW);

        switch (entradas[i].funcion)
        {
        case TRASTE:
            if (presionado) bleGamepad.press(entradas[i].boton);
            else bleGamepad.release(entradas[i].boton);
            break;
        case RASGUEO_ARRIBA:
            rasgueoArriba = presionado;
            actualizarRasgueo();
            break;
        case RASGUEO_ABAJO:
            rasgueoAbajo = presionado;
            actualizarRasgueo();
            break;
        }

        Serial.printf("%s (GPIO%u) %s\n", entradas[i].nombre, entradas[i].pin,
                      presionado ? "presionado" : "suelto");
    }

    if (cambio && conectado) bleGamepad.sendReport();

    delay(5);  // Lectura cada 5 ms; también filtra el rebote de los pulsadores
}
