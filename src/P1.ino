#include<SoftwareSerial.h>
#include<ArduinoJson.h>


#define TIME_MSG_LEN 11
#define TIME_HEADER 255
//serial communication pins
#define RX 5  //receive data from NodeMCU(D5)
#define TX 6  //transmit data to NodeMCU(D6)
//Lamp pins
#define lamp1 3   //1st lamp pin
#define lamp2 9  //2nd lamp pin
#define lamp3 10  //3rd lamp pin
#define lamp4 11   //4th lamp pin
//IR pins
#define IR1 7   //IR pin for lamp 1
#define IR2 8   //IR pin for lamp 2
#define IR3 12  //IR pin for lamp 3
#define IR4 13  //IR pin for lamp 4

//Minimul LDR value for light in Normal mode
const int nModeMinLDRVal = 600;
const byte minBrightness = 20;
const byte maxBrightness = 240;
byte lamp1Brightness = minBrightness;
byte lamp2Brightness = minBrightness;
byte lamp3Brightness = minBrightness;
byte lamp4Brightness = minBrightness;
unsigned long time;

int lampLDRReadings[] = {0,0,0,0};    //store LDR readings for each lamp
int IRReadings[] = {1,1,1,1};         //store each IR readings. Initially all IR values are HIGH


char OpMode = '0';   //Operation in Normal mode
int a = 90;   //debugging

SoftwareSerial S(RX,TX);

void setup() {
  // put your setup code here, to run once:
  S.begin(9600);  
  Serial.begin(9600);
    
  pinMode(lamp1, OUTPUT);
  pinMode(lamp2, OUTPUT);
  pinMode(lamp3, OUTPUT);
  pinMode(lamp4, OUTPUT);

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);

  time = millis()/1000; 
  //everyday at midnight arduino is restarted and time is noted
}

void loop() {
  // put your main code here, to run repeatedly:

  /*json object setup for data transmission*/
  //delay(500);
  if(S.available()>0)//if(a==90)
  {
    Serial.print("OpMode before reading: ");
    Serial.println(OpMode);
    OpMode = S.read();
    Serial.print("Checking OpMode: ");
    Serial.println(OpMode);

    /*Override mode - full shutdown*/
    if(OpMode=='9'){
      Serial.print("OpMode: ");
      Serial.println(OpMode);
      digitalWrite(lamp1, LOW);
      digitalWrite(lamp2, LOW);
      digitalWrite(lamp3, LOW);
      digitalWrite(lamp4, LOW);
    }
    /*Override mode - full on*/
    else if(OpMode=='1'){
      Serial.print("OpMode: ");
      Serial.println(OpMode);
      digitalWrite(lamp1, HIGH);
      digitalWrite(lamp2, HIGH);
      digitalWrite(lamp3, HIGH);
      digitalWrite(lamp4, HIGH);


      /*Checking lamp functions status*/

      int lamp1LDRReading = analogRead(A1);
      int lamp2LDRReading = analogRead(A2);
      int lamp3LDRReading = analogRead(A3);
      int lamp4LDRReading = analogRead(A4);

      Serial.print("lamp 1 LDR reading: ");
      Serial.println(lamp1LDRReading);
      Serial.print("lamp 2 LDR reading: ");
      Serial.println(lamp2LDRReading);
      Serial.print("lamp 3 LDR reading: ");
      Serial.println(lamp3LDRReading);
      Serial.print("lamp 4 LDR reading: ");
      Serial.println(lamp4LDRReading);

      lampLDRReadings[0] = lamp1LDRReading;
      lampLDRReadings[1] = lamp2LDRReading;
      lampLDRReadings[2] = lamp3LDRReading;
      lampLDRReadings[3] = lamp4LDRReading;

      StaticJsonBuffer<1000> jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();
      JsonArray& segs = root.createNestedArray("segs");
      int error = 0;
      for(int k=0; k<4; k++){
        // if IR reading is LOW and lamp ldr is <500 then lamp is not functioning
        if(lampLDRReadings[k]<500){
          error = 1;
          segs.add(1);
        }
        else
          segs.add(0);
      }
      if(error==1){
        root["err"] = 1;
      }
      root.printTo(S);
    }
    /*Override mode - eastern city*/
    else if(OpMode=='2'){
      Serial.print("OpMode: ");
      Serial.println(OpMode);
      unsigned long estTime = millis();
      unsigned long elapsedTime = estTime - time;
      Serial.println(elapsedTime);
      if(elapsedTime <= 18000000 || elapsedTime >=61200000 ){
        //its before 5 in morning or past 5 in evening
        analogWrite(lamp1, minBrightness);
        analogWrite(lamp2, minBrightness);
        analogWrite(lamp3, minBrightness);
        analogWrite(lamp4, minBrightness);
        int ir1Val = digitalRead(IR1);
        if(ir1Val == LOW)
          analogWrite(lamp1, maxBrightness);
        else
          analogWrite(lamp1, minBrightness);  

        int ir2Val = digitalRead(IR2);
        if(ir2Val == LOW)
          analogWrite(lamp2, maxBrightness);
        else
          analogWrite(lamp2, minBrightness);

        int ir3Val = digitalRead(IR3);
        if(ir3Val == LOW)
          analogWrite(lamp3, maxBrightness);
        else
          analogWrite(lamp3, minBrightness);

        int ir4Val = digitalRead(IR4);
        if(ir4Val == LOW)
          analogWrite(lamp4, maxBrightness);
        else
          analogWrite(lamp4, minBrightness);
      }
      if(elapsedTime > 18000000 && elapsedTime < 61200000 ){
        //its past 5 in morning and before 5 in evening
        digitalWrite(lamp1, LOW);
        digitalWrite(lamp2, LOW);
        digitalWrite(lamp3, LOW);
        digitalWrite(lamp4, LOW);
      }    
    }
    /*Override mode - western city*/
    else if(OpMode=='3'){
      Serial.print("OpMode: ");
      Serial.println(OpMode);

      unsigned long westTime = millis()/1000;
      unsigned long elapsedTime = westTime - time;
      if(elapsedTime <= 21600 || elapsedTime >=64800 ){
        //its before 5 in morning or past 5 in evening

        int ir1Val = digitalRead(IR1);
        if(ir1Val == LOW)
          analogWrite(lamp1, maxBrightness);
        else
          analogWrite(lamp1, minBrightness);  
        
        int ir2Val = digitalRead(IR2);
        if(ir2Val == LOW)
          analogWrite(lamp2, maxBrightness);
        else
          analogWrite(lamp2, minBrightness);

        int ir3Val = digitalRead(IR3);
        if(ir3Val == LOW)
          analogWrite(lamp3, maxBrightness);
        else
          analogWrite(lamp3, minBrightness);

        int ir4Val = digitalRead(IR4);
        if(ir4Val == LOW)
          analogWrite(lamp4, maxBrightness);
        else
          analogWrite(lamp4, minBrightness);
      }
      if(elapsedTime > 21600 && elapsedTime < 64800 ){
        //its past 5 in morning and before 5 in evening
        digitalWrite(lamp1, LOW);
        digitalWrite(lamp2, LOW);
        digitalWrite(lamp3, LOW);
        digitalWrite(lamp4, LOW);
      }
    }
    /*Normal mode*/
    else{
      NormalMode();
      //Serial.println("in normal mode");
    }
  }
  /*else{
    NormalMode();
  }*/
}


