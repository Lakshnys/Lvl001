#include <SPI.h>
#include <RF24.h>
//#include <nRF24L01.h>
const byte wAddress[] = "6Node";
const byte rAddress[] = "1Node" "2Node" "3Node" "4Node" "5Node";

byte ackData = 1;
byte theMessage[32];
byte len = 0;
byte newData;

byte mPipe1;
byte mPipe2;
byte mPipe3;

byte motor1[1]={2};

byte ackMessg;

byte LED1 = 2;
byte LED2 = 3;
byte LED3 = 4;
byte LED4 = 5;
byte LED5 = 6;
byte LED6 = 7;
byte LED7 = A1;
RF24 radio(9, 10);

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 4000; // send once per second

void loadAckData(byte pipe) {
  radio.writeAckPayload(pipe, &ackData, sizeof(ackData));
  ackData++;
}
void setup(void) {
  Serial.begin(9600);
  radio.begin();
  radio.setChannel(80);
  radio.setDataRate( RF24_2MBPS );
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  
  for (byte i = 0; i < 5; i++) { //5
    radio.openReadingPipe(i + 1, &rAddress[i * 5]); //5
  }
  loadAckData(1);
  loadAckData(2);
  loadAckData(3);
  loadAckData(4);
  loadAckData(5);

pinMode(LED1, OUTPUT);
pinMode(LED2, OUTPUT);
pinMode(LED3, OUTPUT);
pinMode(LED4, OUTPUT);
pinMode(LED5, OUTPUT);
pinMode(LED6, OUTPUT);
pinMode(LED7, OUTPUT);
  prevMillis = millis();
    send();
}
void loop() {
   currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        send();
        prevMillis = millis();
    }
    getData();
    //levelData();
}

void send() {

  static unsigned long lastSend;
  unsigned long topLoop = millis();
  if (topLoop - lastSend >= 5000) {
    lastSend = topLoop;
    
   if(mPipe1 == 65 && mPipe2 != 65){
      motor1[0] = 1; // Motor "ON"
      digitalWrite(LED7, HIGH);
      delay(2000);
      digitalWrite(LED7, LOW);
      }else if(mPipe1 == 67 || mPipe2 == 65){
    //((mPipe1 == 67 || mPipe1 == 66 ||mPipe1 == 65) && (mPipe2 == 65)) && ((mPipe1 == 67) && (mPipe2 == 65 || mPipe2 == 66 ||mPipe2  == 6))) {
        motor1[0] = 2;  // Motor "OFF"
        digitalWrite(LED7, LOW);
    }
      radio.stopListening();
      radio.openWritingPipe(wAddress[0]);
            bool rslt;
            rslt = radio.write( &motor1, sizeof(motor1) );
        radio.startListening();
        Serial.print("Data Sent ");
        Serial.print(motor1[0]);
        if (rslt) {
            Serial.println("  Acknowledge received");
            //updateMessage();
        }
        else {
            Serial.println("  Tx failed");
        }
        delay(1000);
}
}
void getData(){
  
  byte pipeNum = 0;
  if (radio.available(&pipeNum)) {
    if (pipeNum == 1 || pipeNum == 2 || pipeNum == 3 ||pipeNum == 4 || pipeNum == 5) { // pipeNum == 1 || || pipeNum == 5 
      loadAckData(pipeNum);
    }
    byte len = radio.getDynamicPayloadSize();
    radio.read(theMessage, len);
    newData = 1;
    Serial.print(F("received ["));
    Serial.print(len);
    Serial.print(F("] "));
    for (int x = 0; x < len; x++) {
      if (theMessage[x] < 16) {
        Serial.write('0');
      }
      Serial.print(theMessage[x]);//HEX
    }
    Serial.print(F(" via "));
    Serial.println(pipeNum);
  
    if (newData == 1){
      newData = 0;
      if(pipeNum==1 && len == sizeof(mPipe1)){
      memcpy(&mPipe1, theMessage, sizeof(mPipe1));
        Serial.print("tank1 :  ");
        Serial.print(mPipe1);
        Serial.println();
        
        if(mPipe1 == 65){digitalWrite(LED1, HIGH), digitalWrite(LED2, LOW), digitalWrite(LED3, LOW);}
        else if(mPipe1 == 66){digitalWrite(LED1, HIGH), digitalWrite(LED2, HIGH), digitalWrite(LED3, LOW);}
        else if(mPipe1 == 67){digitalWrite(LED1, HIGH), digitalWrite(LED2, HIGH), digitalWrite(LED3, HIGH);} 
         else {{digitalWrite(LED1, LOW), digitalWrite(LED2, LOW), digitalWrite(LED3, LOW);}   }
      }
        if(pipeNum==2 && len == sizeof(mPipe2)){
         memcpy(&mPipe2, theMessage, sizeof(mPipe2));
          Serial.print("tank2 :  ");
          Serial.print(mPipe2);
          Serial.println();

          if(mPipe2 == 65){digitalWrite(LED4, HIGH), digitalWrite(LED5, LOW), digitalWrite(LED6, LOW);}
        else if(mPipe2 == 66){digitalWrite(LED4, HIGH), digitalWrite(LED5, HIGH), digitalWrite(LED6, LOW);}
        else if(mPipe2 == 67){digitalWrite(LED4, HIGH), digitalWrite(LED5, HIGH), digitalWrite(LED6, HIGH);} 
         else {{digitalWrite(LED4, LOW), digitalWrite(LED5, LOW), digitalWrite(LED6, LOW);}   }
    } 



//  if(pipeNum==3 && len == sizeof(mPipe3)){
//    memcpy(&mPipe3, theMessage, sizeof(mPipe3));
//          Serial.print("Motor :  ");
//          Serial.print(mPipe3);
//          Serial.println();
//    }  
    }
  delay(100);
  }}
















  
