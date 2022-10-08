/*C:\Users\sakth\OneDrive\Documents\Arduino\nRF24L01\
005. Level_Project\Level_Project-2\
slave3_TxRx_003_01
*/

 #include<SPI.h>
 #include<RF24.h>
 //#include<nRF24L01.h>

 const byte wAddress[] = "6Node";
 const byte rAddress[] ="1Node" "2Node" "3Node" "4Node" "5Node";

 byte ackData = 1;//rAddress;
 byte theMessage[32]; //rAddress message holder
 byte len = 0; //rAddress receiving message length
 byte newData = 0; // for retreving rAddress data for individual pipe
 
 byte newPipe1Data; // for acknowledge pipe-1 data
 byte newPipe2Data; // for acknowledge pipe-2 data
 byte newMotorAck; // for acknowledge motor data

 byte mPipe1; //Holding 1Node data received
 byte mPipe2; //Holding 2Node data received
 byte mPipe3; //Holding 3Node data received

 byte motor1[1] = {0}; // Motor value derived from mPipe1 & mPipe2 and send to 3Node

 byte ackMessg; // Acknowledge message received from 3Node

 //-----------Tank_1------------------//
 byte LED1 = 2;
 byte LED2 = 3;
 byte LED3 = 4;
 byte LED4 = 5;
//---Tank_1 Ack----//
 byte LED5 = 6;
//-----------Tank_2------------------//
 byte LED6 = 7;
 byte LED7 = A0;
 byte LED8 = A1;
 byte LED9 = A2;
 //---Tank_2 Ack----//
 byte LED10 = A3;
 //----Motor_Status-------//
 byte LED11 = A4;
 //----Motor_Ack-------//
 byte LED12 = A5;
 //----------------------//
 byte LED5state = HIGH;
 byte LED10state = HIGH;
 byte LED12state = HIGH;
 //----------------------//
 RF24 radio(9,10);
