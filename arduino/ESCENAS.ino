#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// DEFINICION DE PINES
const byte ledSala = 13;
const byte ledCocina = 12;
const byte ledComedor = 8;
const byte ledHabitacion = 4;
const byte ledBano = 2;
const byte motorPin = 5;
const byte botonPuerta = 7;

// CONFIGURACION DE MEMORIA
#define MAX_ESCENAS 4
#define MAX_PASOS 25
#define EEPROM_INICIO_ESCENAS 100
#define TAMANO_POR_ESCENA 200
const int DIR_ULTIMA_ACTIVA = 10;

// BUFFER TEMPORAL EEPROM
const int EEPROM_BUFFER_TEMP = 900; 

int totalPasosBuffer = 0; 
String nombreEscenaActiva = "Manual";
bool escenaActiva = false;

// VARIABLES DE ESTADO
bool modoCarga = false;
String comando;
String nombreNuevaEscena = "";
String escenaEstado = "Manual";

// OBJETOS
LiquidCrystal_I2C lcd(0x20, 16, 2);
Servo miServo;

void setup() {
  // INICIAR COMUNICACION
  Serial.begin(9600);
  comando.reserve(30); 

  // INICIAR LCD
  lcd.init();
  lcd.backlight();
 
  lcd.setCursor(0, 0); lcd.print("HOME");
  lcd.setCursor(0, 1); lcd.print("INICIANDO...");

  // CONFIGURAR SALIDAS
  pinMode(ledSala, OUTPUT);
  pinMode(ledCocina, OUTPUT);
  pinMode(ledComedor, OUTPUT);
  pinMode(ledHabitacion, OUTPUT);
  pinMode(ledBano, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(botonPuerta, INPUT_PULLUP);

  // INICIAR SERVO
  miServo.attach(3, 400, 1600);
  miServo.write(0);

  
  Serial.println(F("BIENVENIDO A LA CASA INTELIGENTE"));
  
  // RECUPERAR ULTIMA ESCENA
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

void loop() {
  if (Serial.available() > 0) {
    // LEER COMANDO
    String lineaRaw = Serial.readStringUntil('\n');
    comando = lineaRaw;
    comando.trim();
    comando.toUpperCase(); 

    if (comando.length() > 0) {

      if (modoCarga) {
        // PROCESAR CARGA DE ARCHIVO
        procesarLineaEscena(comando);
      } else {
    
        // COMANDOS DE GESTION
         if (comando == "LOAD_SCENE") {
          modoCarga = true;
          totalPasosBuffer = 0;
          Serial.println(F("MODO CARGA"));
          Serial.println(F("Envie lineas. Finalice con: END_LOAD NOMBRE"));
          
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
        } 
        else {
          
          // BUSCAR Y EJECUTAR ESCENA
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
}

// --- GESTION DE ESCENAS ---

void procesarLineaEscena(String linea) {
  linea.trim();
 
  // FINALIZAR CARGA
  if (linea.startsWith("END_LOAD")) {
     int espacio = linea.indexOf(' ');
     if (espacio != -1) {
        nombreNuevaEscena = linea.substring(espacio + 1);
        nombreNuevaEscena.trim();
        nombreNuevaEscena.toUpperCase(); 
        
        // BUSCAR ESPACIO
        int slot = buscarSlotPorNombre(nombreNuevaEscena);
        if (slot == -1) {
           slot = buscarSlotVacio();
           if (slot == -1) {
              Serial.println(F("ERROR: MEMORIA LLENA"));
              modoCarga = false;
              return;
           }
        } 
        // GUARDAR DEFINITIVAMENTE
        guardarEscenaDesdeBuffer(slot, nombreNuevaEscena);
        
        Serial.print(F("ESCENA GUARDADA: ")); Serial.println(nombreNuevaEscena);
        lcd.clear(); lcd.print("GUARDADA:"); lcd.setCursor(0,1); lcd.print(nombreNuevaEscena);
     } else {
        Serial.println(F("ERROR: Falta nombre"));
     }
     modoCarga = false;
     return;
  }

  // IGNORAR COMENTARIOS
  if (linea.startsWith("#") || linea.length() == 0) return;
  
  String lineaUpper = linea;
  lineaUpper.toUpperCase();

  // PARSEAR DATOS
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
      // GUARDAR EN BUFFER TEMPORAL
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

// MOVER DE BUFFER A SLOT DEFINITIVO
void guardarEscenaDesdeBuffer(int slot, String nombre) {
  int dirBase = calcularDireccionSlot(slot);
  
  // GUARDAR NOMBRE
  char nombreArr[12];
  nombre.toCharArray(nombreArr, 12);
  for(int i=0; i<12; i++) EEPROM.update(dirBase + i, nombreArr[i]);
  
  // GUARDAR CONTEO DE PASOS
  EEPROM.put(dirBase + 12, totalPasosBuffer);
  
  // COPIAR DATOS
  int dirPasos = dirBase + 14;
  for (int i = 0; i < totalPasosBuffer; i++) {
    int addrDest = dirPasos + (i * 5);
    int addrSrc = EEPROM_BUFFER_TEMP + (i * 5);
    for(int j=0; j<5; j++){
       EEPROM.update(addrDest + j, EEPROM.read(addrSrc + j));
    }
  }
}

// CARGAR SOLO NOMBRE PARA AHORRAR RAM
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

// EJECUTAR LEYENDO DIRECTAMENTE DE EEPROM
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

  // BUCLE DE PASOS
  for (int i = 0; i < totalPasosEnSlot; i++) {
    if(Serial.available() > 0){
       Serial.println(F("INTERRUMPIDO"));
       escenaActiva = false;
       return; 
    }

    // LEER PASO ACTUAL
    int addr = dirPasos + (i * 5);
    byte p_pin = EEPROM.read(addr);
    bool p_estado = EEPROM.read(addr + 1);
    unsigned int p_dur = word(EEPROM.read(addr + 3), EEPROM.read(addr + 2));
    byte p_rep = EEPROM.read(addr + 4);

    Serial.print(F(">> Paso ")); Serial.print(i+1); Serial.print(F(": "));
    imprimirNombreAmbiente(p_pin);
    Serial.print(F(" -> ")); Serial.println(p_estado ? F("ON") : F("OFF"));

    // EJECUTAR REPETICIONES
    int rep = (p_rep > 0) ? p_rep : 1;
    for (int r = 0; r < rep; r++) {
       if(Serial.available() > 0){
         Serial.println(F("INTERRUMPIDO"));
         escenaActiva = false;
         return; 
       }

       if (p_estado) { 
         // SI EL PASO ES EL VENTILADOR
         if(p_pin == motorPin){
           analogWrite(p_pin, 170);
           delay(p_dur);
           analogWrite(p_pin, 0);
           delay(p_dur);
         } else {
           digitalWrite(p_pin, HIGH);
           delay(p_dur);
           digitalWrite(p_pin, LOW);
           delay(p_dur);
         }
          
       } else { 
         if(p_pin == motorPin){
           analogWrite(p_pin, 0);
           delay(p_dur << 1); 
         } else {
           digitalWrite(p_pin, LOW);
           delay(p_dur << 1);
         }
       }
    }
  }
  Serial.println(F("FIN ESCENA"));
  lcd.clear(); lcd.print("FIN ESCENA");
  escenaActiva = false;
}


// IMPRIMIR NOMBRE DE PINES
void imprimirNombreAmbiente(byte pin) {
  if (pin == ledSala) Serial.print(F("SALA"));
  else if (pin == ledComedor) Serial.print(F("COMEDOR"));
  else if (pin == ledCocina) Serial.print(F("COCINA"));
  else if (pin == ledBano) Serial.print(F("BANO"));
  else if (pin == ledHabitacion) Serial.print(F("HABITACION"));
  else if (pin == motorPin) Serial.print(F("VENTILADOR"));
  else { Serial.print(F("PIN_")); Serial.print(pin); }
}

// CALCULAR DIRECCION MEMORIA
int calcularDireccionSlot(int slot) {
  return EEPROM_INICIO_ESCENAS + (slot * TAMANO_POR_ESCENA);
}

// BUSCAR SLOT POR NOMBRE
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

// BUSCAR SLOT VACIO
int buscarSlotVacio() {
  for(int i=0; i<MAX_ESCENAS; i++) {
     int dirBase = calcularDireccionSlot(i);
     byte primerByte = EEPROM.read(dirBase);
     if (primerByte == 0 || primerByte == 255) return i;
  }
  return -1; 
}

// LISTAR ESCENAS
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

// BORRAR MEMORIA
void borrarTodasLasEscenas() {
  Serial.println(F("Borrando memoria..."));
  for (int i = 0; i < EEPROM.length(); i++) EEPROM.update(i, 255);
  Serial.println(F("EEPROM LIMPIA"));
  lcd.clear(); lcd.print("EEPROM LIMPIA");
}

// OBTENER PIN POR NOMBRE
byte getPinFromAmbiente(String amb) {
  if (amb.indexOf("SALA") != -1) return ledSala;
  if (amb.indexOf("COMEDOR") != -1) return ledComedor;
  if (amb.indexOf("COCINA") != -1) return ledCocina;
  if (amb.indexOf("BANO") != -1 || amb.indexOf("BAÑO") != -1) return ledBano;
  if (amb.indexOf("HABITACION") != -1) return ledHabitacion;
  if (amb.indexOf("FAN") != -1) return motorPin;
  return 0;
}

// MOSTRAR DETALLES ESCENA
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
