/*
 * index.js
 */

;(function() {
	var socket = io.connect(),
		probeData = { tempF: 0, tempC: 0, humidity: 0 };

	socket.on('connect', function() {
		console.log("Connecting");
		socket.emit('getTemp');
	});

	socket.on('getTemp', function (data) {
		console.log('Received getTemp socket event!');
		console.log(data);

		// Save the data.
		probeData.tempC = data.temperature;
		probeData.tempF = (9/5 * data.temperature + 32).toPrecision(2);
		probeData.humidity = data.humidity;

		// Update the display.
		updateDisplay();
	});
	
	// Little function we can call to "paint" the display.
	var updateDisplay = function() {
		$("#primary-uom").html(probeData.tempF + "&#8457").fadeOut('fast').fadeIn('slow');
		$("#humidity").html(probeData.humidity + "% humidity");
	}

	// Button
	$("#updateNow").live('click', function() {
		console.log("Sending getTemp");
		socket.emit('getTemp');
	});

})()
