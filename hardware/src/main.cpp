#include <Arduino.h>

// Definición de los pines para el ESP32-C3 Mini
const int pinIndice = 0; // GPIO0
const int pinMedio = 1;  // GPIO1
const int pinPulgar = 2; // GPIO2

const int NUM_POTES = 3;
const int pines[NUM_POTES] = { pinIndice, pinMedio, pinPulgar };

// Cada tramo (bajo/medio/alto) de cada potenciómetro manda un mensaje fijo
const char* mensajes[NUM_POTES][3] = {
  { "Hola",  "Adios",  "Si" },     // pinIndice
  { "No",    "Vale",   "Hambre" }, // pinMedio
  { "Sed",   "Bano",   "Luz" }     // pinPulgar
};

// Umbrales del ADC (0-4095) divididos en 3 tramos iguales
const int UMBRAL_BAJO_MEDIO = 1365;
const int UMBRAL_MEDIO_ALTO = 2730;

// Último tramo enviado por cada potenciómetro (-1 = ninguno todavía)
int ultimoTramo[NUM_POTES] = { -1, -1, -1 };

// Tramo candidato mientras se espera a que se estabilice, y desde cuándo
int tramoCandidato[NUM_POTES] = { -1, -1, -1 };
unsigned long inicioCandidato[NUM_POTES] = { 0, 0, 0 };

// Tiempo que un tramo debe mantenerse estable antes de enviarse (ms)
const unsigned long TIEMPO_ESTABILIZACION = 250;

int obtenerTramo(int valorAnalogico) {
  if (valorAnalogico < UMBRAL_BAJO_MEDIO) return 0;
  if (valorAnalogico < UMBRAL_MEDIO_ALTO) return 1;
  return 2;
}

void setup() {
  Serial.begin(115200);

  pinMode(pinIndice, INPUT);
  pinMode(pinMedio, INPUT);
  pinMode(pinPulgar, INPUT);

  Serial.println("Iniciando lectura de sensores...");
}

void loop() {
  unsigned long ahora = millis();
  int tramoActual[NUM_POTES];
  int indicesCambiados[NUM_POTES];
  int numCambios = 0;

  // 1. Leer cada potenciómetro y calcular su tramo actual
  for (int i = 0; i < NUM_POTES; i++) {
    int valor = analogRead(pines[i]);
    tramoActual[i] = obtenerTramo(valor);

    if (tramoActual[i] != tramoCandidato[i]) {
      // Nuevo tramo candidato: reinicia el contador de estabilización
      tramoCandidato[i] = tramoActual[i];
      inicioCandidato[i] = ahora;
    }

    // Solo se considera "cambio" si el tramo lleva estable el tiempo mínimo
    // y es distinto del último tramo confirmado (evita pasar por tramos intermedios)
    if (tramoCandidato[i] != ultimoTramo[i] &&
        ahora - inicioCandidato[i] >= TIEMPO_ESTABILIZACION) {
      indicesCambiados[numCambios] = i;
      numCambios++;
    }
  }

  // 2. Solo mandar mensaje si ha cambiado exactamente un potenciómetro
  if (numCambios == 1) {
    int i = indicesCambiados[0];
    Serial.println(mensajes[i][tramoCandidato[i]]);
  }

  // 3. Actualizar el estado de los potenciómetros que cambiaron
  for (int j = 0; j < numCambios; j++) {
    int i = indicesCambiados[j];
    ultimoTramo[i] = tramoCandidato[i];
  }

  delay(20);
}
