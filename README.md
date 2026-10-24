# 🏴‍☠️ Búsqueda del Tesoro por ESP-NOW (Hackatón)

Sistema interactivo de **Búsqueda del Tesoro** para dos jugadores en tiempo real, desarrollado sobre microcontroladores **ESP32** utilizando comunicación inalámbrica bidireccional **ESP-NOW** y pantallas **OLED SSD1306 (SPI)**.

El sistema consta de dos dispositivos que se comunican entre sí sin necesidad de router Wi-Fi:
1. **Tesoro (`Tesoro/`)**: Ocultado por el **Jugador 1**. Emite paquetes broadcast ESP-NOW periódicos al activarse con su botón físico, lleva un cronómetro en su pantalla OLED (arrancando desde `-00:00:05`) y emite un pitido por buzzer cada 5 segundos durante la partida hasta que el Buscador lo encuentra.
2. **Buscador (`Buscador/`)**: Utilizado por el **Jugador 2**. Mide el RSSI de los paquetes ESP-NOW recibidos, los suaviza con un filtro exponencial (EMA), calcula una distancia relativa de `0` a `100`, clasifica la proximidad con histéresis y muestra animaciones gráficas en su pantalla OLED según qué tan cerca está del Tesoro.

---

## 📁 Estructura del Proyecto

```text
hackaton/
├── NotasInicio.txt                  # Notas iniciales del proyecto
├── README.md                        # Documentación general
├── Tesoro/
│   ├── Tesoro.ino                   # Sketch principal del Tesoro (máquina de estados)
│   ├── config.h                     # Canal ESP-NOW, pines SPI OLED, botón N1, timer y buzzer
│   ├── TreasureBeacon.h / .cpp      # Transmisión de beacons y recepción de comandos ESP-NOW
│   ├── TimerDisplay.h / .cpp        # Cronómetro en pantalla OLED SSD1306 (desde -00:00:05)
│   ├── Buzzer.h / .cpp              # Pitido periódico no bloqueante cada 5 segundos
│   └── Button.h / .cpp              # Lectura del Botón N1 con pull-up interno y antirrebote
└── Buscador/
    ├── Buscador.ino                 # Sketch principal del Buscador
    ├── config.h                     # Canal ESP-NOW, umbrales de RSSI (dBm), tiempos y pines
    ├── EspNowScanner.h / .cpp       # Recepción de RSSI por ESP-NOW y envío de eventos al Tesoro
    ├── SignalFilter.h               # Filtro de media móvil exponencial (EMA) para suavizar el RSSI
    ├── Proximity.h                  # Enum de niveles de proximidad (SinSenal, Frio, Tibio, Caliente, Encontrado)
    ├── ProximityClassifier.h / .cpp # Clasificador por umbrales en dBm con histéresis
    ├── GameState.h / .cpp           # Fases del juego, conteo regresivo, cálculo de distancia (0..100) y timeout
    ├── Feedback.h                   # Interfaz común para salidas del sistema
    ├── SerialFeedback.h             # Log de cambios de estado por consola Serial
    ├── OledFeedback.h / .cpp        # Renderizado de pantallas según fase del juego y estado
    ├── OledAnimations.h / .cpp      # Animaciones gráficas en OLED (Radar, Pingüino, Tibia, This is fine, Cofre)
    └── Button.h / .cpp              # Lectura del Botón N2 con pull-up interno y antirrebote
```

---

## 🎮 Flujo Completo del Juego

1. **Encendido inicial**:
   * **Tesoro**: Muestra `00:00:00` detenido en su OLED y permanece en espera sin transmitir.
   * **Buscador**: Muestra `"Presiona el boton 2 para empezar"` en su OLED.
2. **Preparación del Tesoro (Jugador 1)**:
   * El Jugador 1 esconde el Tesoro y presiona el **Botón N1 (GPIO 19)**.
   * El Tesoro comienza a emitir paquetes `TreasureBeacon` por ESP-NOW cada `80 ms` y queda esperando la señal de inicio del Buscador.
3. **Inicio de Partida (Jugador 2)**:
   * Si el Jugador 2 presiona el **Botón N2 (GPIO 19)** antes de que el Tesoro esté transmitiendo, la OLED del Buscador advierte **`"Tesoro no preparado"`**.
   * Si el Tesoro ya está transmitiendo, el Buscador envía el comando `GameStart` por ESP-NOW:
     * El **Buscador** muestra **`"Juego comenzado"`** con una cuenta regresiva de **5 segundos** (`Iniciando en 5...`).
     * El **Tesoro** inicia su cronómetro en **`-00:00:05`** y al llegar a `00:00:00` comienza a sonar el buzzer cada 5 segundos.
