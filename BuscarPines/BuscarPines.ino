/*
 * Buscador de pines para la guitarra (diagnóstico, sin Bluetooth)
 *
 * Lee casi todos los GPIO del ESP32 y muestra por el monitor serie (115200)
 * cuál cambia al presionar cada pulsador. Sirve para:
 *   - Saber en qué pin está cada botón de una guitarra ya cableada.
 *   - Detectar un corto: si al presionar un botón no aparece ningún GPIO y el
 *     ESP32 se reinicia, el pulsador no va entre un GPIO y GND.
 *
 * No usa Bluetooth, así que consume poco: si aquí también hay «Brownout», el
 * problema es el cableado o la alimentación, no el firmware de la guitarra.
 */

#include <Arduino.h>

// GPIO2 se omite (LED de la placa) y 1/3 son el puerto serie. 6–11 son de la
// memoria flash.
const uint8_t pines[] = {0, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23,
                         25, 26, 27, 32, 33, 34, 35, 36, 39};
const uint8_t NUM_PINES = sizeof(pines) / sizeof(pines[0]);
const uint32_t ESTABLE_MS = 30;  // Filtro de rebote

uint8_t estado[NUM_PINES];
uint8_t lecturaAnterior[NUM_PINES];
uint32_t ultimoCambio[NUM_PINES];

void setup()
{
    Serial.begin(115200);
    delay(200);
    Serial.printf("\nMotivo del último reinicio: %d (1 encendido, 9 brownout)\n",
                  (int)esp_reset_reason());

    for (uint8_t i = 0; i < NUM_PINES; i++)
    {
        // 34–39 son solo de entrada y no tienen pull-up interna.
        pinMode(pines[i], pines[i] >= 34 ? INPUT : INPUT_PULLUP);
    }
    delay(50);
    for (uint8_t i = 0; i < NUM_PINES; i++)
    {
        estado[i] = lecturaAnterior[i] = digitalRead(pines[i]);
    }

    Serial.println("Estado inicial (0 = presionado o conectado a GND):");
    for (uint8_t i = 0; i < NUM_PINES; i++)
    {
        Serial.printf("  GPIO%-2u = %u%s\n", pines[i], estado[i],
                      pines[i] >= 34 ? "  (sin pull-up interna)" : "");
    }
    Serial.println("Presione los botones uno por uno...");
}

void loop()
{
    const uint32_t ahora = millis();
    for (uint8_t i = 0; i < NUM_PINES; i++)
    {
        const uint8_t lectura = digitalRead(pines[i]);
        if (lectura != lecturaAnterior[i])
        {
            lecturaAnterior[i] = lectura;
            ultimoCambio[i] = ahora;
        }
        else if (lectura != estado[i] && ahora - ultimoCambio[i] >= ESTABLE_MS)
        {
            estado[i] = lectura;
            Serial.printf("GPIO%u %s\n", pines[i], lectura == LOW ? "presionado" : "suelto");
        }
    }
    delay(1);
}
