#include <Servo.h>

#include <Wire.h>

#include <LiquidCrystal_I2C.h>

int ledSala = 13;
int ledCocina = 12;
int ledComedor = 8;
int ledHabitacion = 4;
int ledBano = 2;

String comando;

LiquidCrystal_I2C lcd(0x20, 16, 2);

Servo miServo;
bool estadoPuerta = false;

int botonPuerta = 7;
bool estadoBotonAnterior = HIGH;

int motorPin = 5;

// Estados para comandos de consulta 
String fanEstado = "OFF";        // off / bajo / medio / alto
String escenaEstado = "FIESTA";  

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("HOME");

  lcd.setCursor(0, 1);
  lcd.print("INGRESE COMANDO...");

  pinMode(ledSala, OUTPUT);
  pinMode(ledCocina, OUTPUT);
  pinMode(ledComedor, OUTPUT);
  pinMode(ledHabitacion, OUTPUT);
  pinMode(ledBano, OUTPUT);

  miServo.attach(3, 400, 1600);
  miServo.write(0);

  pinMode(botonPuerta, INPUT_PULLUP);

  pinMode(motorPin, OUTPUT);

  Serial.println("BIENVENIDO A LA CASA INTELIGENTE");
  Serial.println("INGRESE SUS COMANDOS");
  Serial.println("");
}

void loop() {
  if (Serial.available() > 0) {
    comando = Serial.readStringUntil('\n');
    comando.trim();

    // Comandos de consulta y sistema 
    if (comando == "STATUS") {
      imprimirStatus();
    }
    else if (comando == "RESET") {
      resetSistema();
    }
    else if (comando == "VERSION") {
      Serial.println("Casa Dic 2025 ,Grupo 5 conformado por:");
      Serial.println("José Antonio García Roca - 202401166");
      Serial.println("Esmeralda Del Rosario Guillén Veliz - 201901002");
      Serial.println("Nelson Andres Santa Cruz Gil - 202202728");
      Serial.println("Angel Emanuel Rodriguez Corado - 202404856");
      Serial.println("Pablo Daniel Solares Samayoa - 202400044");

    }

    //SALA
    else if (comando == "L1" || comando == "L1ON") {
      encenderLed(ledSala, "SALA");
    }
    else if (comando == "L1OFF") {
      apagarLed(ledSala, "SALA");
    }

    //COMEDOR
    else if (comando == "L2" || comando == "L2ON") {
      encenderLed(ledComedor, "COMEDOR");
    }
    else if (comando == "L2OFF") {
      apagarLed(ledComedor, "COMEDOR");
    }

    //COCINA
    else if (comando == "L3" || comando == "L3ON") {
      encenderLed(ledCocina, "COCINA");
    }
    else if (comando == "L3OFF") {
      apagarLed(ledCocina, "COCINA");
    }

    //BAÑO
    else if (comando == "L4" || comando == "L4ON") {
      encenderLed(ledBano, "BAÑO");
    }
    else if (comando == "L4OFF") {
      apagarLed(ledBano, "BAÑO");
    }

    //HABITACION
    else if (comando == "L5" || comando == "L5ON") {
      encenderLed(ledHabitacion, "HABITACION");
    }
    else if (comando == "L5OFF") {
      apagarLed(ledHabitacion, "HABITACION");
    }

    //FUNCIONES EXTRA
    else if (comando == "ALLON") {
      encenderLeds();
    }
    else if (comando == "ALLOFF") {
      apagarLeds();
    }

    //ABRIR PUERTA
    else if (comando == "DOOROPEN") {
      abrirPuerta();
    }
    else if (comando == "DOORCLOSE") {
      cerrarPuerta();
    }
    else if (comando == "DOOR") {
      alternarPuerta();
    }

    //MANEJAR VENTILADOR
    else if (comando == "FAN0") {
      manejarVentilador(0, "OFF");
    }
    else if (comando == "FAN1") {
      manejarVentilador(85, "BAJO");
    }
    else if (comando == "FAN2") {
      manejarVentilador(170, "MEDIO");
    }
    else if (comando == "FAN3") {
      manejarVentilador(255, "ALTO");
    }

    //SI NO EXISTE LA FUNCION
    else {
      Serial.println("COMANDO INEXISTENTE");
      Serial.println("INGRESE SUS COMANDOS");
      Serial.println("");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("COMANDO INEXISTENTE");
      lcd.setCursor(0, 1);
      lcd.print("INGRESE COMANDO...");
    }
  }

  //PUERTA CON BOTON
  bool estadoBotonActual = digitalRead(botonPuerta);
  if (estadoBotonAnterior == HIGH && estadoBotonActual == LOW) {
    alternarPuerta();
    delay(200);
  }
  estadoBotonAnterior = estadoBotonActual;
}


