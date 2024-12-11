#include <Arduino.h>
#include "rgb_lcd.h"
#include <ESP32Encoder.h>

rgb_lcd lcd;
ESP32Encoder encoder;

//définition des pins des BP
int BP0=0;
int BP1=2;
int BP2=12;

//définition du pin du POTAR
int POT=33;

//définition du pin du CNY70
int CNY=32;

//déclaration des variables pour lire les entrée
int Val_BP0;
int Val_BP1;
int Val_BP2;

//définition du pin du PWM
int PWM=27;

// caractéristique de la PWM
int frequence = 25000;
int canal1=1;
int resolution = 10;


//déclaration de la variables de la lecture du POTAR
int lecture_POT;
float Tension_POT;



//déclaration de la variables de la lecture du CNY70

int lecture_CNY;



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
  ledcSetup(canal1,frequence,resolution);

// Liaison du canaux PWM sur l'ESP32
  ledcAttachPin(PWM,canal1);

//
  encoder.attachHalfQuad ( 23, 19);
  encoder.setCount ( 0 );
  Serial.begin ( 115200 );
}

void loop() {
  /*test 
  printf("test");*/ 
  lcd.setRGB(255, 0, 0);

//Boutons poussoirs
  Val_BP0=digitalRead(BP0);
  Val_BP1=digitalRead(BP1);
  Val_BP2=digitalRead(BP2);

 // Serial.printf("bp0 %d bp1 %d bp2 %d \n",Val_BP0,Val_BP1,Val_BP2);

//Potentiomètre
lecture_POT=analogRead(POT);
Tension_POT=3.3*lecture_POT/4095;

Serial.printf("pot %d \n",lecture_POT);
//lcd.printf("pot %d ",lecture_POT);
delay(10);
lcd.clear();
//lcd.printf("U= %.2f V",Tension_POT);
delay(10);
lcd.clear();

//rapport cyclique
//ledcWrite(canal1,lecture_POT/4);

// encoder
long newPosition = encoder.getCount() / 2;
Serial.println(newPosition);

if (newPosition<= (-200))
{
  ledcWrite(canal1,0);
}
else
{
  ledcWrite(canal1,lecture_POT/4);
}

if(Val_BP0==0)
{
  encoder.setCount(0);
}

// CNY70
lecture_CNY=analogRead(CNY);
lcd.printf("cny %d \n",lecture_CNY);

}
