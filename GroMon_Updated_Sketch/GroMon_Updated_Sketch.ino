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

#include <stdin.h>
#include <SoftwareSerial.h>   //Software Serial Port
#define RxD 2  // these '2' & '3' are fixed by the hardware
#define TxD 3
#define DEBUG_ENABLED 1
#define dht11_pin 5

SoftwareSerial blueToothSerial(RxD,TxD);
boolean enableDebug = true;
String btSerialInput = "";
boolean btSerialComplete = false;

void setup()
{
    Serial.begin(9600);
    pinMode(RxD, INPUT);
    pinMode(TxD, OUTPUT);
    pinMode(dht11_pin, OUTPUT);
    digitalWrite(dht11_pin, HIGH);
    setupBlueToothConnection();
    sendBlueToothCommand("{status::ready}\r\n");
}

void setupBlueToothConnection()
{
    Logln("Setting up..");
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
    char in = (char) blueToothSerial.read();
    
    Log("Char is: ");
    Logln(in);
    
    if(in == '!' || in == '\n'){
      btSerialComplete = true;
    } else {
      btSerialInput += in;
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
    Logln("Fetching temp!");
    getTemp();
  }
  else if(input == "cmd::aisha"){
    sendBlueToothCommand("Hi Aisha!");
  }
  
  input = "";
}

void getTemp() {
byte dht11_dat[5];   
  byte dht11_in;
  byte i;// start condition
       
  digitalWrite(dht11_pin, LOW);
  delay(18);
  digitalWrite(dht11_pin, HIGH);
  delayMicroseconds(1);
  pinMode(dht11_pin, INPUT);
  delayMicroseconds(40);     
    
  if (digitalRead(dht11_pin))
  {
    Serial.println("dht11 start condition 1 not met"); // wait for DHT response signal: LOW
    delay(1000);
    return;
  }
  
  delayMicroseconds(80);
  
  if (!digitalRead(dht11_pin))
  {
    Serial.println("dht11 start condition 2 not met");  //wair for second response signal:HIGH
    return;
  }

  delayMicroseconds(80);// now ready for data reception
  
  for (i=0; i<5; i++)
  {  
    dht11_dat[i] = read_dht11_dat();
  }  //recieved 40 bits data. Details are described in datasheet
    
  pinMode(dht11_pin, OUTPUT);
  digitalWrite(dht11_pin, HIGH);
  byte dht11_check_sum = dht11_dat[0]+dht11_dat[2];// check check_sum
  if(dht11_dat[4]!= dht11_check_sum)
  {
    Serial.println("DHT11 checksum error");
  }

  Serial.print("Current humidity = ");
  Serial.print(dht11_dat[0], DEC);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(dht11_dat[2], DEC);
  Serial.println("C");

  int tempC = dht11_dat[2];
  char buffer[40];
  
  sprintf(buffer, "{\"temperature\": %d, \"humidity\": %d}\n", tempC, dht11_dat[0]);
  blueToothSerial.print(buffer);

}

byte read_dht11_dat()
{
  byte i = 0;
  byte result=0;
  for(i=0; i< 8; i++)
  {
    while (!digitalRead(dht11_pin));
    delayMicroseconds(30);
    if (digitalRead(dht11_pin) != 0 )
      bitSet(result, 7-i);
    while (digitalRead(dht11_pin));
  }
  return result;
}

//
// Logging functions
//
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
    Serial.println(input);
  }
}

void Logln(char* input) {
  if (Serial) {
    Serial.println(input);
  }
}

void Logln(char input) {
  if (Serial) {
    Serial.println(input);
  }
}
