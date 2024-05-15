# Control Inalámbrico Bluetooth para Guitarra de Clone Hero

## Descripción del Proyecto

Este proyecto involucra el desarrollo de un controlador inalámbrico Bluetooth para usar con el juego Clone Hero, imitando una guitarra de juego. Se utiliza un microcontrolador ESP32 debido a su capacidad Bluetooth y se programa con la librería BLEGamepad para emular un gamepad.

## Características Principales

- **Conexión Bluetooth:** El controlador utiliza Bluetooth para conectar con el PC o consola donde se ejecuta Clone Hero.
- **Emulación de Gamepad:** Gracias a la biblioteca BLEGamepad, el dispositivo se reconoce como un control de juego estándar.
- **Botones personalizados:** Incluye botones configurados como las cuerdas de una guitarra, permitiendo una experiencia de juego auténtica.
- **Portabilidad y facilidad de uso:** Diseñado para ser ligero y fácil de manejar durante largas sesiones de juego.

## Herramientas y Tecnologías Utilizadas

- **Microcontrolador:** ESP32
- **Lenguaje de Programación:** Arduino
- **Librerías Específicas:** BLEGamepad para la funcionalidad Bluetooth
- **Herramientas de Desarrollo:** Arduino IDE

## Diagrama del Proyecto

El proyecto está compuesto por:

- Un ESP32 que actúa como el cerebro del controlador.
- Botones mapeados a las funciones de la guitarra de Clone Hero.
- Batería recargable para asegurar horas de juego sin interrupciones.

## Implementación

### Configuración del ESP32

1. Instalar Arduino IDE y configurarlo para soportar el desarrollo con ESP32.
2. Instalar la librería BLEGamepad desde el gestor de librerías de Arduino.
3. Conectar los botones y otros componentes de entrada al ESP32 según el esquema de conexión.


