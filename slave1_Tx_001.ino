//-----------Slave1_Tx_01----------------//
#include <RF24.h>
#include <SPI.h>
//#include<nRF24L01.h>
const byte wAddress[] = "1Node";
byte tank1[1];
byte ackMessg;
byte newData = false;
RF24 radio(9,10);
int SW1 = 2;
int SW2 = 3;
int SW3 = 4;
int SW4 = 5;
int LED = 7;

void setup(){
	Serial.begin(115200);
	radio.begin();
	radio.setDataRate(RF24_1MBPS);
	//radio.setPALevel(RF24_PA_LOW);
	radio.setChannel(80);
	radio.enableAckPayload();
 radio.enableDynamicPayloads();
	pinMode(SW1, INPUT_PULLUP);
	pinMode(SW2, INPUT_PULLUP);
	pinMode(SW3, INPUT_PULLUP);
	pinMode(SW4, INPUT_PULLUP);
	pinMode(LED, OUTPUT);
}

void loop(){
	send();
	tankLevel();
}

void send(){

	static byte index = 0xFF;
	static unsigned long lastSend;
	unsigned long topLoop = millis();
	if(topLoop - lastSend >= 13000){
		lastSend = topLoop;
		//if(++index > 4){
		index = 0;
	//}
		const byte* ptr = &wAddress[index * 1];

		radio.openWritingPipe(ptr);
		if(radio.write(&tank1, sizeof(tank1))){

			if(radio.isAckPayloadAvailable()){
				newData = true;
				//digitalWrite(LED, HIGH);
				//delay(500);
				//digitalWrite(LED, LOW);
				radio.read(&ackMessg, sizeof(ackMessg));
				Serial.print(F("Acknowledge for "));
				for(byte i = 0; i < 5; i++){
					Serial.write(*ptr++);
				}
				Serial.print(F(" received "));
				Serial.println(ackMessg);
				Serial.println(tank1[0]);
			}
		} else {
			Serial.print(F("Send to "));
			for(byte j = 0; j < 5; j++){
				Serial.write(*ptr++);
			}
			Serial.println(F(" failed "));
		}
	}
}

void tankLevel(){
	if(newData == true){
		newData = false;
				if(digitalRead(SW1) == HIGH && digitalRead(SW2) == HIGH && digitalRead(SW3) == HIGH && digitalRead(SW4) == HIGH){
        tank1[0] = 'A'; //65 Hex
			 } else if(digitalRead(SW1) == LOW && digitalRead(SW2) == HIGH && digitalRead(SW3) == HIGH && digitalRead(SW4) == HIGH){
			  tank1[0] = 'B'; //66 Hex
		   } else if(digitalRead(SW1) == LOW && digitalRead(SW2) == LOW && digitalRead(SW3) == HIGH && digitalRead(SW4) == HIGH){
			  tank1[0] = 'C'; //67 Hex
		  } else if(digitalRead(SW1) == LOW && digitalRead(SW2) == LOW && digitalRead(SW3) == LOW && digitalRead(SW4) == HIGH){
			  tank1[0] = 'D'; //68 Hex
		  } else if(digitalRead(SW1) == LOW && digitalRead(SW2) == LOW && digitalRead(SW3) == LOW && digitalRead(SW4) == LOW){
		  	tank1[0] = 'D'; //69 Hex
		  }
	}//Serial.println(tank1[0]);
  //delay(1000);
}
