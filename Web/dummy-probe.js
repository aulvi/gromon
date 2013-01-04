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
	util = require('util')
	, serialport = require('serialport')
	, argv = require('optimist').argv
	, mode = argv.mode || 'prod'
	, port = argv.port || '/dev/rfcomm0'
	, options = argv.options || {}
	, _probe = {
		temperature: 76.4
		, humidity: 30
	}
;

console.log("Port: " + port);

var remoteProbe = (function() {
	if (options.baudrate) {
		return new serialport.SerialPort(port, options);
	} else {
		return new serialport.SerialPort(port);
	}
})();

if (mode == 'test') {
	console.log("Entering test mode.");
	setInterval(testWrite, 2000);
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

		if (data.toString() === 'cmd::getTemp!\n') {
			console.log("Received 'getTemp!' message.");
			sendTempData();
		}
	});

});

function sendTempData() {
	console.log("Sending TempData message.");
	remoteProbe.write("{\"temperature\":74.6,\"humidity\":45}");
}

