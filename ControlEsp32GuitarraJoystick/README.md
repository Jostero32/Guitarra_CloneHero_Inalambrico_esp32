# Versión joystick (para teléfono)

Variante del firmware para jugar Clone Hero en **Android**. Usa el mismo circuito y los mismos pines que la
versión de botones; solo cambia cómo se presenta la guitarra por Bluetooth.

## Por qué existe

La versión original envía los 10 pulsadores como botones HID 1 a 10. Android convierte cada número en una tecla
fija, y varias de ellas no las leen la mayoría de los juegos:

| Botón HID | Tecla en Android | ¿Lo leen los juegos? |
|---|---|---|
| 3 | `BUTTON_C` | A menudo no |
| 6 | `BUTTON_Z` | A menudo no |
| 9 | `BUTTON_L2` | A veces solo como gatillo analógico |

Esta versión usa solo entradas estándar, con la misma distribución que una guitarra de Xbox 360:

| Pulsador | GPIO | Llega al teléfono como |
|---|---|---|
| Traste verde | 13 | `A` |
| Traste rojo | 14 | `B` |
| Traste amarillo | 27 | `Y` |
| Traste azul | 26 | `X` |
| Traste naranja | 25 | `L1` |
| Rasgueo arriba | 33 | Joystick Y arriba + cruceta arriba |
| Rasgueo abajo | 32 | Joystick Y abajo + cruceta abajo |
| Start | 35 | `START` (pull-up externa de 10 kΩ) |
| Select | 34 | `SELECT` (pull-up externa de 10 kΩ) |
| Extra (p. ej. Star Power) | 23 | `R1` |

Si sus cables están en otro orden, cambie los pines en la tabla `entradas[]` del sketch.

## Uso

1. Suba `ControlEsp32GuitarraJoystick.ino` igual que la versión original (ESP32 Dev Module, librería
   ESP32-BLE-Gamepad 0.8.x).
2. En el teléfono, **olvide** el dispositivo «Guitarra Azul» si estaba emparejado y empareje
   **«Guitarra Naranja»**. El nombre cambia a propósito para que Android lea el nuevo descriptor.
3. En Clone Hero asigne cada traste, el rasgueo (cruceta o eje Y), Start/Select y el botón extra.

Por el monitor serie (115200 baudios) se imprime cada pulsador al presionarlo y soltarlo, útil para comprobar
el cableado.

Si aun así falta algún botón, pruebe cambiar `TIPO_CONTROL` a `CONTROLLER_TYPE_JOYSTICK`: los botones llegarán
como `BUTTON_1`…`BUTTON_16`, que algunos teléfonos manejan mejor.

> Compila sin advertencias propias (48 % de la flash), pero todavía no se ha probado en la guitarra física.
