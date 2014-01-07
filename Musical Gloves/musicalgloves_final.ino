#include <SoftwareSerial.h>

//Variables

//On/Off Switch
int currentPowerState;
const int powerPin = 13;

//Flex Sensors
byte note1 = 0;
byte note2 = 0;
byte note3 = 0;
int AnalogValue1 = 0;
int AnalogValue2 = 0;
int AnalogValue3 = 0;

//Contact Switch
const int switchPin = 10;
boolean ifPressed = false;
int lastNotePlayed = 0;
int lastSwitchState = 0; 
int currentSwitchState = 0;

//Accelerometer
int AnalogValueY=0;                  // x-axis of the accelerometer
const int ypin = A3;
int currentYState = 0;
int lastYState = 0;

//Software serial
SoftwareSerial midiSerial(2,3); //  what's 2?

void setup(){
 
 digitalWrite(powerPin, HIGH);
  
 pinMode(powerPin,INPUT); 
  
 pinMode(switchPin, INPUT);
 
 Serial.begin(9600);
 
 //Set MIDI baud rate.  Standard is 31250
 midiSerial.begin(31250); 
 
}

void loop(){
  
  //Power Switch
  currentPowerState = digitalRead(powerPin);
 
  if(currentPowerState == HIGH){
    
    // Serial.println("ON");
   //  delay(30);
  
    
    //Flex Sensor A0
    AnalogValue1 = analogRead(A0);
    //Serial.println (AnalogValue1);
    constrain(AnalogValue1, 335, 538);
    note1 = map(AnalogValue1, 335, 538, 0, 126);
    constrain(note1, 0, 126);
  
    
   //Flex Sensor A1
    AnalogValue2 = analogRead(A1);
    constrain(AnalogValue2, 430, 518);
    note2 = map(AnalogValue2, 430, 518, 0, 126);
    constrain(note2, 0, 126);
    
    //FlexSensor A2 
    AnalogValue3 = analogRead(A2);
    //Serial.println (AnalogValue3);
    constrain(AnalogValue3,322, 442);
    note3 = map(AnalogValue3, 322, 442, 0, 126);
    constrain(note3, 0, 126);
  
  
    //Accelerometer
    //AnalogValueX = analogRead();
    AnalogValueY = analogRead(ypin);
    //AnalogValueZ = analogRead();
    
    
    if (AnalogValueY >= 355){
      currentYState = 1;
    }
    else{
      
      if(AnalogValueY <=310){
        currentYState = 2;}
      else
        currentYState = 0;
    }
    
    if((currentYState == 1) && (lastYState == 0)){
      noteOn(146, 75, 64);
    }
    else{ 
      if((currentYState == 2) && (lastYState == 0)){
        noteOn(146, 71, 64);
      }
      else
        noteOn(146,60,0); 
    }
    
    lastYState = currentYState;
   
   
     //Contact Switch
    currentSwitchState = digitalRead(switchPin);
    
    if((currentSwitchState == 1) && (lastSwitchState == 0)){
      noteOn(144, 60, 64);
    }
    else if(currentSwitchState==0) {
       noteOn(144,60,0); 
    }
    lastSwitchState = currentSwitchState;

    if(AnalogValue1 > 0){
      //continuousController(Control Change Value, Channel #, Note)
      noteOn(176, 1, note1);
    }
  
  
    if(AnalogValue2 > 0){
       //continuousController(Control Change Value, Channel #, Note)
      noteOn(176, 2, note2);
    }
   
    
    if(AnalogValue3 > 0){
      //continuousController(Control Change Value, Channel #, Note)
      noteOn(176, 3, note3);
    }
  
  
  /*  // print the sensor values:
    Serial.print("X:");
    Serial.print(analogRead(xpin));
    // print a tab between values:
    // Serial.println(" Y:");
    // Serial.print(analogRead(ypin));
    // print a tab between values:
    Serial.print("\t Z:");
    Serial.print(analogRead(zpin));
    Serial.println();
    // delay before next reading
    delay(30); */
  
  }
  else{
    Serial.println("OFF");
    delay(30);
  }
}


//  plays a MIDI note.  Doesn't check to see that
//  cmd is greater than 127, or that data values are  less than 127:

void noteOn(byte cmd, byte data1, byte  data2) 
{
    midiSerial.write(cmd);
    midiSerial.write(data1);
    midiSerial.write(data2);
    
   // prints the values in the serial monitor so we can see what note we're playing
    Serial.print("cmd: ");
    Serial.print(cmd);
    Serial.print(", data1: ");
    Serial.print(data1);
    Serial.print(", data2: ");
    Serial.println(data2);
}


