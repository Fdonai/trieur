#include <Arduino.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

//définition des pins des BP
int BP0=0;
int BP1=2;
int BP2=12;

//définition du pin du POTAR
int POT=33;

//déclaration des variables pour lire les entrée
int Val_BP0;
int Val_BP1;
int Val_BP2;

//déclaration de la variables de la lecture du POTAR
int lecture_POT;
float Tension_POT;

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
  lcd.printf("Trieur de balles");

}

void loop() {
  /*test 
  printf("test");*/ 
  lcd.setRGB(255, 0, 0);

//Boutons poussoirs
  Val_BP0=digitalRead(BP0);
  Val_BP1=digitalRead(BP1);
  Val_BP2=digitalRead(BP2);

  Serial.printf("bp0 %d bp1 %d bp2 %d \n",Val_BP0,Val_BP1,Val_BP2);

//Potentiomètre
lecture_POT=analogRead(POT);
Tension_POT=3.3*lecture_POT/4095;

Serial.printf("pot %d \n",lecture_POT);
lcd.printf("pot %d ",lecture_POT);
delay(1000);
lcd.clear();
lcd.printf("U= %.2f V",Tension_POT);
delay(1000);
lcd.clear();

}