4. **Búsqueda en curso (`Playing`)**:
   * El Buscador muestra en la parte superior de la OLED la **distancia estimada (`1` a `100`)** mapeada entre `THRESHOLD_ENCONTRADO` (`0`) y `THRESHOLD_TIBIO` (`100`).
   * En el centro de la pantalla renderiza una animación grande acorde al estado de proximidad:
     * 📡 **`SIN SENAL`**: Ondas de radar expandiéndose con un `"?"` en el centro (cuando se pierde la señal por más de `3 s`).
     * 🐧 **`FRIO`** (`RSSI < THRESHOLD_TIBIO`): Un **pingüino** caminando y aleteando bajo la nieve.
     * 🦴 **`TIBIO`** (`RSSI >= -60 dBm`): Una **tibia (hueso)** balanceándose.
     * 🔥 **`CALIENTE`** (`RSSI >= -45 dBm`): El meme ***"This is fine"*** (el perrito con sombrero y su taza de café humeante rodeado de fuego).
5. **Tesoro Encontrado (`Victory`)**:
   * Al alcanzar `THRESHOLD_ENCONTRADO` (`>= -10 dBm`, distancia `0`), el Buscador envía automáticamente el comando `TreasureFound` por ESP-NOW.
   * El **Tesoro** detiene al instante el cronómetro (dejando el tiempo final en pantalla) y apaga el buzzer.
   * El **Buscador** muestra `"ENCONTRADO"` en grande rebotando junto con un **cofre del tesoro abriéndose y lanzando destellos** (cuya velocidad de animación varía según el tiempo que tomó encontrarlo).

---

## 📡 Protocolo ESP-NOW (`EspNowPacket`)

Ambos dispositivos operan en modo Wi-Fi Station (`WIFI_STA`) sobre el **Canal 1** enviando tramas broadcast (`FF:FF:FF:FF:FF:FF`) con la siguiente estructura binaria compacta:

```cpp
struct __attribute__((packed)) EspNowPacket {
  uint32_t magic;        // 0x5445534F ("TESO"): filtra paquetes ajenos en el mismo canal
  MsgType  type;         // 1 = TreasureBeacon, 2 = GameStart, 3 = TreasureFound
  int32_t  timerSeconds; // Segundos actuales del cronómetro del Tesoro
};
```

* **`PACKET_MAGIC` (`0x5445534F` / `"TESO"`)**: Permite ignorar tramas ESP-NOW de otros dispositivos o equipos que estén transmitiendo en el mismo canal.
* **Ráfagas de comandos (`CMD_BURST_COUNT = 3`)**: Los eventos críticos (`GameStart` y `TreasureFound`) se envían por triplicado para garantizar su recepción inmediata.

---

## 🔌 Conexiones de Hardware (Pinout)

Ambos dispositivos comparten el mismo esquema de pines para la pantalla OLED SPI y el botón principal:

| Componente | Pin en ESP32 | Constante en `config.h` |
| :--- | :---: | :--- |
| **OLED MOSI (SDA / D1)** | `GPIO 2` | `OLED_MOSI` |
| **OLED CLK (SCL / D0)** | `GPIO 1` | `OLED_CLK` |
| **OLED CS** | `GPIO 3` | `OLED_CS` |
| **OLED DC** | `GPIO 6` | `OLED_DC` |
| **OLED RST (RES)** | `GPIO 7` | `OLED_RST` |
| **Botón N1 / N2** *(a GND)* | `GPIO 19` | `BUTTON_N1_PIN` / `BUTTON_N2_PIN` |
| **Buzzer** *(solo en Tesoro)* | `GPIO 25` | `BUZZER_PIN` |

> **Nota sobre la pantalla**: Ambos firmwares configuran la rotación en 180° (`OLED_ROTATION = 2`).

---

## 🛠️ Requisitos e Instalación

### Librerías Necesarias
1. ** Core ESP32 para Arduino** (`esp32` por Espressif Systems, incluye `WiFi.h`, `esp_now.h` y `esp_wifi.h`).
2. **Adafruit SSD1306** y **Adafruit GFX Library** (instalables desde el Administrador de Bibliotecas del Arduino IDE).

### Carga y Calibración
1. Cargar `Tesoro/Tesoro.ino` en la placa del **Jugador 1**.
2. Cargar `Buscador/Buscador.ino` en la placa del **Jugador 2**.
3. Para calibrar las distancias según el entorno físico, ajustar en `Buscador/config.h`:
   * `THRESHOLD_TIBIO` (por defecto `-60.0f` dBm)
   * `THRESHOLD_CALIENTE` (por defecto `-45.0f` dBm)
   * `THRESHOLD_ENCONTRADO` (por defecto `-10.0f` dBm)
   * `HYSTERESIS_DB` (por defecto `3.0f` dB para evitar parpadeos entre niveles)
