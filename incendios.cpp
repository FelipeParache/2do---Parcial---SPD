#include <IRremote.h>
#include <LiquidCrystal.h>
#include <Servo.h>


// CONSTANTES
#define sensorInfrarrojo 4
#define botonOnOff 0xFF00BF00
#define SERVO 7
#define sensorTemperarura A0
#define ledRojo 2
#define ledVerde 3

// LCD
#define RS 8
#define E 9
#define D4 10
#define D5 11
#define D6 12
#define D7 13

// VARIABLES
bool sistemaEncendido = false;
bool incendio = false;
String estacion = "";
int temperatura = 0; 

// OBJETOS
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);
Servo miServo;

///////////////////////////////////////////////

void setup() {

  pinMode(ledRojo, OUTPUT);  
  pinMode(ledVerde, OUTPUT);
  
  lcd.begin(16,2); // 16 columnas y 2 filas
  
  IrReceiver.begin(sensorInfrarrojo, DISABLE_LED_FEEDBACK); // Creo el objeto para recibir señales de un receptor de infrarrojos.
  
  miServo.attach(SERVO, 500, 2500);
  
  Serial.begin(9600); 
}

void loop() {
  
  DetectarControlRemoto();
  
  if(sistemaEncendido) {
    
    int reading = analogRead(sensorTemperarura);
    temperatura = DefinirTemperatura(reading);
    DefinirEstacion(temperatura);
    DetectarIncendio();
    MostrarTextoLCD(temperatura);
    
    if(incendio) {
      
      MoverServo(180);
      apagarLed(ledVerde);
      titilarLedIncendio(ledRojo, 3);    
    } else {
      
      prenderLed(ledVerde);
      MoverServo(0); 
  	}
  } else {
    
    apagarLed(ledVerde);
  }
  delay(500);
  lcd.clear();
}

void DetectarIncendio() {
  
  if (temperatura > 60) {
    
    incendio = true;
  } else {
    
    incendio = false;
  }
}

void MoverServo(int grados) {
  
  miServo.write(grados); 
}

void DetectarControlRemoto() {

  if (IrReceiver.decode()) { // Verificar si hay alguna señal de control remoto recibida. 
    
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); //Imprimo datos con valor Hexa
    
    if(IrReceiver.decodedIRData.decodedRawData == botonOnOff) {
      
      sistemaEncendido = !sistemaEncendido;
    }
    
    IrReceiver.resume(); //Preparo para recibir un nuevo codigo
    delay(100);
  }
}

float DefinirTemperatura(int reading) {
 
  Serial.print("Reading: ");  
  Serial.println(reading);

  float cuenta = 5.0 / 1024.0;
    
  float voltage = reading * cuenta;  
  
  float temp = map(reading, 20, 350, -40, 125);

  Serial.print("Temp: ");
  Serial.println(temp);
  
  // Convert the voltage into the temperature in Celsius
  // restamos 500mV porque 0°C = 0.5V

  float temperatureC = (voltage - 0.5) * 100;
  
  return temperatureC;
}


void DefinirEstacion(int temperatura) {
  
  if(temperatura < 0) {
    
    estacion = "INVIERNO";
  } else if (temperatura < 15) {
    
    estacion = "OTONIO";
  } else if (temperatura < 35) {
    
  	estacion = "PRIMAVERA";
  } else {
    
  	estacion = "VERANO";
  }
}

void MostrarTextoLCD(float temp) {
  
  if(incendio) {
    
    lcd.setCursor(0,0);
    lcd.print("ALERTA INCENDIO");
  } else {
    
    lcd.setCursor(0,0);
    lcd.print(temp);
    lcd.setCursor(0,1);
    lcd.print(estacion);
  }
}

void prenderLed(int led) {
  
  digitalWrite(led, HIGH);  
}

void apagarLed(int led) {
  
  digitalWrite(led, LOW);  
}

void titilarLedIncendio(int pinLed, int veces) {
  
  int contador = 0;
  apagarLed(pinLed);    
  delay(200);
  
  while (contador < veces) {
    
    prenderLed(pinLed);
    delay(300);
    apagarLed(pinLed);    
    delay(300);
    contador++;
  }
}
