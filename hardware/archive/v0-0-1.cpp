#include <Arduino.h>

// Definición de los pines para el ESP32-C3 Mini
const int pinIndice = 0; // GPIO0
const int pinMedio = 1;  // GPIO1
const int pinPulgar = 2; // GPIO2

void setup() {
  // Iniciar la comunicación serie a 115200 baudios (estándar para ESP32)
  Serial.begin(115200);
  
  // Configurar los pines analógicos como entradas
  pinMode(pinIndice, INPUT);
  pinMode(pinMedio, INPUT);
  pinMode(pinPulgar, INPUT);
  
  Serial.println("Iniciando lectura de sensores...");
}

void loop() {
  // 1. Leer los valores de los potenciómetros
  // En el ESP32, el valor irá de 0 (0V) a 4095 (3.3V)
  int valorIndice = analogRead(pinIndice);
  int valorMedio = analogRead(pinMedio);
  int valorPulgar = analogRead(pinPulgar);

  // 2. Mostrar los valores en el Monitor Serie
  Serial.print("Índice (GPIO0): ");
  Serial.print(valorIndice);
  
  Serial.print("  |  Medio (GPIO1): "); 
  Serial.print(valorMedio);
  
  Serial.print("  |  Pulgar (GPIO2): ");
  Serial.println(valorPulgar); // println añade un salto de línea al final

  // 3. Esperar 100 milisegundos antes de volver a leer
  // Esto evita saturar el Monitor Serie de datos
  delay(100); 
}