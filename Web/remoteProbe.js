/*
 * remoteProbe.js
 * State manager for the remote bluetooth module. Creates a child process to
 * interact with the serial connection.
 */
var
	argv = require('optimist').argv
	, util = require('util')
	, serialport = require('serialport')
	, probeOptions = argv.probeOptions || { 
		baudrate: 38400, parser: serialport.parsers.readline("\n") }
	, probePort = argv.probePort || "/dev/rfcomm0"
;

module.exports = function () {
	console.log("Probe listening on " + probePort);
	if (probeOptions.baudrate) {
		return new serialport.SerialPort(probePort, probeOptions);
	} else {
		return new serialport.SerialPort(probePort);
	}
}
