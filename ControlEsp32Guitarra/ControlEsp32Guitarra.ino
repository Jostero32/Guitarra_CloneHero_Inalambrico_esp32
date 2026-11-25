/*
 * Guitarra inalámbrica para Clone Hero con ESP32 (Bluetooth LE)
 *
 * El ESP32 se anuncia como un gamepad BLE («Guitarra Azul») con 9 botones.
 * Cada botón físico va entre su GPIO y GND; en Clone Hero se asignan a los
 * trastes, el rasgueo y los botones de menú desde la configuración del control.
 *
 * Librería: ESP32-BLE-Gamepad (lemmingDev), que instala también NimBLE-Arduino.
 *
 * Nota de hardware: GPIO34 y GPIO35 son solo de entrada y NO tienen pull-up
 * interna. Esos dos botones necesitan una resistencia de 10 kΩ a 3V3.
 */

#include <Arduino.h>
#include <BleGamepad.h>  // https://github.com/lemmingDev/ESP32-BLE-Gamepad

BleGamepad bleGamepad("Guitarra Azul");  // Nombre que aparece al emparejar

const byte NUM_BUTTONS = 9;
const byte buttonPins[NUM_BUTTONS] = {13, 14, 27, 26, 25, 33, 32, 35, 34};
const byte gamepadButtons[NUM_BUTTONS] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

byte previousStates[NUM_BUTTONS];

void setup()
{
    for (byte i = 0; i < NUM_BUTTONS; i++)
    {
        // En GPIO34/35 INPUT_PULLUP no tiene efecto: usan la pull-up externa.
        pinMode(buttonPins[i], INPUT_PULLUP);
        previousStates[i] = HIGH;
    }

    BleGamepadConfiguration bleGamepadConfig;
    bleGamepadConfig.setAutoReport(false);  // Se envía un solo reporte por cambio
    bleGamepadConfig.setButtonCount(NUM_BUTTONS);
    bleGamepad.begin(&bleGamepadConfig);
}

void loop()
{
    if (!bleGamepad.isConnected())
    {
        delay(100);
        return;
    }

    bool changed = false;
    for (byte i = 0; i < NUM_BUTTONS; i++)
    {
        const byte state = digitalRead(buttonPins[i]);
        if (state == previousStates[i]) continue;

        if (state == LOW) bleGamepad.press(gamepadButtons[i]);
        else bleGamepad.release(gamepadButtons[i]);

        previousStates[i] = state;
        changed = true;
    }

    if (changed) bleGamepad.sendReport();

    delay(5);  // Lectura cada 5 ms; también filtra el rebote de los pulsadores
}
