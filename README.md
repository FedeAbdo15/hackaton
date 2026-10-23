# 🏴‍☠️ Búsqueda del Tesoro BLE (Hackatón)

Este proyecto consiste en un sistema interactivo de **Búsqueda del Tesoro** en tiempo real desarrollado para microcontroladores (como ESP32) utilizando **Bluetooth Low Energy (BLE)**. 

El sistema consta de dos roles principales:
1. **Tesoro (`Tesoro.ino`)**: Dispositivo emisor (Beacon / BLE Server) que anuncia su presencia periódicamente.
2. **Buscador (`Buscador/`)**: Dispositivo receptor (BLE Client / Scanner) que rastrea la intensidad de la señal (RSSI), filtra el ruido, calcula la proximidad y proporciona feedback al usuario mediante una pantalla OLED y la consola serial.

---

## 📁 Estructura del Proyecto

```text
hackaton-main/
├── NotasInicio.txt                 # Notas de diseño y configuración inicial
├── Tesoro/
│   └── Tesoro.ino                  # Firmware del dispositivo emisor (Beacon BLE)
└── Buscador/
    ├── Buscador.ino                # Sketch principal del rastreador
    ├── config.h                    # Parámetros de red BLE, tiempos y umbrales de RSSI
    ├── BleScanner.h / .cpp         # Manejo del escaneo BLE asíncrono
    ├── SignalFilter.h              # Filtro para suavizar lecturas de RSSI (ruido)
    ├── Proximity.h                 # Enums y definiciones de niveles de distancia
    ├── ProximityClassifier.h / .cpp# Lógica de cálculo de proximidad según RSSI
    ├── GameState.h / .cpp          # Máquina de estados del juego (buscando, encontrado, etc.)
    ├── Feedback.h                  # Interfaz genérica para notificación/salidas
    ├── SerialFeedback.h            # Salida de depuración por monitor serial
    └── OledFeedback.h / .cpp       # Interfaz gráfica y barras de señal en pantalla OLED (I2C)
```

---

## ⚙️ Componentes y Arquitectura

### 1. El Tesoro (`Tesoro/`)
* Emite paquetes publicitarios BLE continuos con un identificador (`UUID` o `Device Name`) preconfigurado.
* Opera en modo bajo consumo mientras aguarda ser encontrado.

### 2. El Buscador (`Buscador/`)
* **Escaneo BLE (`BleScanner`)**: Detecta balizas cercanas e identifica el paquete específico del tesoro.
* **Filtrado de Señal (`SignalFilter`)**: Implementa un filtro (ej. media móvil o suavizado exponencial) para reducir el rebote e inconsistencia intrínseca del RSSI.
* **Clasificador de Proximidad (`ProximityClassifier`)**: Traduce el RSSI procesado a categorías intuitivas:
  * ❄️ *Muy Lejos / Frío*
  * 🌤️ *Tibio*
  * 🔥 *Caliente / Muy Cerca*
  * 🏆 *¡Encontrado!*
* **Máquina de Estados (`GameState`)**: Controla las transiciones del juego (Buscando, Tesoro en Rango, Tesoro Capturado/Descubierto, Fin de Partida).
* **Feedback al Usuario (`OledFeedback` & `SerialFeedback`)**: Renderiza en una pantalla OLED (típicamente SSD1306 vía I2C) animaciones, barras de proximidad y mensajes de estado.

---

## 🚀 Requisitos y Dependencias

### Hardware
* 2x Placas de desarrollo con soporte BLE (ej. **ESP32 NodeMCU / WROOM**).
* 1x Pantalla OLED I2C 128x64 (SSD1306) conectada al dispositivo Buscador.
* Cables dupont y fuentes de alimentación/baterías portátiles.

### Software y Librerías Arduino
* **Arduino IDE** o **PlatformIO / VS Code**.
* Paquete de placas **ESP32 Arduino Core**.
* Librería BLE nativa de ESP32 (`ESP32 BLE Arduino`).
* Librería OLED (ej. `Adafruit_SSD1306` y `Adafruit_GFX` o `U8g2`, según la implementación de `OledFeedback`).

---

## 🛠️ Instalación y Carga

1. **Configuración Inicial**:
   * Abrí `Buscador/config.h` y verificá que los identificadores de servicio (`SERVICE_UUID`, nombre de dispositivo o MAC) coincidan con los configurados en `Tesoro/Tesoro.ino`.
   * Ajustá los umbrales de RSSI en `config.h` si el entorno tiene muchas paredes o interferencias.

2. **Cargar el Tesoro**:
   * Abrí `Tesoro/Tesoro.ino` en el Arduino IDE.
   * Conectá la primera placa ESP32, seleccioná el puerto COM correspondiente y hacé clic en **Subir**.

3. **Cargar el Buscador**:
   * Abrí `Buscador/Buscador.ino`.
   * Conectá la segunda placa con la pantalla OLED conectada (pines típicos SDA/SCL: GPIO 21 y GPIO 22 en ESP32).
   * Seleccioná el puerto y hacé clic en **Subir**.

---

## 🎮 Modo de Uso

1. Encendé y ocultá la placa del **Tesoro** en el área designada.
2. Encendé el **Buscador**.
3. Guiate por los indicadores visuales en la pantalla OLED (o monitor serial a `115200 bauds`): a medida que el valor de RSSI aumente, la pantalla indicará mayor cercanía ("Frío" → "Tibio" → "Caliente").
4. Al llegar a la distancia mínima configurada, el estado cambiará a **Tesoro Encontrado**.