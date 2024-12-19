#include <Arduino.h>
#include "rgb_lcd.h"
#include "Adafruit_TCS34725.h"
#include <SPI.h>
#include <ESP32Encoder.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>


rgb_lcd lcd;
ESP32Encoder encoder;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_16X);
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;

//definition des toutes les fonctions 
void Init_ES(void); // encodeurs + BP
void Init_PWMs(void); // pwm hacheur et servo
void Init_couleur(void); // capteur de couleur
void Init_accel(void); // acceleromètre
void Accelerometre(void); // affiche acceleration
void lecture_Entrees(void); //lecture BP, potar, CNY, encodeur
void printBP(void); // print boutons poussoirs
void printEncodeur(void); // affiche val encodeur
void printAvailableData(void);
void vTaskPeriodic(void *pvParameters); // asservissement
void couleurview(void); // affiche code RGB


//Déclaration des variables des bouton poussoirs
int BP0=0;
int BP1=2;
int BP2=12;

int Val_BP0;
int Val_BP1;
int Val_BP2;

//déclaration des variables du POTAR
int POT=33;

int lecture_POT;
float Tension_POT;

//definition des vaiables encodeur
long Position;

//definition des vaiables CNY
int CNY = 32;

int lecture_CNY;

//définition et caractéristiques des variables du Hacheur
int PWM=27;
int MODE=26;
int SLEEP=25;

int frequence = 25000;
int canal1=1;
int resolution = 11;



//definition variable servomoteur
int SM = 13;
int canal2 = 2;
int frequence_servo = 50;
int resolution_servo = 10;

//déclaration des variable pour l'asservisement de la vitesse
int NewVal;
int vitesse;
int OldVal = 0;
int Erreur;
int Consigne;
int pwm;
int Somme;

float Kp = 5;//Coeff Proportionnel
float Ki = 2.5;//Coeff Integral

//declaration des variables pour le capteur couleur
float red, green, blue;



void setup() {
  // Initialise la liaison avec le terminal
  Serial.begin(115200);

  //initialisations de toutes les entrées et sorties du uC
  Init_ES();
  Init_PWMs();
  //Init_couleur();
  //Init_accel();

  // Début OS en Temps Réel
  Serial.printf("Initialisations\n");
  // Création de la tâche périodique
  //xTaskCreate(vTaskPeriodic, "vTaskPeriodic", 10000, NULL, 2, NULL);

  // Initialise l'écran LCD
  Wire1.setPins(15, 5);
  lcd.begin(16, 2, LCD_5x8DOTS, Wire1);
  lcd.printf("Trieur de balles");
}

void loop() {
  lcd.setRGB(255,100,200);
  digitalWrite(SLEEP, HIGH);
  ledcWrite(canal1, 900);
  
  static int i = 0;
  Serial.printf("Boucle principale : %d\n", i++);
  delay(500); 
  lcd.setCursor(0, 0);
  Serial.printf("Cons : %d, vitesse : %d , Erreur : %d, Somme : %d, pwm : %d\n", Consigne,vitesse, Erreur, Somme, pwm);lcd.clear();
  


}

void Accelerometre (void){
 /* first show some 'normal' readings */
  mpu.enableSleep(false);
  mpu.enableCycle(false);

  for (uint16_t i = 0; i < 300; i++) {
    printAvailableData();
    delay(10);
  }

  /* Next, turn on cycle mode. Note how this changes how often the
   *  readings are updated.
   *
   *  First set a slow cycle rate so the effect can be seen clearly.
   */

  mpu.setCycleRate(MPU6050_CYCLE_20_HZ);
  /* ensure that sleep mode is not active. Cycle mode only works
   *  as intended while sleep mode is not active */
  mpu.enableSleep(false);
  /* Finally, enable cycle mode */
  mpu.enableCycle(true);

  for (uint16_t i = 0; i < 300; i++) {
    printAvailableData();
    delay(10);
  }

  /* Finally enable sleep mode. Note that while we can still fetch
   *  data from the measurement registers, the measurements are not
   *  updated. In sleep mode the accelerometer and gyroscope are
   *  deactivated to save power, so measurements are halted.
   */
  mpu.enableCycle(false);
  mpu.enableSleep(true);

  for (uint16_t i = 0; i < 300; i++) {
    printAvailableData();
    delay(10);
  }
}