void NormalMode(){
  /*reinitializing IR readings and LDR readings array*/
  for(int i=0;i<4;i++){
    IRReadings[i] = 1;
    lampLDRReadings[i] = 0;
  }

  int mainLDRReading = 0;
  mainLDRReading = analogRead(A0);
  
  //debug message
  Serial.print("MSG: Main LDR reading: ");
  Serial.println(mainLDRReading);
  //debug message
  delay(100);
  
  
  /*if LDR reading less than minimum value. Manage LEDs */
  if(mainLDRReading < nModeMinLDRVal){
    /*First lamp*/
    int ir1Val = digitalRead(IR1);
    if(ir1Val == LOW){
      //object approaching lamp 1
      analogWrite(lamp1, maxBrightness);

      /*Check reading from lamp LDR for working status of LED*/
      //delay(100);
      int lamp1LDRReading = 0;
      lamp1LDRReading = analogRead(A1);
      //start debug message
      Serial.print("MSG: Lamp1 LDR reading: ");
      Serial.println(lamp1LDRReading);
      //end debug message
      /*Store readings of IR and lampLDR*/
      IRReadings[0] = 0;
      lampLDRReadings[0] = lamp1LDRReading;  
    }
    else{
      analogWrite(lamp1, minBrightness);
    }

    /*Second lamp*/
    int ir2Val = digitalRead(IR2);
    if(ir2Val == LOW){
      analogWrite(lamp2, maxBrightness);
      //delay(100);
      /*Check reading from lamp LDR for working status of LED*/
      int lamp2LDRReading = 0;
      lamp2LDRReading = analogRead(A2);
      //start debug message
      Serial.print("MSG: Lamp2 LDR reading: ");
      Serial.println(lamp2LDRReading);
      //end debug message  
      /*Store readings of IR and lampLDR*/
      IRReadings[1] = 0;
      lampLDRReadings[1] = lamp2LDRReading;
    }
    else{
      analogWrite(lamp2, minBrightness);
    }

    /*Third lamp*/
    int ir3Val = digitalRead(IR3);
    if(ir3Val == LOW){
      analogWrite(lamp3, maxBrightness);
      //delay(100);
      /*Check reading from lamp LDR for working status of LED*/
      int lamp3LDRReading = 0;
      lamp3LDRReading = analogRead(A3);
      //start debug message
      Serial.print("MSG: Lamp3 LDR reading: ");
      Serial.println(lamp3LDRReading);
      //end debug message  
      /*Store readings of IR and lampLDR*/
      IRReadings[2] = 0;
      lampLDRReadings[2] = lamp3LDRReading;
    }
    else{
      analogWrite(lamp3, minBrightness);
    }

    /*Fourth lamp*/
    int ir4Val = digitalRead(IR4);
    if(ir4Val == LOW){
      analogWrite(lamp4, maxBrightness);
      //delay(100);
      /*Check reading from lamp LDR for working status of LED*/
      int lamp4LDRReading = 0;
      lamp4LDRReading = analogRead(A4);
      //start debug message
      Serial.print("MSG: Lamp4 LDR reading: ");
      Serial.println(lamp4LDRReading);
      //end debug message
      /*Store readings of IR and lampLDR*/
      IRReadings[3] = 0;
      lampLDRReadings[3] = lamp4LDRReading;
    }
    else{
      analogWrite(lamp4, minBrightness);
    }
  }
  else{
    digitalWrite(lamp1, LOW);
    digitalWrite(lamp2, LOW);
    digitalWrite(lamp3, LOW);
    digitalWrite(lamp4, LOW);
  }

  /*send status data to NodeMCU via JSON*/
  
    StaticJsonBuffer<1000> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonArray& segs = root.createNestedArray("segs");
    int error = 0;
    for(int j=0; j<4; j++){
      // if IR reading is LOW and lamp ldr is <500 then lamp is not functioning
      if(IRReadings[j]==0 && lampLDRReadings[j]<500){
        error = 1;
        segs.add(1);
      }
      else
        segs.add(0);
    }
    if(error==1){
      root["err"] = 1;
    }
    root.printTo(S);

}
