/*
 * index.js
 */

;(function() {
	console.log("Hello!");

	var socket = io.connect('http://localhost');

	socket.on('connect', function() {
		console.log("Connecting");
		socket.emit('getTemp');
	});

	socket.on('getTemp', function (data) {
		console.log('Received getTemp socket event!');
		console.log(data);
		$("#primary-uom").html((9/5 * data.temperature + 32).toPrecision(2) + "&#8457");
		$("#humidity").html(data.humidity + "% humidity");
	});

	$("#updateNow").live('click', function() {
		console.log("Sending getTemp");
		socket.emit('getTemp');
	});

})()
