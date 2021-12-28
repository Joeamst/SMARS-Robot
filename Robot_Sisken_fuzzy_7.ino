#include "AFMotor.h"
#include <FuzzyRule.h>
#include <FuzzyComposition.h>
#include <Fuzzy.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzyOutput.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzySet.h>
#include <FuzzyRuleAntecedent.h>

#define minDistance 15
#define delayForwards 750
#define delayRotate 300
#define delayLoop 75
#define maxSpeed 255
#define echoPin A1 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin A0 //attach pin D3 Arduino to pin Trig of HC-SR04


long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

AF_DCMotor motorR(1);  // right DC Motor
AF_DCMotor motorL(2);  // left DC Motor


  
  //Inisialisasi Fuzzy
  Fuzzy* fuzzy = new Fuzzy();

  /*FuzzySet* dekat     = new FuzzySet(0, 2, 150, 225);
  FuzzySet* medium_in = new FuzzySet(150, 225, 300, 375);
  FuzzySet* jauh      = new FuzzySet(300, 375, 450, 450);*/
  
  FuzzySet* dekat     = new FuzzySet(10, 15, 20, 25);
  FuzzySet* medium_in = new FuzzySet(15, 20, 25, 30);
  FuzzySet* jauh      = new FuzzySet(20, 25, 30, 35);
  
  FuzzySet* sangat_lambat = new FuzzySet(0, 0, 25, 55);
  FuzzySet* lambat        = new FuzzySet(25, 55, 85, 115);
  FuzzySet* medium_out    = new FuzzySet(85, 115, 145, 175);
  FuzzySet* cepat         = new FuzzySet(145, 175, 205, 235);
  FuzzySet* sangat_cepat  = new FuzzySet(205, 235, 255, 255);

  /*FuzzySet* sangat_lambat = new FuzzySet(0, 150, 300, 450);
  FuzzySet* lambat        = new FuzzySet(150, 300, 450, 600);
  FuzzySet* medium_out    = new FuzzySet(300, 450, 600, 750);
  FuzzySet* cepat         = new FuzzySet(450, 600, 750, 900);
  FuzzySet* sangat_cepat  = new FuzzySet(600, 750, 900, 1050);*/
  
void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
 
  //motorR.setSpeed(255);
  //motorL.setSpeed(255);
  //motorR.run(RELEASE);
  //motorL.run(RELEASE);
