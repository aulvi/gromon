/*
 * dummy-probe.js
 * Simulate the remote probe's behaviors for local testing.
 *
 * Instructions:
 * 1. Create two devices, you'll see the paths in the output:
 *    socat -d -d pty,raw,echo=0 pty,raw,echo=0
 *
 * 2. Start this script using one of the devices above:
 *    node dummy-probe.js --port=/dev/pts/5
 *
 * 3. Start the express app using the other device:
 *    node app.js --port=/dev/pts/7
 *
 */
 
var
	util = require('util')
	, serialport = require('serialport')
	, argv = require('optimist').argv
	, mode = argv.mode || 'prod'
	, port = argv.port || '/dev/rfcomm0'
//	, options = argv.options || {}
	, options = argv.options || { baudrate: 38400, parser: serialport.parsers.readline("\n") }
;

console.log("Port: " + port);

if (port === undefined) { 
	console.log("ERROR! No port specified!");
	process.exit(1);
}

if (mode == 'test') {
	console.log("Entering test mode.");
	setInterval(testWrite, 2000);
}

var remoteProbe;
if (options.baudrate) {
	remoteProbe = new serialport.SerialPort(port, options);
} else {
	remoteProbe = new serialport.SerialPort(port);
}

function testWrite() {
	console.log("Test message from port " + port);
	remoteProbe.write("Test message from port " + port);
}

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

