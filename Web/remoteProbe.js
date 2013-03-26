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

module.exports = (function () {

	var _probe = {
			temperature: 0
			, humidity: 0
			, lastUpdated: '1970/1/1'
			, port: connectSerial()
		}
	;

	function connectSerial() {
		console.log("Probe connecting on " + probePort);
		if (probeOptions.baudrate) {
			return new serialport.SerialPort(probePort, probeOptions);
		} else {
			return new serialport.SerialPort(probePort);
		}
	}

	// Serial port handler
	_probe.port.on("open", function () {
		console.log('Probe serial port is open!');
		
		var sendGetTemp = function () { 
			console.log("Probe - sending getTemp message.");
			_probe.port.write("cmd::getTemp!\n");
		}

		// Let's do this regularly!
		setInterval(sendGetTemp, 5000);

		// Receive data.
		_probe.port.on("data", function(data){

			console.log("Probe - received data: " + data);

			var _data;

			try {
				_data = JSON.parse(data);
			} catch (err) {
				// Swallow for now
				console.log("Json parse error: " + err);
				return;
			}

			if(_data.temperature) {
				 _probe.temperature = _data.temperature;
			}

			if(_data.humidity) {
				_probe.humidity = _data.humidity;
			}

			console.log("Probe temp is " + _probe.temperature + " celcius and humidity is " + _probe.humidity);
		});

	});

	return {
		getTemp: function() {
			return {
				temperature: _probe.temperature 
				, humidity: _probe.humidity
			};
		}
	};
	
})(); // end module.exports
