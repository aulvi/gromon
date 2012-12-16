/*
 * Sketch: GroMon.pde
 *   Desc: Simple app to verify bluetooth connectivity.


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
boolean enableDebug = true;
int led = 4;
String btSerialInput = "";
boolean btSerialComplete = false;

void setup()
{
    Serial.begin(9600);
    pinMode(RxD, INPUT);
    pinMode(TxD, OUTPUT);
    pinMode(led,OUTPUT);  // for 'XBee carrier' can be either '5' or '6'
    setupBlueToothConnection();
}

void setupBlueToothConnection()
{
    Logln("Setting up..");
    blueToothSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
    delay(2000);
    sendBlueToothCommand("\r\n+STWMOD=0\r\n");    // 0 = slave
    sendBlueToothCommand("\r\n+STNA=GroMonProbe1\r\n"); 
    sendBlueToothCommand("\r\n+STAUTO=0\r\n");    // 0 = Auto-connect forbidden
    sendBlueToothCommand("\r\n+STOAUT=1\r\n");    // 1 = Permit Paired device to connect me
    sendBlueToothCommand("\r\n+STPIN=0000\r\n");
    delay(2000); // This delay is required.
    sendBlueToothCommand("\r\n+INQ=1\r\n");      // 1 = Enable been inquired (slave)
    delay(2000); // This delay is required.
}

void sendBlueToothCommand(char command[])
{
    blueToothSerial.print(command);
    Log("Sending ");
    Logln(command);
}

//
// Do work!
//
void loop()
{
  readBtSerial(); // Fetch data from BT serial into string "btSerialInput".
  commandLoop(); // Take action on the command strings.
}

void readBtSerial() {
  while(blueToothSerial.available()){
    Logln("Serial available!");
    char in = (char) blueToothSerial.read();
    
    Log("Char is: ");
    Logln(in);
    
    if(in == '!' || in == '\n'){
      btSerialComplete = true;
    } else {
      btSerialInput += in;
      Log("Now adding char :");
      Log(in);
      Log(": to ");
      Logln(btSerialInput);
    }
  }
}

void commandLoop() {
  if (btSerialComplete == true) {
    
    Logln("Bluetooth serial command is: " + btSerialInput);
    
    handleBtInput(btSerialInput);
    btSerialComplete = false;
  }
}

void handleBtInput(String& input){
  if(input == "cmd::test"){
    sendBlueToothCommand("Test reply!");
  }
  else if(input == "cmd::getTemp"){
    getTemp();
  }
  else if(input == "cmd::aisha"){
    sendBlueToothCommand("Hi Aisha!");
  }
  
  input = "";
}

void getTemp() {
  sendBlueToothCommand("temp: 98, humid: 75");
}

void Log(String& input) {
  if (Serial) {
    Serial.println(input);
  }
}

void Log(char* input) {
  if (Serial) {
    Serial.print(input);
  }
}

void Log(char input) {
  if (Serial) {
    Serial.print(input);
  }
}

void Logln(String& input) {
  if (Serial) {
    Serial.println(input):
  }
}

void Logln(char* input) {
  if (Serial) {
    Serial.println(input):
  }
}

void Logln(char input) {
  if (Serial) {
    Serial.println(input):
  }
}
