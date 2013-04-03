/*
 * Sketch: GroMon.ino
 *   Desc: Provide temperature and humidity data across a Bluetooth connection.
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
String blueInput= "";
boolean blueReady = false;
boolean debug = true;

//
// Set it up!
void setup() {
	Serial.begin(9600);

	pinMode(RxD, INPUT);
	pinMode(TxD, OUTPUT);
	pinMode(dht11_pin, OUTPUT);
	digitalWrite(dht11_pin, HIGH);

	// Set up the bluetooth device.
	setupBlueToothConnection();
}

void setupBlueToothConnection() {
	if (debug) {
		Serial.println("Setting up Bluetooth connection..");
	}

	blueToothSerial.begin(38400); //Set Bluetooth Bee to default baud rate 38400
	delay(2000);

	sendConfigCmd("\r\n+STWMOD=0\r\n");    // 0 = slave
	sendConfigCmd("\r\n+STNA=ProbeMon\r\n"); 
	sendConfigCmd("\r\n+STAUTO=0\r\n");    // 0 = Auto-connect disabled 
	sendConfigCmd("\r\n+STOAUT=1\r\n");    // 1 = Permit Paired device to connect me
	sendConfigCmd("\r\n+STPIN=0000\r\n");
	delay(2000); // This delay is required.

	sendConfigCmd("\r\n+INQ=1\r\n");      // 1 = Enable been inquired (slave)
	delay(2000); // This delay is required.

	if (debug) {
		Serial.println("..done!");
	}
}

// This sends commands to the Bluetooth module.
void sendConfigCmd(char command[]) {
	blueToothSerial.print(command);
	checkOk();

	if (debug) {
		Serial.print("Bluetooth sending: ");
		Serial.println(command);
	}
}

// This sends commands to the Bluetooth module.
void sendBlueToothCommand(char command[]) {
	blueToothSerial.print(command);
	checkOk();

	if (debug) {
		Serial.print("Bluetooth sending: ");
		Serial.println(command);
	}
}

//
// After setup completes, run the main loop.
//
void loop() {
	readBtSerial(); // Read bluetooth data into blueInput.
	commandLoop(); // Take action on the command strings.
}

// This function reads incoming data from the Bluetooth module
// and adds it to a "command string" variable named blueInput.
// We can only "eat" one character each time loop() is called, so 
// we use a boolean variable named blueReady to tell the rest
// of our program whether or not we have a full "command string" to
// take action on.
// Later, in the commandLoop() we read the "command string" and
// take appropriate action.
void readBtSerial() {
	while (blueToothSerial.available()) {
		char in = (char) blueToothSerial.read();

		// If we hit a newline (or exlamation point) then we
		// have a full "command string" and can take action
		// when commandLoop() is called.
		if (in == '!' || in == '\n'){
			blueReady = true;
		} else {
			blueInput += in;
		}
	}
}

// Checks if the response "OK" is received
void checkOk() {
	char a,b;
	while(1) {
		if (blueToothSerial.available()) {
			a = blueToothSerial.read();

			if (debug) {
				Serial.write(a);    // show response from bluetooth module
			}

			// Wait for next character K. The available() function is 
			// required in case K is not immediately available.
			if ('O' == a) {
				while (blueToothSerial.available()) {
					b = blueToothSerial.read();
					if (debug) {
						Serial.write(b);        
					}
					break;
				}
				if('K' == b) {
					break;
				}
			}
		}
	}

	//Wait until all other response chars are received
	while ((a = blueToothSerial.read()) != -1) {}
}

// 
void commandLoop() {
	if (blueReady == true) {
		if (debug) {
			Serial.println("Bluetooth serial command is: " + blueInput);
		}

		handleBtInput(blueInput);
		blueReady = false;
	}
}

void handleBtInput(String& input) {
	if (input == "cmd::getTemp"){
		if (debug) {
			Serial.println("Fetching temp!");
		}
		getTemp();
	}
	else if (input == "cmd::getVersion") {
		if (debug) {
			Serial.println("Returning version 0.1");
		}
		sendBlueToothCommand("0.1");
	}
	else if (input == "cmd::debug") {
		debug = !debug;
		if (debug) {
			Serial.println("Debug enabled");
		} else {
			Serial.println("Debug disabled");
		}
	}
	input = "";
}

// Fetch the temperature and humidity from the DHT11 sensor.
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

	if (dht11_dat[4]!= dht11_check_sum) {
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
	for (i=0; i< 8; i++) {
		while (!digitalRead(dht11_pin));
		delayMicroseconds(30);
		if (digitalRead(dht11_pin) != 0) {
			bitSet(result, 7-i);
		}
		while (digitalRead(dht11_pin));
	}
	return result;
}

