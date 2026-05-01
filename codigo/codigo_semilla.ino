/*
 * TEMA: SISTEMA DE CONTROL PARA CÉLULA ROBÓTICA
 * Estudiante: [MATEO ROCA]
 * 
 * Versión simplificada usando solo IF y números para los estados.
 */

// Pines para los LEDs
int ledProceso = 2;   // LED 5 segundos
int ledCalidad = 4;   // LED 3 segundos
int ledFinal = 5;     // LED Final

// Pines para los Botones
int btn1 = 13; // Detecta
int btn2 = 12; // Clasifica
int btn3 = 14; // Reinicia

// Variables de control (usamos números: 1, 2, 3, 4, 5, 6)
volatile int estado = 1; 
volatile int segundos = 0;

hw_timer_t * reloj = NULL;

// --- FUNCIONES DE INTERRUPCIÓN (Lo que pasa al tocar botones) ---

void IRAM_ATTR boton1() {
  if (estado == 1) { 
    estado = 2; // Si está en reposo, pasa a detección
  }
}

void IRAM_ATTR boton2() {
  if (estado == 2) { 
    estado = 3; // Si detectó, pasa a clasificar
  }
}

void IRAM_ATTR boton3() {
  if (estado == 6) { 
    estado = 1; // Si terminó, reinicia a reposo
  }
}

// --- FUNCIÓN DEL TIMER (Cuenta 1 segundo) ---
void IRAM_ATTR cuentaTiempo() {
  segundos++;
}

void setup() {
  Serial.begin(115200);

  pinMode(ledProceso, OUTPUT);
  pinMode(ledCalidad, OUTPUT);
  pinMode(ledFinal, OUTPUT);

  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);

  // Configurar interrupciones de los botones
  attachInterrupt(digitalPinToInterrupt(btn1), boton1, FALLING);
  attachInterrupt(digitalPinToInterrupt(btn2), boton2, FALLING);
  attachInterrupt(digitalPinToInterrupt(btn3), boton3, FALLING);

  // Configurar el Timer para que cuente cada 1 segundo
  reloj = timerBegin(1000000); 
  timerAttachInterrupt(reloj, &cuentaTiempo);
  timerAlarm(reloj, 1000000, true, 0); 
}

void loop() {

  // ESTADO 1: REPOSO
  if (estado == 1) {
    digitalWrite(ledProceso, LOW);
    digitalWrite(ledCalidad, LOW);
    digitalWrite(ledFinal, LOW);
    segundos = 0;
  }

  // ESTADO 2: DETECCIÓN
  if (estado == 2) {
    Serial.println("Pieza Detectada");
  }

  // ESTADO 3: CLASIFICACIÓN
  if (estado == 3) {
    Serial.println("Clasificando...");
    segundos = 0; // Ponemos el reloj en cero antes de empezar
    estado = 4;   // Pasamos a procesar automáticamente
  }

  // ESTADO 4: ENSAMBLAJE (Dura 5 segundos)
  if (estado == 4) {
    digitalWrite(ledProceso, HIGH);
    Serial.print("Ensamblando pieza: "); Serial.println(segundos);
    
    if (segundos >= 5) {
      digitalWrite(ledProceso, LOW);
      segundos = 0; // Limpiamos el tiempo para el siguiente paso
      estado = 5;   // Pasamos a inspección
    }
  }

  // ESTADO 5: CALIDAD (Dura 3 segundos)
  if (estado == 5) {
    digitalWrite(ledCalidad, HIGH);
    Serial.print("Inspeccionando calidad: "); Serial.println(segundos);
    
    if (segundos >= 3) {
      digitalWrite(ledCalidad, LOW);
      estado = 6; // Pasamos a listo
    }
  }

  // ESTADO 6: LISTO
  if (estado == 6) {
    digitalWrite(ledFinal, HIGH);
    Serial.println("Sistema Listo. Presione Reiniciar.");
  }
}
