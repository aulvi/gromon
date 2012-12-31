/*
 * dummy-probe.js
 * Simulate the remote probe's behaviors for local testing.
 *
 * Instructions:
 * 1. Create two devices, you'll see the paths in the output:
 *    socat -d -d pty,raw,echo=0 pty,raw,echo=0
 *
 * 2. Start this script using one of the devices above:
 *    node dummy-probe.js --device=/dev/pts/5
 *
 * 3. Start the express app using the other device:
 *    node app.js --device=/dev/pts/7
 *
 */
 
var
	util = require('util')
	, serialport = require('serialport')
	, args = require('optimist').argv
;

function findDevice(device) {

	console.log("Device: " + device);

	if (device === undefined) { 
		console.log("ERROR! No device found!");
		process.exit(1);
	}

	return new serialport.SerialPort(device
		, { baudrate: 38400, 
			parser: serialport.parsers.readline("\n") }
		);
}

remoteProbe = findDevice(args.device);

// Serial port handler
remoteProbe.on("open", function () {

	console.log('Serial port is open!');
	
	remoteProbe.on("data", function(data){

		console.log("Data|" + data.toString() +"|");	

		if (data.toString() === "cmd::getTemp!") {
			console.log("Getting temp!");
			//	getTemp();
		}
	});

});

function getTemp() {
	remoteProbe.write("{\"temperature\":74.6,\"humidity\":45}");
}