//--------------------------------------------------------------------------------------
  
  
  //input nih
  FuzzyInput* fuzz_jarak = new FuzzyInput(1);
  
  fuzz_jarak->addFuzzySet(dekat);
  fuzz_jarak->addFuzzySet(medium_in);
  fuzz_jarak->addFuzzySet(jauh);
  fuzzy->addFuzzyInput(fuzz_jarak);

  //output nih
  FuzzyOutput* fuzz_kecepatan = new FuzzyOutput(1);

  fuzz_kecepatan->addFuzzySet(sangat_lambat);
  fuzz_kecepatan->addFuzzySet(lambat);
  fuzz_kecepatan->addFuzzySet(medium_out);
  fuzz_kecepatan->addFuzzySet(cepat);
  fuzz_kecepatan->addFuzzySet(sangat_cepat);
  fuzzy->addFuzzyOutput(fuzz_kecepatan);

  //FuzzyRuleBase
  
  //Rule01 , Jika jarak dekat maka motor sangat lambat
  FuzzyRuleAntecedent* if_jarak_dekat = new FuzzyRuleAntecedent();
  if_jarak_dekat->joinSingle(dekat);
  FuzzyRuleConsequent* then_kecepatan_sangat_lambat = new FuzzyRuleConsequent();
  then_kecepatan_sangat_lambat->addOutput(sangat_lambat);
  
  FuzzyRule* fuzzyRule01 = new FuzzyRule(1, if_jarak_dekat, then_kecepatan_sangat_lambat);

  //Rule02, Jika jarak dekat dan medium maka motor lambat
  FuzzyRuleAntecedent* if_jarak_dekat_AND_jarak_medium = new FuzzyRuleAntecedent();
  if_jarak_dekat_AND_jarak_medium->joinWithAND(dekat, medium_in);
  FuzzyRuleConsequent* then_kecepatan_lambat = new FuzzyRuleConsequent();
  then_kecepatan_lambat->addOutput(lambat);

  FuzzyRule* fuzzyRule02 = new FuzzyRule(2, if_jarak_dekat_AND_jarak_medium, then_kecepatan_lambat);

  //Rule03, jika jarak medium maka motor medium
  FuzzyRuleAntecedent* if_jarak_medium = new FuzzyRuleAntecedent();
  if_jarak_medium->joinSingle(medium_in);
  FuzzyRuleConsequent* then_kecepatan_medium = new FuzzyRuleConsequent();
  then_kecepatan_medium->addOutput(medium_out);

  FuzzyRule* fuzzyRule03 = new FuzzyRule(3, if_jarak_medium, then_kecepatan_medium);

  //Rule04, jika jarak medium dan jauh maka motor cepat
  FuzzyRuleAntecedent* if_jarak_medium_AND_jarak_jauh = new FuzzyRuleAntecedent();
  if_jarak_medium_AND_jarak_jauh->joinWithAND(medium_in, jauh);
  FuzzyRuleConsequent* then_kecepatan_cepat = new FuzzyRuleConsequent();
  then_kecepatan_cepat->addOutput(cepat);

  FuzzyRule* fuzzyRule04 = new FuzzyRule(4, if_jarak_medium_AND_jarak_jauh, then_kecepatan_cepat);

  //Rule05, jika jarak jauh maka motor sangat cepat
  FuzzyRuleAntecedent* if_jarak_jauh = new FuzzyRuleAntecedent();
  if_jarak_jauh->joinSingle(jauh);
  FuzzyRuleConsequent* then_kecepatan_sangat_cepat = new FuzzyRuleConsequent();
  then_kecepatan_sangat_cepat->addOutput(sangat_cepat);

  FuzzyRule* fuzzyRule05 = new FuzzyRule(5, if_jarak_jauh, then_kecepatan_sangat_cepat);

  //Add fuzzy Rule
  fuzzy->addFuzzyRule(fuzzyRule01);
  fuzzy->addFuzzyRule(fuzzyRule02);
  fuzzy->addFuzzyRule(fuzzyRule03);
  fuzzy->addFuzzyRule(fuzzyRule04);
  fuzzy->addFuzzyRule(fuzzyRule05);
 //-------------------------------------------------------------------------------------- 

}
void loop() {
  jarak();
  //proses_fuzzy();\

  if (distance > 10 && distance < 35){
    int input_value = distance;
    fuzzy->setInput(1, input_value);
    fuzzy->fuzzify();
    float output_value = fuzzy->defuzzify(1);
    Serial.print("output: ");
    Serial.println(output_value);
    //delay(2000);
    motorR.run(FORWARD);
    motorL.run(FORWARD);
    motorR.setSpeed(output_value);
    motorL.setSpeed(output_value);
    delay(100);
  } else if (distance <= 5){
    motorR.run(FORWARD);
    motorL.run(FORWARD);
    motorR.setSpeed(0);
    motorL.setSpeed(0);
    digitalWrite(9, 0);
    delay(100);
  } else if (distance > 35){
    motorR.run(FORWARD);
    motorL.run(FORWARD);
    motorR.setSpeed(255);
    motorL.setSpeed(255);
    delay(100);
  } else if (distance <= 10 && distance > 5){
    motorR.run(FORWARD);
    motorL.run(BACKWARD);
    motorR.setSpeed(20);
    motorL.setSpeed(20);
    Serial.println("Turn Left");
    delay(100);
  }
 }
 
void moveForwards(int timeDelay){
  uint8_t i;
  motorR.run(FORWARD);
  motorL.run(FORWARD);
  for (i=0; i<maxSpeed; i++) {
    motorR.setSpeed(i);
    motorL.setSpeed(i);
    delay(2);
  }

  delay(timeDelay);

  for (i=maxSpeed; i!=0; i--) {
    motorR.setSpeed(i);
    motorL.setSpeed(i);
    delay(2);
  }
  motorR.run(RELEASE);
  motorL.run(RELEASE);
}

void turnAround(int timeDelay) {
  uint8_t i;
  motorR.run(FORWARD);
  motorL.run(BACKWARD);
  for (i=0; i<maxSpeed; i++) {
    motorR.setSpeed(i);
    motorL.setSpeed(i);
    delay(10);
  }
  delay(timeDelay);
  for (i=maxSpeed; i!=0; i--) {
    motorR.setSpeed(i);
    motorL.setSpeed(i);
    delay(10);
  }
  motorR.run(RELEASE);
  motorL.run(RELEASE);
}

void jarak(){
   digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration/58.2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}
