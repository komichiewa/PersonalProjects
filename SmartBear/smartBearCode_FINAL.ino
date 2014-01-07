//LED
  #include <Adafruit_NeoPixel.h>
  
  #define PIN 6
  
  // Parameter 1 = number of pixels in strip
  // Parameter 2 = pin number (most are valid)
  // Parameter 3 = pixel type flags, add together as needed:
  //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
  //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
  //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
  //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
  Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);
  
  int brightness = 0;
  int fadeAmount = 1;
  int count = 0;


//GSM
  #include <GSM.h>
  
  #define PINNUMBER ""
  
  // initialize the library instance
  GSM gsmAccess;
  GSM_SMS sms;
  
  // Array to hold the number a SMS is retreived from
  char senderNumber[20]; 

  String guardianNumber="+16269277786";
  String message; 
  
//STATES
  int currentState; 

//FSR
  int FSR_Pin = A0;

//MAGNET SWITCH
  int magnetPin = 8;
  int magnetSensor;

//TIMER
  unsigned long startTime;
  unsigned long currentTime;
  unsigned long timeDifference;

void setup(){ 
   
  Serial.begin(9600);
  loadingLight(); 
  
  currentState=0;
  
  //LED
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  //MAGNET SWITCH
  pinMode(magnetPin, INPUT);
  digitalWrite(magnetPin, LOW);
  
  //GSM
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  Serial.println("SMS Messages Sender + Receiver");
   
  while(count<3){
    loadingLight();
  }
  
  // connection state
  boolean notConnected = true;

  // Start GSM shield
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while(notConnected)
  {
    if(gsmAccess.begin(PINNUMBER)==GSM_READY){
      notConnected = false;
    }
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }
  Serial.println("GSM initialized");
  Serial.println("Ready to send and receive messages.");
  Serial.println(" ");
  delay(5);
  activatedLight();
  currentState = 0;
}

void loop(){
   
  //---COMBO LOCK---
  
  //Magnet Switch  
  magnetSensor = digitalRead(magnetPin);
  
  //FSR
  int FSRReading = analogRead(FSR_Pin); 
  delay(10); //just here to slow down the output for easier reading
  
  //Timer
  timeDifference = currentTime-startTime;
  
  Serial.println(currentState);
   
  if ((currentState==0) && (magnetSensor == HIGH)){
    
    currentState=1;
    comboLight();
    startTime=millis();
    
  }
  
  if ((currentState == 1) && (FSRReading >= 800)){
    
    currentTime=millis();
    
    if((timeDifference)<=5000){
      
      currentState=2;
      comboLight();
      startTime=millis();
      timeDifference=0;
      
    }
  } 
  
  if ((currentState == 2) && (FSRReading >= 800)){
    
    currentTime=millis();
    
    if(((timeDifference)<=5000) && ((timeDifference)>=1000)){
      currentState=3;
      comboLight();
      startTime = millis();
    }
    
  }    
  
  if(currentState == 3){
    
    sendMessage();
    Serial.println("HELP SENT");
    activatedLight();
    currentState=4;
    
   }
   
   if(currentState ==5){
     receivedLight();   
   }
   
   if((currentState == 1) || (currentState == 2)){
     
     currentTime = millis();
     
     if(currentTime-startTime > 5000)
       currentState = 0;
   }
   
  //---RECEIVING MESSAGES---
  
  char c;
  
  // If there are any SMSs available()  
  if (sms.available())
  {
    Serial.println(" ");
    Serial.println("Message received from:");
    
    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.println(senderNumber);
    
    String phoneNumber = String(senderNumber);
    
    if(phoneNumber.equals(guardianNumber)){

      // An example of message disposal    
      // Any messages starting with # should be discarded
      if(sms.peek()=='#')
      {
        Serial.println("Discarded SMS");
        sms.flush();
      }
      
      // Read message bytes and print them
      while(c=sms.read()){
        Serial.print(c);
        message+=c;
      
      }
      
      //Serial.println("\nEND OF MESSAGE");
      
      if(message.equals("Received")){     
        Serial.println(" ");  
        Serial.println("RESPONSE RECEIVED FROM GUARDIAN.");
        currentState=5;
      }
      
      if(message.equals("Found")){     
        Serial.println(" ");  
        Serial.println("CHILD FOUND.");
        activatedLight();
        currentState=0;
      }
      
      // Delete message from modem memory
      message="";
      sms.flush();
      //Serial.println("MESSAGE DELETED");
    }
  
    delay(500);
  }
  
}

void sendMessage(){
  
  char remoteNum[20]="+16269277786";  // telephone number to send sms
    
  // sms text
  //Serial.print("Now, enter SMS content: ");
  char txtMsg[200] = "HELP";
  Serial.println(txtMsg);
  
  // send the message
  sms.beginSMS(remoteNum);
  sms.print(txtMsg);
  sms.endSMS(); 
  Serial.println("\nCOMPLETE!\n"); 
  
}

int readSerial(char result[])
{
  int i = 0;
  while(1)
  {
    while (Serial.available() > 0)
    {
      char inChar = Serial.read();
      if (inChar == '\n')
      {
        result[i] = '\0';
        Serial.flush();
        return 0;
      }
      if(inChar!='\r')
      {
        result[i] = inChar;
        i++;
      }
    }
  }
}

//GSM SHIELD LOADING
void loadingLight(){
  
  colorWipe(strip.Color(brightness, brightness, 0), 25); // Red
  brightness = brightness + fadeAmount;

  if(brightness == 0 || brightness == 80){
    fadeAmount = -fadeAmount;
  } 
  count++;
}

//COMBO SWITCH CORRECT AND EMERGENCY SIGNAL SENT OR CHILD
void activatedLight(){
  colorWipe(strip.Color(0, 30, 0), 0); // Green
  delay(800);
  colorWipe(strip.Color(0,0,0),0);
 }

//COMBO SWITCH CORRECT
void comboLight(){
  colorWipe(strip.Color(30, 30, 0), 0); // Green
  delay(800);
  colorWipe(strip.Color(0,0,0),0);
 }

//WHEN PARENT ACKNOWLEDGES RECEPTION OF EMERGENNCY SIGNAL
void receivedLight(){
  
   // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(0,brightness, brightness), 5); 
  brightness = brightness + fadeAmount;

  if(brightness == 0 || brightness == 30){
    fadeAmount = -fadeAmount;
  }
  
 // delay(0);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait){
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
