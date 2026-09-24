#include <Arduino.h>
#include <Wire.h>
#include <AudioOutputI2S.h>
#include <AudioFileSourcePROGMEM.h>
#include <AudioGeneratorWAV.h>
#include "audio/wav_hola.h"
#include "audio/wav_adios.h"
#include "audio/wav_si.h"

// Definición de los pines para el ESP32-C3 Mini (según schematics/v0-1-0.png)
const int pinIndice = 2; // GPIO2
const int pinMedio = 1;  // GPIO1
const int pinPulgar = 0; // GPIO0

const int NUM_SENSORES = 3;
const int pines[NUM_SENSORES] = { pinIndice, pinMedio, pinPulgar };

// I2C para el MPU6050 (según schematics/v0-1-0.png)
const int pinSDA = 8; // GPIO8
const int pinSCL = 9; // GPIO9
const uint8_t MPU_ADDR = 0x68;

// I2S para el amplificador/altavoz (según conexiones indicadas por el usuario)
const int pinI2S_BCLK = 5; // GPIO5
const int pinI2S_LRC = 6;  // GPIO6
const int pinI2S_DIN = 7;  // GPIO7

AudioOutputI2S *salidaAudio = nullptr;

// Audios pregrabados (voz TTS de Windows) embebidos en flash, sin SD ni filesystem.
// índice, medio, pulgar
const uint8_t *AUDIOS[NUM_SENSORES] = { WAV_HOLA, WAV_ADIOS, WAV_SI };
const uint32_t AUDIOS_LEN[NUM_SENSORES] = { WAV_HOLA_len, WAV_ADIOS_len, WAV_SI_len };

// Offsets de calibración del MPU6050 (calculados en setup)
int16_t axOffset = 0;
int16_t azOffset = 0;
const int MUESTRAS_CALIBRACION = 200;

// Intervalo entre envíos (ms)
const unsigned long INTERVALO_ENVIO = 50;
unsigned long ultimoEnvio = 0;

// Detección de palabras por flexión de dedos
const int UMBRAL_FLEX = 4000;
const unsigned long TIEMPO_MANTENER = 1000; // ms
const char* PALABRAS[NUM_SENSORES] = { "Hola", "Adios", "Si" }; // índice, medio, pulgar (solo para log por Serial)

unsigned long tiempoInicioSobreUmbral[NUM_SENSORES] = {0, 0, 0};
bool publicado[NUM_SENSORES] = {false, false, false};
int dedoActivo = -1; // índice del dedo con el turno; -1 = ninguno

void mpuEscribirRegistro(uint8_t reg, uint8_t valor) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(valor);
  Wire.endTransmission();
}

// ax: inclinación izquierda/derecha, az: subir/bajar
// Lectura cruda, sin aplicar offsets de calibración
void mpuLeerAccelCrudo(int16_t &ax, int16_t &az) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // ACCEL_XOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom((int)MPU_ADDR, 6, true);

  ax = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read(); // ACCEL_YOUT (descartado)
  az = (Wire.read() << 8) | Wire.read();
}

// Lectura con offsets de calibración aplicados
void mpuLeerAccel(int16_t &ax, int16_t &az) {
  mpuLeerAccelCrudo(ax, az);
  ax -= axOffset;
  az -= azOffset;
}

// Promedia MUESTRAS_CALIBRACION lecturas en reposo y calcula offsets.
// Asume el sensor apoyado plano (Z alineado con la gravedad, ~1g = 16384 LSB).
void calibrarMPU() {
  long sumAx = 0, sumAz = 0;

  for (int i = 0; i < MUESTRAS_CALIBRACION; i++) {
    int16_t ax, az;
    mpuLeerAccelCrudo(ax, az);
    sumAx += ax;
    sumAz += az;
    delay(3);
  }

  axOffset = sumAx / MUESTRAS_CALIBRACION;
  azOffset = (sumAz / MUESTRAS_CALIBRACION) - 16384;
}

// Reproduce el WAV embebido correspondiente al dedo i (bloqueante hasta terminar).
void reproducirPalabra(int i) {
  AudioFileSourcePROGMEM fuente(AUDIOS[i], AUDIOS_LEN[i]);
  AudioGeneratorWAV generador;
  generador.begin(&fuente, salidaAudio);
  while (generador.isRunning()) {
    if (!generador.loop()) generador.stop();
  }
}

// Detecta si algún dedo se mantiene sobre UMBRAL_FLEX durante TIEMPO_MANTENER
// e imprime/reproduce su palabra asociada. Solo un dedo puede tener el turno a la vez.
void procesarDedos(int valores[]) {
  unsigned long ahora = millis();
  for (int i = 0; i < NUM_SENSORES; i++) {
    if (valores[i] >= UMBRAL_FLEX) {
      if (dedoActivo == -1 || dedoActivo == i) {
        dedoActivo = i;
        if (tiempoInicioSobreUmbral[i] == 0) {
          tiempoInicioSobreUmbral[i] = ahora;
        } else if (!publicado[i] && (ahora - tiempoInicioSobreUmbral[i] >= TIEMPO_MANTENER)) {
          Serial.println(PALABRAS[i]);
          reproducirPalabra(i);
          publicado[i] = true;
        }
      }
      // si otro dedo ya tiene el turno, este se ignora por completo
    } else {
      if (dedoActivo == i) dedoActivo = -1;
      tiempoInicioSobreUmbral[i] = 0;
      publicado[i] = false;
    }
  }
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM_SENSORES; i++) {
    pinMode(pines[i], INPUT);
  }

  Wire.begin(pinSDA, pinSCL);
  mpuEscribirRegistro(0x6B, 0x00); // PWR_MGMT_1: despierta el MPU6050

  delay(100); // margen tras despertar antes de calibrar
  calibrarMPU();

  // dma_buf_count subido de 8 (default) a 32: más margen contra underruns de DMA.
  salidaAudio = new AudioOutputI2S(0, AudioOutputI2S::EXTERNAL_I2S, 32);
  salidaAudio->SetPinout(pinI2S_BCLK, pinI2S_LRC, pinI2S_DIN);
  salidaAudio->SetGain(2.0); // máximo permitido; por encima se clampea igual a 4.0
}

void loop() {
  unsigned long ahora = millis();
  if (ahora - ultimoEnvio < INTERVALO_ENVIO) return;
  ultimoEnvio = ahora;

  int valores[NUM_SENSORES];
  for (int i = 0; i < NUM_SENSORES; i++) {
    valores[i] = analogRead(pines[i]);
  }

  int16_t ax, az;
  mpuLeerAccel(ax, az); // se sigue midiendo/calibrando, sin publicar por ahora
  (void)ax;
  (void)az;

  procesarDedos(valores);
}
