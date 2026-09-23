#include <Arduino.h>
#include <Wire.h>

// Definición de los pines para el ESP32-C3 Mini (según schematics/v0-1-0.png)
const int pinIndice = 0; // GPIO0
const int pinMedio = 1;  // GPIO1
const int pinPulgar = 2; // GPIO2

const int NUM_SENSORES = 3;
const int pines[NUM_SENSORES] = { pinIndice, pinMedio, pinPulgar };

// I2C para el MPU6050 (según schematics/v0-1-0.png)
const int pinSDA = 8; // GPIO8
const int pinSCL = 9; // GPIO9
const uint8_t MPU_ADDR = 0x68;

// Offsets de calibración del MPU6050 (calculados en setup)
int16_t axOffset = 0;
int16_t azOffset = 0;
const int MUESTRAS_CALIBRACION = 200;

// Intervalo entre envíos (ms)
const unsigned long INTERVALO_ENVIO = 50;
unsigned long ultimoEnvio = 0;

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

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM_SENSORES; i++) {
    pinMode(pines[i], INPUT);
  }

  Wire.begin(pinSDA, pinSCL);
  mpuEscribirRegistro(0x6B, 0x00); // PWR_MGMT_1: despierta el MPU6050

  delay(100); // margen tras despertar antes de calibrar
  calibrarMPU();
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
  mpuLeerAccel(ax, az);

  // CSV: indice,medio,pulgar,ax,az
  Serial.print(valores[0]);
  Serial.print(',');
  Serial.print(valores[1]);
  Serial.print(',');
  Serial.print(valores[2]);
  Serial.print(',');
  Serial.print(ax);
  Serial.print(',');
  Serial.println(az);

  delay(500);
}
