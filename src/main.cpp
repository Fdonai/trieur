#include <Arduino.h>
#include "rgb_lcd.h"
#include <ESP32Encoder.h>
#include "Adafruit_TCS34725.h"
#include <SPI.h>

rgb_lcd lcd;
ESP32Encoder encoder;

#define VIT 400

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
  
}

void loop() {

  lcd.setRGB(255, 0, 0);

//Boutons poussoirs
  Val_BP0=digitalRead(BP0);
  Val_BP1=digitalRead(BP1);
  Val_BP2=digitalRead(BP2);

 // Serial.printf("bp0 %d bp1 %d bp2 %d \n",Val_BP0,Val_BP1,Val_BP2);

//Potentiomètre
lecture_POT=analogRead(POT);
Tension_POT=3.3*lecture_POT/4095;

/*
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
long newPosition = encoder.getCount();
lcd.setCursor(0,1);
lcd.print(newPosition);



switch (etat)
{
case 0:
  ledcWrite(canal1,0);
  digitalWrite(SLEEP,LOW);
  if (!Val_BP2)etat=1; // bouton Init vert
  break;

case 1:
  digitalWrite(MODE,HIGH);
  digitalWrite(SLEEP,HIGH);
  ledcWrite(canal1,VIT);
  if(lecture_CNY>=4000)etat=2;
break;

case 2:
  i=0;
  encoder.setCount ( 0 );
  ledcWrite(canal1,0);
  digitalWrite(SLEEP,LOW);
  if(!Val_BP0)etat=3;//bouton bleu 
  if(!Val_BP1)etat=4;//bouton jaune
break;
case 3:
  x=1;
  digitalWrite(MODE,HIGH);//sens horaire 
  digitalWrite(SLEEP,HIGH);//Armé
  ledcWrite(canal1,VIT);//vitesse de rotation
  if(newPosition<=(-103))etat=5;
break;
case 4:
  x=2;
  digitalWrite(MODE,LOW);//sens anti-horaire 
  digitalWrite(SLEEP,HIGH);//Armé
  ledcWrite(canal1,VIT);//vitesse de rotation
  if(newPosition>=(103))etat=5;
break;
case 5://boite tempo
  ledcWrite(canal1,0);//Arret
  digitalWrite(SLEEP,LOW);//Moteur SLEEP
  encoder.setCount (0);
  i++;
  delay(2000);
  if(i>=8)etat=2;
  else if(x==1)etat=3;
  
  else if(x==2)etat=4;
  
break;

default:
  break;
}
 


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
}

