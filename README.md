# Guitarra inalámbrica para Clone Hero (ESP32)

<p>
  <img alt="ESP32" src="https://img.shields.io/badge/ESP32-Bluetooth_LE-E7352C?logo=espressif&logoColor=white" />
  <img alt="Arduino" src="https://img.shields.io/badge/Arduino-core_esp32_3.x-00878F?logo=arduino&logoColor=white" />
</p>

Control inalámbrico para jugar **Clone Hero** con una guitarra. Un ESP32 se anuncia por **Bluetooth LE**
como un gamepad estándar («Guitarra Azul») con 10 botones, así que la PC lo reconoce sin drivers y Clone Hero
lo configura como cualquier control.

## Cómo funciona

- Cada pulsador va entre su GPIO y GND. Presionado se lee 0 y suelto 1.
- El firmware envía un reporte BLE **solo cuando cambia algún botón**, para no saturar el enlace.
- En Clone Hero, en *Settings → Controls*, se asigna cada botón del gamepad a un traste, al rasgueo o al menú.

## Conexiones

| Botón del gamepad | GPIO | Nota |
|---|---|---|
| 1 | 13 | |
| 2 | 14 | |
| 3 | 27 | |
| 4 | 26 | |
| 5 | 25 | |
| 6 | 33 | |
| 7 | 32 | |
| 8 | 35 | **Pull-up externa de 10 kΩ a 3V3** |
| 9 | 34 | **Pull-up externa de 10 kΩ a 3V3** |
| 10 | 23 | En el lado derecho de la placa |

> GPIO34 y GPIO35 son pines solo de entrada y **no tienen resistencia pull-up interna**. Sin la resistencia
> externa quedan flotando y pueden generar pulsaciones fantasma.
>
> No use GPIO12 para botones: es un pin de arranque y, con la pull-up activa, el ESP32 puede quedar en un ciclo
> de reinicios con `csum err`.

**Materiales:** ESP32 DevKit V1 · 10 pulsadores · 2 resistencias de 10 kΩ · cuerpo de guitarra · batería
recargable con regulación a 5 V por USB o VIN.

## Compilar y usar

1. Arduino IDE 2 con el paquete **esp32 de Espressif** (3.x) y la placa **ESP32 Dev Module**.
2. Instale la librería **ESP32-BLE-Gamepad** (lemmingDev) desde el gestor de librerías; instala también
   *NimBLE-Arduino*.
3. Abra `ControlEsp32Guitarra/ControlEsp32Guitarra.ino` y suba el programa. Compila sin advertencias y usa el
   47 % de la flash.
4. En la PC, empareje el dispositivo Bluetooth **Guitarra Azul** y asigne los botones en Clone Hero.

## Versión joystick para teléfono

En Android algunos botones de esta versión no se reconocen. La carpeta
[`ControlEsp32GuitarraJoystick`](ControlEsp32GuitarraJoystick/) tiene una variante con el mismo circuito que
envía el rasgueo por el joystick y la cruceta, los trastes como A, B, X, Y y L1, y el botón extra como R1.

## Diagnóstico de pines

[`BuscarPines`](BuscarPines/) es un sketch sin Bluetooth que muestra por el monitor serie (115200 baudios) qué GPIO
cambia al presionar cada pulsador. Sirve para identificar el cableado de una guitarra o detectar un corto: si al
presionar un botón el ESP32 se reinicia con `Brownout detector was triggered`, ese pulsador no va entre un GPIO
y GND.
