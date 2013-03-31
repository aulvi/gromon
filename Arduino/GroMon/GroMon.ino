/*
 * Sketch: GroMon.ino
 *   Desc: 
 *
 * Required Arduino libraries:
 *   SoftwareSerial - http://arduino.cc/en/Reference/SoftwareSerial
 *
 * Attributions:
 * Bluetooth Bee code borrowed from Dejwoot
 *    http://dejwoot.blogspot.com/2012/07/howto-test-your-bluetooth-bee-standalone.html
 *
 * DHT11 code borrowed from Camel and DFROBOT
 *	http://www.dfrobot.com/wiki/index.php?title=DHT11_Temperature_and_Humidity_Sensor_(SKU:_DFR0067)
 *
 */
 
#include <stdin.h>
#include <SoftwareSerial.h>   //Software Serial Port
#define RxD 2  // Pins '2' and '3' are set by the hardware.
#define TxD 3
#define dht11_pin 5  // Temperature sensor pin

SoftwareSerial blueToothSerial(RxD,TxD);
String btSerialInput = "";
boolean btSerialComplete = false;
String usbSerialInput = "";
boolean usbSerialComplete = false;

//
// Set it up!
void setup() {
	Serial.begin(9600);

	pinMode(RxD, INPUT);
	pinMode(TxD, OUTPUT);
    pinMode(dht11_pin, OUTPUT);
    digitalWrite(dht11_pin, HIGH);

    setupBlueToothConnection();
    sendBlueToothCommand("{status::ready}\r\n");
}

void setupBlueToothConnection() {
    Logln("Setting up Bluetooth connection..");

    blueToothSerial.begin(38400); //Set Bluetooth Bee to default baud rate 38400
    delay(2000);

    sendBlueToothCommand("\r\n+STWMOD=0\r\n");    // 0 = slave
    sendBlueToothCommand("\r\n+STNA=ProbeMon\r\n"); 
    sendBlueToothCommand("\r\n+STAUTO=0\r\n");    // 0 = Auto-connect disabled 
    sendBlueToothCommand("\r\n+STOAUT=1\r\n");    // 1 = Permit Paired device to connect me
    sendBlueToothCommand("\r\n+STPIN=0000\r\n");
    delay(2000); // This delay is required.

    sendBlueToothCommand("\r\n+INQ=1\r\n");      // 1 = Enable been inquired (slave)
    delay(2000); // This delay is required.

	Logln("..done!");
}

// This sends commands to the Bluetooth module.
void sendBlueToothCommand(char command[]) {
	blueToothSerial.print(command);
    Log("Bluetooth sending: ");
    Logln(command);
}

//
// After setup completes, run the main loop.
//
void loop() {
	readBtSerial();	// Read bluetooth data into btSerialInput.
	readUsbSerial();	// Read USB serial port data into usbSerialInput, this is useful for debugging.
	commandLoop();	// Take action on the command strings.
}

void readBtSerial() {
	while (blueToothSerial.available()){
		char in = (char) blueToothSerial.read();

		Log("Bluetooth: Char is: ");
		Logln(in);

		if (in == '!' || in == '\n'){
			btSerialComplete = true;
			Logln("Bluetooth: Exclam or newline found!");
		} else {
			btSerialInput += in;
		}
	}
}

void readUsbSerial() {
	while(Serial.available()){
		char in = (char) Serial.read();

		Log("USB: Char is: ");
		Logln(in);

		if (in == '!' || in == '\n'){
			usbSerialComplete = true;
			Logln("USB: Exclam or newline found!");
		} else {
			usbSerialInput += in;
		}
	}
}

void commandLoop() {
	if (btSerialComplete == true) {
		Logln("Bluetooth serial command is: " + btSerialInput);

		handleBtInput(btSerialInput);
		btSerialComplete = false;
	}

	if (usbSerialComplete == true) {
		Logln("USB serial command: " + usbSerialInput);
		handleUsbInput(usbSerialInput);
	}
}

void handleBtInput(String& input) {
	if (input == "cmd::getTemp"){
		Logln("Fetching temp!");
		getTemp();
	}
	else if (input == "cmd::getVersion") {
		Logln("Returning version " + VERSION);
		sendBlueToothCommand("0.1");
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
    
	if (digitalRead(dht11_pin)) {
		Serial.println("dht11 start condition 1 not met"); // wait for DHT response signal: LOW
		delay(1000);
		return;
	}
  
	delayMicroseconds(80);
  
	if (!digitalRead(dht11_pin)) {
		Serial.println("dht11 start condition 2 not met");  //wait for second response signal:HIGH
		return;
	}

	delayMicroseconds(80);// now ready for data reception
  
	for (i=0; i<5; i++) {  
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

byte read_dht11_dat() {
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

// Log without a line break (ie, add your own).
void Log(String& input) {
  if (Serial) {
    Serial.print(input);
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

// Log using a "println".
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
