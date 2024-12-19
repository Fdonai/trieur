#include <Arduino.h>
#include "rgb_lcd.h"
#include <ESP32Encoder.h>
#include "Adafruit_TCS34725.h"
#include <SPI.h>



rgb_lcd lcd;
ESP32Encoder encoder;


#define VIT 400
//void vTaskPeriodic(void *pvParameters);


//initialisation du switch case
char etat=0;

//définition des pins des BP
int BP0=0;
int BP1=2;
int BP2=12;

//définition du pin du POTAR
int POT=33;

//définition du pin du CNY70
int CNY=36;

//déclaration des variables pour lire les entrée
int Val_BP0;
int Val_BP1;
int Val_BP2;

//définition du pin du PWM
int PWM=27;
int MODE=26;
int SLEEP=25;

// caractéristique de la PWM
int frequence = 25000;
int canal1=1;
int resolution = 11;

//déclaration de la variables de la lecture du POTAR
int lecture_POT;
float Tension_POT;

//déclaration de la variables de la lecture du CNY70
int lecture_CNY;

//déclaration d'une variable 
char i;
int x;

//déclaration des variable pour l'asservisement de la vitesse
int NewVal;
int vitesse;
int OldVal = 0;
int Erreur;
int Consigne;
int pwm;

float Kp = 5;//Coeff Proportionnel
float Ki = 2.5;//Coeff Integral

int Somme;

//Déclaration du pin du servo moteur
int servo = 13;
int pos;

// caractéristique du servo moteur
int servfreq = 50;
int canal2=1;
int resolutionser = 10;

void setup() {
  // Initialise la liaison avec le terminal
  Serial.begin(115200);

  //Init Entree Digital
  pinMode(BP0,INPUT_PULLUP);//BP Blue
  pinMode(BP1,INPUT_PULLUP);//BP Jaune
  pinMode(BP2,INPUT_PULLUP);//BP Vert

  // Initialise l'écran LCD
  Wire1.setPins(15, 5);
  lcd.begin(16, 2, LCD_5x8DOTS, Wire1);
 // lcd.printf("Trieur de balles");

// Configuration de la PWM
  pinMode(MODE,OUTPUT);
  pinMode(SLEEP,OUTPUT);

  ledcSetup(canal1,frequence,resolution);

// Liaison du canaux PWM sur l'ESP32
  ledcAttachPin(PWM,canal1);

//Config Encoder
  encoder.attachFullQuad ( 23, 19);
  encoder.setCount ( 0 );
  

// Début OS en Temps Réel
Serial.printf("Initialisations\n");
// Création de la tâche périodique
//xTaskCreate(vTaskPeriodic, "vTaskPeriodic", 10000, NULL, 2, NULL);


//Configuration du servo moteur
ledcSetup(canal2,servfreq,resolutionser);
// Liaison du canaux PWM du servo sur l'ESP32
  ledcAttachPin(servo,canal2);
}

void loop() {

  lcd.setRGB(255, 100, 200);
  digitalWrite(SLEEP, HIGH);

//Boutons poussoirs
  Val_BP0=digitalRead(BP0);
  Val_BP1=digitalRead(BP1);
  Val_BP2=digitalRead(BP2);

 // Serial.printf("bp0 %d bp1 %d bp2 %d \n",Val_BP0,Val_BP1,Val_BP2);

//Potentiomètre
/*
Tension_POT=3.3*lecture_POT/4095;

//Serial.printf("pot %d \n",lecture_POT);
lcd.printf("pot %d ",lecture_POT);
delay(10);
lcd.clear();
lcd.printf("U= %.2f V",Tension_POT);
delay(10);
lcd.clear();
*/

//rapport cyclique


// encoder
/*
long newPosition = encoder.getCount();
lcd.setCursor(0,1);
lcd.print(newPosition);
*/






lcd.clear();
lcd.setCursor(0, 0);
lcd.printf("pos : %d",pos);
delay(50); 
/*
// CNY70
lcd.setCursor(0,0);
lecture_CNY=analogRead(CNY);
lcd.printf("cny %d ",lecture_CNY);


//IHM
lcd.setCursor(6,1);
lcd.printf("etat %d",etat);
lcd.setCursor(10,0);
lcd.printf("i %d",i);
/*
lcd.setCursor(0,0);
lcd.printf("Init Press BPV");
lcd.printf("cycle %.2f%",(lecture_POT/2.0)*100/2047);
lcd.setCursor(0,1);
lcd.printf("Choir mode A/R:");
*/

pos =
  (((30*(analogRead(POT)/4))/1024)+60);
  
  if((70<=pos) && (pos <= 75))pos=75;
  else
  {
  pos =(((30*(analogRead(POT)/4))/1024)+60);
    
  }
  ledcWrite(canal2, pos);


}


