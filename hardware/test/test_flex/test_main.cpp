#include <Arduino.h>

const int flexPin = 0; // Pin analógico GPIO 0 conectado al divisor de tensión

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Configuración de la resolución del ADC (la ESP32-C3 soporta hasta 12 bits: 0 - 4095)
  analogSetAttenuation(ADC_11db); // Rango de lectura de 0V hasta aprox. 2.5V - 3V
}

void loop() {
  int flexValue = analogRead(flexPin);

  Serial.print("Valor crudo del sensor: ");
  Serial.println(flexValue);

  delay(200);
}
