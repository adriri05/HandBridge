#include <Arduino.h>

// Definición de los pines para el ESP32-C3 Mini (según schematics/v0-1-0.png)
const int pinIndice = 0; // GPIO0
const int pinMedio = 1;  // GPIO1
const int pinPulgar = 2; // GPIO2

const int NUM_SENSORES = 3;
const int pines[NUM_SENSORES] = { pinIndice, pinMedio, pinPulgar };

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Configuración de la resolución del ADC (la ESP32-C3 soporta hasta 12 bits: 0 - 4095)
  analogSetAttenuation(ADC_11db); // Rango de lectura de 0V hasta aprox. 2.5V - 3V

  for (int i = 0; i < NUM_SENSORES; i++) {
    pinMode(pines[i], INPUT);
  }
}

void loop() {
  int valores[NUM_SENSORES];
  for (int i = 0; i < NUM_SENSORES; i++) {
    valores[i] = analogRead(pines[i]);
  }

  // CSV: indice,medio,pulgar
  Serial.print(valores[0]);
  Serial.print(',');
  Serial.print(valores[1]);
  Serial.print(',');
  Serial.println(valores[2]);

  delay(200);
}
