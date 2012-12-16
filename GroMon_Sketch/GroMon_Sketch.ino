//********* start program ********* 


/*
- modified by DK : july 2012
- IDE v1.0.1 (using Nano-168)

 note: to use v1.0
 - #include <NewSoftSerial.h> -> #include <SoftwareSerial.h>fa
 - NewSoftSerial blueToothSerial(RxD,TxD); -> SoftwareSerial blueToothSerial(RxD,TxD);
 */


#include <SoftwareSerial.h>   //Software Serial Port
#define RxD 2  // these '2' & '3' are fixed by the hardware
#define TxD 3
#define DEBUG_ENABLED 1

SoftwareSerial blueToothSerial(RxD,TxD);
int led = 4;

void setup()
{
    Serial.begin(9600);
    pinMode(RxD, INPUT);
    pinMode(TxD, OUTPUT);
    pinMode(led,OUTPUT);  // for 'XBee carrier' can be either '5' or '6'
    setupBlueToothConnection();
}

void loop()
{
  unsigned char control;
  unsigned char temp;
 
  while(temp!='4')  {
      temp=blueToothSerial.read();     
  }
 
  Serial.println("You are connected-1");
  blueToothSerial.print("You are connected-2");      
//You can write you BT communication logic here
 
 
  // Big event loop.
  while(1){
  
    Serial.println("Getting Data");
  
    char bluetoothData = (char)blueToothSerial.read();
    
    if (blueToothSerial.available()) {
     
      Serial.print("Data: ");
      Serial.println(bluetoothData);
      
      Serial.write(bluetoothData);
    }
  
    if (bluetoothData == 'n') {
      digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    }
    if (bluetoothData == 'f') {
      digitalWrite(led, LOW);   // turn the LED off (LOW is the voltage level)
    }
     
    if (Serial.available()) {
      blueToothSerial.write(Serial.read());
    }
      
  } // End while(1)
 
}

void setupBlueToothConnection()
{
    blueToothSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
    delay(2000);
    sendBlueToothCommand("\r\n+STWMOD=0\r\n");    // 0 = slave
    sendBlueToothCommand("\r\n+STNA=GroMonProbe\r\n"); 
    sendBlueToothCommand("\r\n+STAUTO=0\r\n");    // 0 = Auto-connect forbidden
    sendBlueToothCommand("\r\n+STOAUT=1\r\n");    // 1 = Permit Paired device to connect me
    sendBlueToothCommand("\r\n+STPIN=0000\r\n");
    delay(2000); // This delay is required.
    sendBlueToothCommand("\r\n+INQ=1\r\n");      // 1 = Enable been inquired (slave)
    delay(2000); // This delay is required.
}

//Checks if the response "OK" is received
void CheckOK()
{
  char a,b;
  while(1) {
    if(blueToothSerial.available())
    {
    a = blueToothSerial.read();
// ************************************* debug ********************************************
    Serial.write(a);    // show response from bluetooth module

    if('O' == a)
    {
      // Wait for next character K. available() is required in some cases, as K is not immediately available.
      while(blueToothSerial.available())
      {
         b = blueToothSerial.read();
        
// ************************************* debug ********************************************
    Serial.write(b);        
        
         break;
      }
      if('K' == b)
      {
        break;
      }
    }
   }
  }

  while( (a = blueToothSerial.read()) != -1)
  {
    //Wait until all other response chars are received
  }
}

void sendBlueToothCommand(char command[])
{
    blueToothSerial.print(command);
    CheckOK();   
}


//********* end program *********

