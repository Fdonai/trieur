#include <Arduino.h>
#include "rgb_lcd.h"
#include <ESP32Encoder.h>
#include "Adafruit_TCS34725.h"
#include <SPI.h>

rgb_lcd lcd;
ESP32Encoder encoder;

#define VIT 400
void vTaskPeriodic(void *pvParameters);


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
xTaskCreate(vTaskPeriodic, "vTaskPeriodic", 10000, NULL, 2, NULL);


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



static int i = 0;
Serial.printf("Boucle principale : %d\n", i++);
delay(500); 
lcd.setCursor(0, 0);
Serial.printf("Cons : %d, vitesse : %d , Erreur : %d, Somme : %d, pwm : %d\n", Consigne,vitesse, Erreur, Somme, pwm);lcd.clear();
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
}

void vTaskPeriodic(void *pvParameters)
{
 TickType_t xLastWakeTime;
 // Lecture du nombre de ticks quand la tâche commence
 xLastWakeTime = xTaskGetTickCount();
 while (1)
 {


 NewVal= encoder.getCount ( );
 vitesse= NewVal-OldVal;
 OldVal= NewVal;

  Consigne =
    ((analogRead(POT)-2048)/5);
  
  if(((-10)<=Consigne) && (Consigne <= 10))Consigne=0,Somme=0;
  else
  {
     Consigne =
    ((analogRead(POT)-2048)/5);
  }
  

   Erreur = 
    Consigne - vitesse;

   Somme = 
    Somme + Erreur;

   pwm = 
    Kp * Erreur + Ki * Somme ;

    


if (pwm>0)
  {
  if (pwm>2047) pwm=2047/2;
  digitalWrite(MODE,LOW);
  ledcWrite(canal1,pwm);
  }
else
{
  if (pwm<(-2047))pwm=((-2047)/2);
  digitalWrite(MODE,HIGH);
  ledcWrite(canal1,(-pwm));
}

 // Endort la tâche pendant le temps restant par rapport au réveil,
 // ici 100ms, donc la tâche s'effectue ici toutes les 100ms.
 // xLastWakeTime sera mis à jour avec le nombre de ticks au prochain
 // réveil de la tâche.
 vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));
 }
}