void printAvailableData(void) {

  /* Populate the sensor events with the readings*/
  mpu.getEvent(&a, &g, &temp);

  /* Print out acceleration data in a plotter-friendly format */
  Serial.print(a.acceleration.x);
  Serial.print(",");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.print(a.acceleration.z);
  Serial.println("");
}

void lecture_Entrees(void){
  //lecture boutons poussoirs
  Val_BP0=digitalRead(BP0);
  Val_BP1=digitalRead(BP1);
  Val_BP2=digitalRead(BP2);

  //lectures analogiques
  lecture_POT=analogRead(POT);
  lecture_CNY = analogRead(CNY);
  Position = encoder.getCount();

}

void printBP(void){ //print des BP
  Serial.printf("BP0 : %d, BP1 : %d, BP2 : %d\n", Val_BP0, Val_BP1, Val_BP2);
}

void printEncodeur(void){ // print encodeur
  Serial.printf("Position : %d\n", Position);
}

void positionSM(int cote){ // position auto avec consigne
  if (cote == 1) ledcWrite(canal2,90); // position gauche (exterieur)
  if (cote == 2) ledcWrite(canal2,75); // position basse
  if (cote == 3) ledcWrite(canal2,60); // position droite (interieur)
}

void vTaskPeriodic(void *pvParameters)
{
 TickType_t xLastWakeTime;
 // Lecture du nombre de ticks quand la tâche commence
 xLastWakeTime = xTaskGetTickCount();
 while (1)
 {
  //calcul de vitesse
 NewVal= encoder.getCount ( );
 vitesse= NewVal-OldVal;
 OldVal= NewVal;

  // recuperation consigne via potar
  Consigne = ((analogRead(POT)-2048)/5);
  if(((-10)<=Consigne) && (Consigne <= 10))Consigne=0,Somme=0;

  // calcul correction
   Erreur = Consigne - vitesse;
   Somme = Somme + Erreur;
   pwm = Kp * Erreur + Ki * Somme ;
    
if (pwm>0)
  {
  if (pwm>2047/2) pwm=2047/2;
  digitalWrite(MODE,LOW);
  ledcWrite(canal1,pwm);
  }
else
{
  if (pwm<(-2047/2))pwm=((-2047)/2);
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

void couleurview(void){

  tcs.setInterrupt(false);  // turn on LED

  //delay(60);  // takes 50ms to read

  tcs.getRGB(&red, &green, &blue);
  
  tcs.setInterrupt(true);  // turn off LED

  Serial.print("\nR:\t"); Serial.print(int(red)); 
  Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t"); Serial.print(int(blue));
  lcd.setRGB(red, green, blue);

}

void Init_ES(void){
  //Init Entree Digital
  pinMode(BP0,INPUT_PULLUP);//BP Blue
  pinMode(BP1,INPUT_PULLUP);//BP Jaune
  pinMode(BP2,INPUT_PULLUP);//BP Vert

  // Configuration de la PWM hacheur
  pinMode(MODE,OUTPUT);
  pinMode(SLEEP,OUTPUT);

  //Config Encoder
  encoder.attachFullQuad ( 23, 19);
  encoder.setCount ( 0 );
}

void Init_PWMs(void){
  // Configuration des PWM et SM et canaux
  ledcSetup(canal1,frequence,resolution);
  ledcSetup(canal2, frequence_servo, resolution_servo);

  ledcAttachPin(PWM,canal1);
  ledcAttachPin(SM, canal2);
}

void Init_couleur(void){
    //initialise le capteur de couleur
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }
}

void Init_accel(void){
  //initialisation de l'acceleromètre
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
}