//METODO PARA ENCENDER LEDS
void encenderLed(int ledPin, String nombre) {
  digitalWrite(ledPin, HIGH);
  Serial.println(nombre + ": ON");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("COMANDO EJECUTADO:");
  lcd.setCursor(0, 1);
  lcd.print(nombre + ": ON");
}

//METODO PARA APAGAR LEDS
void apagarLed(int ledPin, String nombre) {
  digitalWrite(ledPin, LOW);
  Serial.println(nombre + ": OFF");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("COMANDO EJECUTADO:");
  lcd.setCursor(0, 1);
  lcd.print(nombre + ": OFF");
}

//METODO PARA ENCENDER TODAS LAS LEDS
void encenderLeds() {
  digitalWrite(ledSala, HIGH);
  digitalWrite(ledComedor, HIGH);
  digitalWrite(ledCocina, HIGH);
  digitalWrite(ledBano, HIGH);
  digitalWrite(ledHabitacion, HIGH);
  Serial.println("TODAS LAS LUCES: ON");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TODO ENCENDIDO");
}

//METODO PARA APAGAR TODAS LAS LEDS
void apagarLeds() {
  digitalWrite(ledSala, LOW);
  digitalWrite(ledComedor, LOW);
  digitalWrite(ledCocina, LOW);
  digitalWrite(ledBano, LOW);
  digitalWrite(ledHabitacion, LOW);
  Serial.println("TODAS LAS LUCES: OFF");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TODO APAGADO");
}

//METODO PARA ABRIR LA PUERTA (SERVOMOTOR)
void abrirPuerta() {
  if (estadoPuerta){
    Serial.println("LA PUERTA YA SE ENCONTRABA ABIERTA");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LA PUERTA YA SE");
    lcd.setCursor(0, 1);
    lcd.print("ENCONTRABA ABIERTA");
  }
  else {
    miServo.write(90);
    Serial.println("PUERTA ABIERTA");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PUERTA: ABIERTA");
    estadoPuerta = true;
  }
}

//METODO PARA CERRAR LA PUERTA (SERVOMOTOR)
void cerrarPuerta() {
  if (estadoPuerta){
    miServo.write(0);
    Serial.println("PUERTA CERRADA");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PUERTA: CERRADA");
    estadoPuerta = false;
  }
  else {
    Serial.println("LA PUERTA YA SE ENCONTRABA CERRADA");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LA PUERTA YA SE");
    lcd.setCursor(0, 1);
    lcd.print("ENCONTRABA CERRADA");
  }
}

//METODO PARA ALTERNAR PUERTA (SERVOMOTOR)
void alternarPuerta() {
  if (estadoPuerta){
    miServo.write(0);
    Serial.println("PUERTA CERRADA");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PUERTA: CERRADA");
    estadoPuerta = false;
  }
  else {
    miServo.write(90);
    Serial.println("PUERTA ABIERTA");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PUERTA: ABIERTA");
    estadoPuerta = true;
  }
}

//METODO PARA MANEJAR VENTILADOR(MOTOR DC)
void manejarVentilador(int velocidad, String nombreVel){
  analogWrite(motorPin, velocidad);
//Estado del ventilador
  fanEstado = nombreVel; 
  Serial.println("VENTILADOR: " + nombreVel);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fan: ");
  lcd.print(nombreVel);
}


//Mostrar el status
void imprimirStatus() {
  Serial.print("SALA:");    Serial.println(digitalRead(ledSala) ? "ON" : "OFF");
  Serial.print("COMEDOR:"); Serial.println(digitalRead(ledComedor) ? "ON" : "OFF");
  Serial.print("COCINA:");  Serial.println(digitalRead(ledCocina) ? "ON" : "OFF");
  Serial.print("BAÑO:");    Serial.println(digitalRead(ledBano) ? "ON" : "OFF");
  Serial.print("HAB:");     Serial.println(digitalRead(ledHabitacion) ? "ON" : "OFF");
  Serial.print("FAN:");     Serial.println(fanEstado);
  Serial.print("PUERTA:");  Serial.println(estadoPuerta ? "ABIERTA" : "CERRADA");
  Serial.print("ESCENA:");  Serial.println(escenaEstado);
}



// Resetear todo
void resetSistema() {
  digitalWrite(ledSala, LOW);
  digitalWrite(ledComedor, LOW);
  digitalWrite(ledCocina, LOW);
  digitalWrite(ledBano, LOW);
  digitalWrite(ledHabitacion, LOW);
  manejarVentilador(0, "OFF");
  miServo.write(0);
  estadoPuerta = false;
  escenaEstado = "NINGUNA";
  Serial.println("SISTEMA REINICIADO");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema");
  lcd.setCursor(0, 1);
  lcd.print("Reiniciado");
}