//---------delay for send()-------//
unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 5000; //send once every 5 sec
// //---------delay for LED5()-------//
// unsigned long LED5currentMillis;
// unsigned long LED5prevMillis;
// unsigned long LED5txIntervalMillis = 1000; //LED5 Blink for every 1 sec
// unsigned long LED5OnMillis = 500;
// unsigned long LED5OffMillis = 500;
// //---------delay for LED10()-------//.
// unsigned long LED10currentMillis;
// unsigned long LED10prevMillis;
// unsigned long LED10txIntervalMillis = 1000; //LED10 Blink for every 1 sec
// unsigned long LED10OnMillis = 500;
// unsigned long LED10OffMillis = 500;
// //---------delay for LED12()-------//
// unsigned long LED12currentMillis;
// unsigned long LED12prevMillis;
// unsigned long LED12txIntervalMillis = 1000; //LED12 Blink for every 1 sec
// unsigned long LED12OnMillis = 500;
// unsigned long LED12OffMillis = 500;
// //--------LED flash for not receving data from 1Node [LED1,2,3,4]-----//
//
// //--------LED flash for not receving data from 2Node [LED6,7,8,9]-----//

 void loadAckData(byte pipe){
 radio.writeAckPayload(pipe, &ackData, sizeof(ackData));
 ackData++;
 }
 void setup(void){
   Serial.begin(9600);
      Serial.begin(115200);
   radio.begin();
   radio.setChannel(80);
   //radio.setDataRate(RF24_250KBPS);
   radio.setDataRate(RF24_1MBPS);
   radio.enableAckPayload();
   radio.enableDynamicPayloads();

   for ( byte i = 0; i < 5; i++) {
   radio.openReadingPipe(i + 1, &rAddress[i * 5]);
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
   pinMode(LED8, OUTPUT);
   pinMode(LED9, OUTPUT);
   pinMode(LED10, OUTPUT);
   pinMode(LED11, OUTPUT);
   pinMode(LED12, OUTPUT);
  
   prevMillis = millis();
   send();
 }

 void loop(){
   currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        send();
        prevMillis = currentMillis;
    }
    getData();
   //LED5_blink();
   //LED10_blink();
   //LED12_blink();
 }
 
 void send(){

 	  static unsigned long lastSend;
  	  unsigned long topLoop = millis();
  	 if (topLoop - lastSend >= 10000) {
     lastSend = topLoop;

	 if(mPipe1 == 66 && (mPipe2 != 65 || mPipe2 != 0)){
	 motor1[0] = 'A'; // Motor "ON"
	 digitalWrite(LED11, LOW);
  }
	 else if((mPipe1 == 67 || mPipe2 ==65) || (mPipe1 ==69 || mPipe1 == 0) || (mPipe2 == 69 || mPipe2 == 0)){
	 motor1[0] = 'B'; // Motor "OFF"
	 digitalWrite(LED11, HIGH);
  }
	 radio.stopListening();
	 radio.openWritingPipe(wAddress[0]);
	 bool rslt;
	 rslt = radio.write( &motor1, sizeof(motor1));
	 radio.startListening();
	 Serial.print("Data Send ");
	 Serial.println(motor1[0]);
   delay(100); 
  if(rslt){
 	newMotorAck = 1;
 	Serial.println(" Acknowledge received 3Node");
  }
  // code missed during update  - from Master_Rx_3.1
  else
   {
      Serial.println("  Tx failed");
      Serial.println("<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>");
        }
        delay(300);
     }
 }
 void getData(){
	 byte pipeNum = 0;
	 if(radio.available(&pipeNum)){
	 	if(pipeNum == 1 || pipeNum == 2 || pipeNum == 3 || pipeNum == 4 || pipeNum == 5){
	 		loadAckData(pipeNum);
  }
	 byte len = radio.getDynamicPayloadSize();
	 radio.read(&theMessage, len);
	 newData = 1;
	 Serial.print(F("received ["));
	 Serial.print(len);
	 Serial.print(F("]"));
   delay(50);
   int x;
    for(x = 0; x < len; x++){
		 if(theMessage[x] < 16){
		 Serial.write('0');
		}
 		Serial.print(theMessage[x]);
	 }
	  Serial.print(F("Via"));
	  Serial.println(pipeNum);
    Serial.print("[ newData: ");
    Serial.print(newData);
    Serial.println(" ]");
    Serial.println("---------------------------------------------");
    delay(100);
 //-----------------------------------------------------------------//
 if(newData == 1){
   	newData = 0;

    bool pipe1;
    bool pipe2;

    pipe1 = (pipeNum == 1 && len == sizeof(mPipe1));
    pipe2 = (pipeNum == 2 && len == sizeof(mPipe2));

    if(pipe1){
   	  memcpy(&mPipe1, &theMessage , sizeof(theMessage)); // check memcpy ()
     //delay(100);
     Serial.print(" Tank_1 : ");
     Serial.println(mPipe1);
     delay(50);
     newPipe1Data = 1;
     Serial.print("[ newPipe1Data: ");
     Serial.print(newPipe1Data);
     Serial.println(" ]");
     delay(50);
	 if(mPipe1 == 65){digitalWrite(LED1, HIGH), digitalWrite(LED2, LOW), digitalWrite(LED3, LOW), digitalWrite(LED4, LOW);}
	 else if(mPipe1 == 66) {digitalWrite(LED1, HIGH), digitalWrite(LED2, HIGH), digitalWrite(LED3, LOW), digitalWrite(LED4, LOW);}
	 else if(mPipe1 == 67) {digitalWrite(LED1, HIGH), digitalWrite(LED2, HIGH), digitalWrite(LED3, HIGH), digitalWrite(LED4, LOW);}
	 else if(mPipe1 == 68) {digitalWrite(LED1, HIGH), digitalWrite(LED2, HIGH), digitalWrite(LED3, HIGH), digitalWrite(LED4, HIGH);}
	 // else if(mPipe1 == 69 || mPipe1 == 0){
		//  for(int p1 = 0; p1<3; p1++){
		//  	digitalWrite(LED1, HIGH), digitalWrite(LED2, HIGH), digitalWrite(LED3, HIGH), digitalWrite(LED4, HIGH);
		//  	delay(300);
		//  	digitalWrite(LED1, LOW), digitalWrite(LED2, LOW), digitalWrite(LED3, LOW), digitalWrite(LED4, LOW);
		//  }
	 // 	}
	 }else {
    Serial.println("mPipe1 Data Not Received");
    delay(10);
    newPipe1Data = 0;
   }

  if(pipe2){
         	    memcpy(&mPipe2, &theMessage, sizeof(theMessage)); // check memcpy ()
              delay(50);
              Serial.print(" Tank_2 : ");
              Serial.println(mPipe2);
              delay(150);
              newPipe2Data = 1;
              Serial.print("[ newPipe2Data: ");
              Serial.print(newPipe2Data);
              Serial.println(" ]");
              delay(50);
	 if(mPipe2 == 65){digitalWrite(LED1, HIGH), digitalWrite(LED2, LOW), digitalWrite(LED3, LOW), digitalWrite(LED4, LOW);}
	 else if(mPipe2 == 66) {digitalWrite(LED1, HIGH), digitalWrite(LED2, HIGH), digitalWrite(LED3, LOW), digitalWrite(LED4, LOW);}
	 else if(mPipe2 == 67) {digitalWrite(LED1, HIGH), digitalWrite(LED2, HIGH), digitalWrite(LED3, HIGH), digitalWrite(LED4, LOW);}
	 else if(mPipe2 == 68) {digitalWrite(LED1, HIGH), digitalWrite(LED2, HIGH),digitalWrite(LED3, HIGH), digitalWrite(LED4, HIGH);}
	// else if(mPipe2 == 69 || mPipe2 == 0){
		// for(int p1 = 0; p1<3; p1++){
		// 	digitalWrite(LED1, HIGH), digitalWrite(LED2, HIGH), digitalWrite(LED3, HIGH), digitalWrite(LED4, HIGH);
		//   delay(300);
		//   digitalWrite(LED1, LOW), digitalWrite(LED2, LOW), digitalWrite(LED3, LOW), digitalWrite(LED4, LOW);
 	// 	  }
 	  //}
 	}else{
    Serial.println("mPipe2 Data Not Received");
    Serial.println("=======================================================");
    delay(10);
    newPipe2Data = 0;
  }
 }
 }
 }
