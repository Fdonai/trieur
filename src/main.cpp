#include <Arduino.h>
#include "rgb_lcd.h"
#include "Adafruit_TCS34725.h"
#include <SPI.h>


rgb_lcd lcd;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_16X);

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

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

}

void loop() {
  

/*
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
*/
/*
uint16_t r, g, b, c, colorTemp, lux;

  tcs.getRawData(&r, &g, &b, &c);
  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
  Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");*/
 float red, green, blue;
  
  tcs.setInterrupt(false);  // turn on LED

  //delay(60);  // takes 50ms to read

  tcs.getRGB(&red, &green, &blue);
  
  tcs.setInterrupt(true);  // turn off LED

  Serial.print("R:\t"); Serial.print(int(red)); 
  Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t\n"); Serial.print(int(blue));
  lcd.setRGB(red, green, blue);
}
