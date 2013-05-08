/*
 * dummy-probe.js
 * Simulate the remote probe's behaviors for local testing.
 *
 * Instructions:
 * 1. Create two devices, you'll see the paths in the output:
 *    socat -d -d pty,raw,echo=0 pty,raw,echo=0
 *
 * 2. Start this script using the first device from the command above:
 *    node dummy-probe.js --port=/dev/pts/5
 *
 * 3. Start the express app using the other device:
 *    node app.js --probePort=/dev/pts/7 --probeOptions {}
 *
 */

var
	util = require('util'),
	serialport = require('serialport'),
	argv = require('optimist').argv,
	mode = argv.mode || 'prod',
	port = argv.port || '/dev/rfcomm0',
	options = argv.options || {
		baudrate: 38400, 
		parser: serialport.parsers.readline("\n") 
	} ,
	_probe = {
		tempMin: 5,
		tempMax: 24,
		humidity: 16 
	},
	ezlog = function(data) { console.log("[DummyProbe]	" + data); };

var remoteProbe = (function() {
	if (options.baudrate) {
		return new serialport.SerialPort(port, options);
	} else {
		return new serialport.SerialPort(port);
	}
})();

if (mode == 'test') {
	ezlog("Entering test mode.");
	setInterval(testWrite, 2000);
}

function testWrite() {
	ezlog("Test message from port " + port);
	remoteProbe.write("Test message from port " + port);
}

// Serial port handler
remoteProbe.on("open", function () {

	ezlog("Serial port " + port + " is open.");

	remoteProbe.on("data", function(data){

		ezlog("Received message: " + data.toString());

		if (data.toString() === 'cmd::getTemp!') {
			sendTempData();
		}
	});

});

// getRandomArbitrary, found on MDN. Great resource!
function getRandom(min, max) {
  return parseFloat((Math.random() * (max - min) + min).toPrecision(2));
}	

function sendTempData() {
	var tempObj = {},
		message = null;

	tempObj.temperature = getRandom(_probe.tempMin,_probe.tempMax);
	tempObj.humidity = getRandom(5,69);
	message = JSON.stringify(tempObj);

	ezlog("Sending message: " + message);

	remoteProbe.write(message+"\n");
}