//void LED12_blink(){
//    if(newMotorAck = 1){
//  	newMotorAck = 0;
//
//	  LED12currentMillis = millis();
// 	  if(LED12currentMillis - LED12prevMillis >= LED12txIntervalMillis){
// 		  LED12prevMillis += LED12txIntervalMillis;
// 		  LED12state = !LED12state;
// 			  if (LED12state == HIGH){
// 				  LED12txIntervalMillis = LED12OnMillis;
// 			  }
//		  else{
//			 LED12txIntervalMillis = LED12OffMillis;
// 			}
//			 digitalWrite(LED12, LED12state);
// 	  }
// }
// 	  else {
// 	  	Serial.print(" Tx Failed ");
// 	  }
//}
//
//void LED5_blink(){
//	if(newPipe1Data = 1){
//		newPipe1Data = 0;
//
// 		LED5currentMillis = millis();
// 			if(LED5currentMillis - LED5prevMillis >= LED5txIntervalMillis){
// 				LED5prevMillis += LED5txIntervalMillis;
// 				LED5state = !LED5state;
//			 	if (LED5state == HIGH){
// 				LED5txIntervalMillis = LED5OnMillis;
// 				} 
//			 else{
// 				LED5txIntervalMillis = LED5OffMillis;
// 				}
// 				digitalWrite(LED5, LED5state);
// 			}
//
//	}
//}
//
//void LED10_blink(){
//
//	if(newPipe2Data = 1){
//		newPipe2Data = 0;
//
//		 LED10currentMillis = millis();
// 			if(LED10currentMillis - LED10prevMillis >= LED10txIntervalMillis){
// 				LED10prevMillis += LED10txIntervalMillis;
// 				LED10state = !LED10state;
// 				if (LED10state == HIGH){
//				LED10txIntervalMillis = LED10OnMillis;
//				} 
//			 else{
// 				LED10txIntervalMillis = LED10OffMillis;
// 				}
//			 digitalWrite(LED10, LED10state);
//			 }
//
//	}
//}
