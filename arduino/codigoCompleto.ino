#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

const byte ledSala = 13;
const byte ledCocina = 12;
const byte ledComedor = 8;
const byte ledHabitacion = 4;
const byte ledBano = 2;
const byte motorPin = 5;
const byte botonPuerta = 7;

#define MAX_ESCENAS 4
#define MAX_PASOS 25
#define EEPROM_INICIO_ESCENAS 100
#define TAMANO_POR_ESCENA 200
const int DIR_ULTIMA_ACTIVA = 10;

const int EEPROM_BUFFER_TEMP = 900; 

int totalPasosBuffer = 0; 
String nombreEscenaActiva = "Manual";
bool escenaActiva = false;

// Variables de estado
String escenaEstado = "Manual"; 

bool modoCarga = false;
String comando;
String nombreNuevaEscena = "";
String fanEstado = "OFF";

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo miServo;

bool estadoPuerta = false;
bool estadoBotonAnterior = HIGH;

// CONFIGURACION INICIAL DEL HARDWARE Y RECUPERACION DE ESTADO
void setup() {
  Serial.begin(9600);
  comando.reserve(30); 

  lcd.init();
  lcd.backlight();
 
  lcd.setCursor(0, 0); lcd.print("HOME");
  lcd.setCursor(0, 1); lcd.print("INICIANDO...");

  pinMode(ledSala, OUTPUT);
  pinMode(ledCocina, OUTPUT);
  pinMode(ledComedor, OUTPUT);
  pinMode(ledHabitacion, OUTPUT);
  pinMode(ledBano, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(botonPuerta, INPUT_PULLUP);

  miServo.attach(3);
  miServo.write(0);

  
  Serial.println(F("BIENVENIDO A LA CASA INTELIGENTE"));
  
  char nombreGuardado[13]; 
  for (int i = 0; i < 12; i++) nombreGuardado[i] = EEPROM.read(DIR_ULTIMA_ACTIVA + i);
  nombreGuardado[12] = '\0'; 

  if (isalnum(nombreGuardado[0])) {
    String nombreStr = String(nombreGuardado);
    Serial.print(F("Recuperando: ")); Serial.println(nombreStr);
    int slot = buscarSlotPorNombre(nombreStr);
    if (slot != -1) {
      cargarNombreEscena(slot);
      ejecutarEscena();
    }
  }
}

// BUCLE PRINCIPAL DE LECTURA DE COMANDOS Y BOTONES
void loop() {
  if (Serial.available() > 0) {
    String lineaRaw = Serial.readStringUntil('\n');
    comando = lineaRaw;
    comando.trim();
    comando.toUpperCase(); 

    if (comando.length() > 0) {

      if (modoCarga) {
        procesarLineaEscena(comando);
      } else {
    
        
        // SALA
        if (comando == "L1" || comando == "L1ON") encenderLed(ledSala, "SALA");
        else if (comando == "L1OFF") apagarLed(ledSala, "SALA");
        
        // COMEDOR
        else if (comando == "L2" || comando == "L2ON") encenderLed(ledComedor, "COMEDOR");
        else if (comando == "L2OFF") apagarLed(ledComedor, "COMEDOR");
        
        // COCINA
        else if (comando == "L3" || comando == "L3ON") encenderLed(ledCocina, "COCINA");
        else if (comando == "L3OFF") apagarLed(ledCocina, "COCINA");
        
        // BAÑO
        else if (comando == "L4" || comando == "L4ON") encenderLed(ledBano, "BANO");
        else if (comando == "L4OFF") apagarLed(ledBano, "BANO");
        
        // HABITACION
        else if (comando == "L5" || comando == "L5ON") encenderLed(ledHabitacion, "HABITACION");
        else if (comando == "L5OFF") apagarLed(ledHabitacion, "HABITACION");
        
        // GLOBAL
        else if (comando == "ALLON") encenderLeds();
        else if (comando == "ALLOFF") apagarLeds();
        
        // PUERTA
        else if (comando == "DOOROPEN") abrirPuerta();
        else if (comando == "DOORCLOSE") cerrarPuerta();
        else if (comando == "DOOR") alternarPuerta();
        
        // VENTILADOR
        else if (comando == "FAN0") manejarVentilador(0, "OFF");
        else if (comando == "FAN1") manejarVentilador(85, "BAJO");
        else if (comando == "FAN2") manejarVentilador(170, "MEDIO");
        else if (comando == "FAN3") manejarVentilador(255, "ALTO");

        // ESCENAS
        else if (comando == "LOAD_SCENE") {
          modoCarga = true;
          totalPasosBuffer = 0;
          Serial.println(F("MODO CARGA"));
          Serial.println(F("Envie lineas. Finalice con: END_LOAD NOMBRE"));
          // Sin F en LCD
          lcd.clear(); lcd.print("Cargando...");
        } 
        else if (comando == "LIST_SCENE") listarEscenas();
        else if (comando == "ERASE_SCENES") borrarTodasLasEscenas();
        else if (comando.startsWith("SHOW_SCENE")) {
          mostrarEscena(comando.substring(11));
        } 
        else if(comando == "STOP"){
          escenaActiva = false;
          nombreEscenaActiva = "Manual";
          escenaEstado = "Manual";
          Serial.println(F("ESCENA DETENIDA"));
          lcd.clear(); lcd.print("Escena: Manual");
        } 
        else if (comando == "STATUS") {
           imprimirStatus();
        }
        else if (comando == "RESET") {
           resetSistema();
        }
        else if (comando == "VERSION") {
           Serial.println(F("Casa Dic 2025 ,Grupo 5 conformado por:"));
           Serial.println(F("José Antonio García Roca - 202401166"));
           Serial.println(F("Esmeralda Del Rosario Guillén Veliz - 201901002"));
           Serial.println(F("Nelson Andres Santa Cruz Gil - 202202728"));
           Serial.println(F("Angel Emanuel Rodriguez Corado - 202404856"));
           Serial.println(F("Pablo Daniel Solares Samayoa - 202400044"));
        }
        else {
          int slot = buscarSlotPorNombre(comando); 
          if (slot != -1) {
            Serial.print(F("Cargando escena slot: ")); Serial.println(slot);
            cargarNombreEscena(slot); 
            ejecutarEscena(); 
          } else {
            Serial.println(F("COMANDO INEXISTENTE"));
            lcd.clear(); lcd.print("ERROR COMANDO");
          }
        }
      }
    }
  }

  // BOTON PUERTA
  bool estadoBotonActual = digitalRead(botonPuerta);
  if (estadoBotonAnterior == HIGH && estadoBotonActual == LOW) {
    alternarPuerta();
    delay(200);
  }
  estadoBotonAnterior = estadoBotonActual;
}

// --- GESTIÓN DE ESCENAS ---

// INTERPRETA COMANDOS DE CARGA Y GUARDA EN BUFFER TEMPORAL
void procesarLineaEscena(String linea) {
  linea.trim();
 
  if (linea.startsWith("END_LOAD")) {
     int espacio = linea.indexOf(' ');
     if (espacio != -1) {
        nombreNuevaEscena = linea.substring(espacio + 1);
        nombreNuevaEscena.trim();
        nombreNuevaEscena.toUpperCase(); 
        
        int slot = buscarSlotPorNombre(nombreNuevaEscena);
        if (slot == -1) {
           slot = buscarSlotVacio();
           if (slot == -1) {
              Serial.println(F("ERROR: MEMORIA LLENA"));
              modoCarga = false;
              return;
           }
        } 
        guardarEscenaDesdeBuffer(slot, nombreNuevaEscena);
        
        Serial.print(F("ESCENA GUARDADA: ")); Serial.println(nombreNuevaEscena);
        lcd.clear(); lcd.print("GUARDADA:"); lcd.setCursor(0,1); lcd.print(nombreNuevaEscena);
     } else {
        Serial.println(F("ERROR: Falta nombre"));
     }
     modoCarga = false;
     return;
  }

  if (linea.startsWith("#") || linea.length() == 0) return;
  
  String lineaUpper = linea;
  lineaUpper.toUpperCase();

  int pos1 = lineaUpper.indexOf(':');
  int pos2 = lineaUpper.indexOf(':', pos1 + 1);
  int pos3 = lineaUpper.indexOf(':', pos2 + 1);

  if (pos1 > 0 && pos2 > pos1 && pos3 > pos2 && totalPasosBuffer < MAX_PASOS) {
    String amb = lineaUpper.substring(0, pos1);
    String est = lineaUpper.substring(pos1 + 1, pos2);
    int dur = linea.substring(pos2 + 1, pos3).toInt(); 
    int rep = linea.substring(pos3 + 1).toInt();

    byte pin = getPinFromAmbiente(amb);
    if (pin > 0) {
      // Guardar en BUFFER de EEPROM
      int addr = EEPROM_BUFFER_TEMP + (totalPasosBuffer * 5);
      EEPROM.update(addr, pin);
      EEPROM.update(addr + 1, (est == "ON"));
      EEPROM.update(addr + 2, lowByte(dur));
      EEPROM.update(addr + 3, highByte(dur));
      EEPROM.update(addr + 4, rep);
      
      totalPasosBuffer++;
      Serial.print("Paso OK: " ); Serial.print(amb); Serial.print(F(" ")); Serial.println(est); 
   
    } else {
      Serial.println(F("ERROR: Formato incorrecto"));
    }
  }
}

// MUEVE DATOS DEL BUFFER TEMPORAL A UN SLOT PERMANENTE
void guardarEscenaDesdeBuffer(int slot, String nombre) {
  int dirBase = calcularDireccionSlot(slot);
  
  char nombreArr[12];
  nombre.toCharArray(nombreArr, 12);
  for(int i=0; i<12; i++) EEPROM.update(dirBase + i, nombreArr[i]);
  
  EEPROM.put(dirBase + 12, totalPasosBuffer);
  
  int dirPasos = dirBase + 14;
  for (int i = 0; i < totalPasosBuffer; i++) {
    int addrDest = dirPasos + (i * 5);
    int addrSrc = EEPROM_BUFFER_TEMP + (i * 5);
    for(int j=0; j<5; j++){
       EEPROM.update(addrDest + j, EEPROM.read(addrSrc + j));
    }
  }
}

// CARGA EL NOMBRE DE LA ESCENA ACTIVA SIN USAR RAM
bool cargarNombreEscena(int slot) {
  int dirBase = calcularDireccionSlot(slot);
  char nombreArr[13]; 
  for(int i=0; i<12; i++) nombreArr[i] = EEPROM.read(dirBase + i);
  nombreArr[12] = '\0';
  nombreEscenaActiva = String(nombreArr);
  escenaEstado = nombreEscenaActiva;
  
  for(int i=0; i<12; i++) EEPROM.update(DIR_ULTIMA_ACTIVA + i, nombreArr[i]);
  return true;
}

// REPRODUCE LA ESCENA LEYENDO DIRECTAMENTE DE LA EEPROM
void ejecutarEscena() {
  escenaActiva = true;
  Serial.print(F("EJECUTANDO: ")); Serial.println(nombreEscenaActiva);
  lcd.clear(); lcd.print("RUN:"); lcd.setCursor(4,0); lcd.print(nombreEscenaActiva);

  int slot = buscarSlotPorNombre(nombreEscenaActiva);
  if(slot == -1) return;

  int dirBase = calcularDireccionSlot(slot);
  int totalPasosEnSlot = 0;
  EEPROM.get(dirBase + 12, totalPasosEnSlot);
  
  if(totalPasosEnSlot > MAX_PASOS) totalPasosEnSlot = 0;

  int dirPasos = dirBase + 14;

  for (int i = 0; i < totalPasosEnSlot; i++) {
    if(Serial.available() > 0){
       Serial.println(F("INTERRUMPIDO"));
       escenaActiva = false;
       return; 
    }

    int addr = dirPasos + (i * 5);
    byte p_pin = EEPROM.read(addr);
    bool p_estado = EEPROM.read(addr + 1);
    unsigned int p_dur = word(EEPROM.read(addr + 3), EEPROM.read(addr + 2));
    byte p_rep = EEPROM.read(addr + 4);

    Serial.print(F(">> Paso ")); Serial.print(i+1); Serial.print(F(": "));
    imprimirNombreAmbiente(p_pin);
    Serial.print(F(" -> ")); Serial.println(p_estado ? F("ON") : F("OFF"));

    int rep = (p_rep > 0) ? p_rep : 1;
    for (int r = 0; r < rep; r++) {
       if(Serial.available() > 0){
         Serial.println(F("INTERRUMPIDO"));
         escenaActiva = false;
         return; 
       }

       if (p_estado) { 
          
         
            digitalWrite(p_pin, HIGH);
            delay(p_dur);
            digitalWrite(p_pin, LOW);
            delay(p_dur);
          
          
       } else { 
         
            digitalWrite(p_pin, LOW);
            delay(p_dur << 1);
          
       }
    }
  }
  Serial.println(F("FIN ESCENA"));
  lcd.clear(); lcd.print("FIN ESCENA");
  escenaActiva = false;
}

// --- FUNCIONES AUXILIARES ---

// IMPRIME EL NOMBRE DEL AMBIENTE SEGUN EL PIN
void imprimirNombreAmbiente(byte pin) {
  if (pin == ledSala) Serial.print(F("SALA"));
  else if (pin == ledComedor) Serial.print(F("COMEDOR"));
  else if (pin == ledCocina) Serial.print(F("COCINA"));
  else if (pin == ledBano) Serial.print(F("BANO"));
  else if (pin == ledHabitacion) Serial.print(F("HABITACION"));
  else { Serial.print(F("PIN_")); Serial.print(pin); }
}

// CALCULA LA DIRECCION DE MEMORIA DE UN SLOT
int calcularDireccionSlot(int slot) {
  return EEPROM_INICIO_ESCENAS + (slot * TAMANO_POR_ESCENA);
}

// BUSCA SI EXISTE UNA ESCENA CON ESE NOMBRE
int buscarSlotPorNombre(String nombre) {
  nombre.toUpperCase();
  char buffer[13]; 
  for(int i=0; i<MAX_ESCENAS; i++) {
     int dirBase = calcularDireccionSlot(i);
     if (EEPROM.read(dirBase) == 0 || EEPROM.read(dirBase) == 255) continue;
     
     for(int j=0; j<12; j++) buffer[j] = EEPROM.read(dirBase + j);
     buffer[12] = '\0'; 
     if (nombre.equalsIgnoreCase(buffer)) return i; 
  }
  return -1; 
}

// BUSCA UN ESPACIO LIBRE EN LA MEMORIA
int buscarSlotVacio() {
  for(int i=0; i<MAX_ESCENAS; i++) {
     int dirBase = calcularDireccionSlot(i);
     byte primerByte = EEPROM.read(dirBase);
     if (primerByte == 0 || primerByte == 255) return i;
  }
  return -1; 
}

// MUESTRA TODAS LAS ESCENAS GUARDADAS
void listarEscenas() {
  Serial.println(F("ESCENAS:"));
  char buffer[13];
  bool hay = false;
  for(int i=0; i<MAX_ESCENAS; i++) {
     int dirBase = calcularDireccionSlot(i);
     if (EEPROM.read(dirBase) != 255 && EEPROM.read(dirBase) != 0) {
        for(int j=0; j<12; j++) buffer[j] = EEPROM.read(dirBase + j);
        buffer[12] = '\0';
        Serial.println(buffer);
        hay = true;
     }
  }
  if(!hay) Serial.println(F("SIN ESCENAS"));
}

// BORRA TODA LA MEMORIA EEPROM
void borrarTodasLasEscenas() {
  Serial.println(F("Borrando memoria..."));
  for (int i = 0; i < EEPROM.length(); i++) EEPROM.update(i, 255);
  Serial.println(F("EEPROM LIMPIA"));
  lcd.clear(); lcd.print("EEPROM LIMPIA");
}

// CONVIERTE NOMBRE DE AMBIENTE A NUMERO DE PIN
byte getPinFromAmbiente(String amb) {
  if (amb.indexOf("SALA") != -1) return ledSala;
  if (amb.indexOf("COMEDOR") != -1) return ledComedor;
  if (amb.indexOf("COCINA") != -1) return ledCocina;
  if (amb.indexOf("BANO") != -1 || amb.indexOf("BAÑO") != -1) return ledBano;
  if (amb.indexOf("HABITACION") != -1) return ledHabitacion;
  return 0;
}

// MUESTRA LOS DETALLES DE UNA ESCENA PASO A PASO
void mostrarEscena(String nombre) {
  int slot = buscarSlotPorNombre(nombre);
  if (slot < 0) { Serial.println(F("NO EXISTE")); return; }
  
  int base = calcularDireccionSlot(slot);
  char nombreArr[13];
  for(int i=0; i<12; i++) nombreArr[i] = EEPROM.read(base+i);
  nombreArr[12]='\0';
  
  int total = EEPROM.read(base+12);
  
  Serial.print(F("ESCENA: ")); Serial.println(nombreArr);
  Serial.print(F("PASOS: ")); Serial.println(total);

  int dirPasos = base + 14;

  for (int i = 0; i < total; i++) {
    int addr = dirPasos + (i * 5);
    byte p_pin = EEPROM.read(addr);
    bool p_estado = EEPROM.read(addr + 1);
    unsigned int p_dur = word(EEPROM.read(addr + 3), EEPROM.read(addr + 2));
    byte p_rep = EEPROM.read(addr + 4);

    Serial.print(i + 1); Serial.print(F(") "));
    imprimirNombreAmbiente(p_pin);
    Serial.print(F(":"));
    Serial.print(p_estado ? F("ON") : F("OFF"));
    Serial.print(F(":"));
    Serial.print(p_dur);
    Serial.print(F("ms:x"));
    Serial.println(p_rep);
  }
}

// ENCIENDE UN LED ESPECIFICO
void encenderLed(int pin, const char* nombre) {
  digitalWrite(pin, HIGH);
  Serial.print(nombre); Serial.println(F(": ON"));
  lcd.clear(); lcd.print(nombre); lcd.print(": ON");
  escenaEstado = "Manual";
}

// APAGA UN LED ESPECIFICO
void apagarLed(int pin, const char* nombre) {
  digitalWrite(pin, LOW);
  Serial.print(nombre); Serial.println(F(": OFF"));
  lcd.clear(); lcd.print(nombre); lcd.print(": OFF");
  escenaEstado = "Manual";
}

// ENCIENDE TODAS LAS LUCES
void encenderLeds() {
  digitalWrite(ledSala, HIGH); digitalWrite(ledComedor, HIGH);
  digitalWrite(ledCocina, HIGH); digitalWrite(ledBano, HIGH); digitalWrite(ledHabitacion, HIGH);
  Serial.println(F("TODAS LAS LUCES: ON"));
  lcd.clear(); lcd.print("Todo Encendido");
  escenaEstado = "Manual";
}

// APAGA TODAS LAS LUCES
void apagarLeds() {
  digitalWrite(ledSala, LOW); digitalWrite(ledComedor, LOW);
  digitalWrite(ledCocina, LOW); digitalWrite(ledBano, LOW); digitalWrite(ledHabitacion, LOW);
  Serial.println(F("TODAS LAS LUCES: OFF"));
  lcd.clear(); lcd.print("Todo Apagado");
  escenaEstado = "Manual";
}

// ABRE LA PUERTA CON EL SERVO
void abrirPuerta() {
  if (!estadoPuerta) {
    miServo.write(90);
    delay(600);
    estadoPuerta = true;
    Serial.println(F("PUERTA ABIERTA"));
    lcd.clear(); lcd.print("PUERTA: ABIERTA");
  }
}

// CIERRA LA PUERTA CON EL SERVO
void cerrarPuerta() {
  if (estadoPuerta) {
    miServo.write(0);
    delay(600);
    estadoPuerta = false;
    Serial.println(F("PUERTA CERRADA"));
    lcd.clear(); lcd.print("PUERTA: CERRADA");
  }
}

// ALTERNA EL ESTADO DE LA PUERTA
void alternarPuerta() {
  if (estadoPuerta) cerrarPuerta(); else abrirPuerta();
}

// CONTROLA LA VELOCIDAD DEL VENTILADOR
void manejarVentilador(int velocidad, const char* nombreVel) {
  analogWrite(motorPin, velocidad);
  fanEstado = nombreVel; 
  Serial.print(F("VENTILADOR: ")); Serial.println(nombreVel);
  lcd.clear(); lcd.setCursor(0, 0); lcd.print("FAN: "); lcd.print(nombreVel);
}

// MUESTRA EL ESTADO ACTUAL DE TODOS LOS COMPONENTES
void imprimirStatus() {
  Serial.print(F("SALA:"));    Serial.println(digitalRead(ledSala) ? F("ON") : F("OFF"));
  Serial.print(F("COMEDOR:")); Serial.println(digitalRead(ledComedor) ? F("ON") : F("OFF"));
  Serial.print(F("COCINA:"));  Serial.println(digitalRead(ledCocina) ? F("ON") : F("OFF"));
  Serial.print(F("BAÑO:"));    Serial.println(digitalRead(ledBano) ? F("ON") : F("OFF"));
  Serial.print(F("HAB:"));     Serial.println(digitalRead(ledHabitacion) ? F("ON") : F("OFF"));
  Serial.print(F("FAN:"));     Serial.println(fanEstado);
  Serial.print(F("PUERTA:"));  Serial.println(estadoPuerta ? F("ABIERTA") : F("CERRADA"));
  Serial.print(F("ESCENA:"));  Serial.println(escenaEstado);
}

// REINICIA EL SISTEMA A SU ESTADO INICIAL
void resetSistema() {
  apagarLeds();
  manejarVentilador(0, "OFF");
  cerrarPuerta();
  escenaActiva = false;
  nombreEscenaActiva = "Manual";
  escenaEstado = "Manual";
  
  Serial.println(F("SISTEMA REINICIADO"));
  lcd.clear();
  lcd.print("Sistema");
  lcd.setCursor(0, 1);
  lcd.print("Reiniciado");
}

