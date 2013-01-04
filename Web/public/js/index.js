/*
 * index.js
 */

;(function() {
	console.log("Hello!");

	var socket = io.connect('http://localhost');

	socket.on('getTemp', function (data) {
		console.log('Received getTemp socket event!');
		console.log(data);
	});

	$("#updateNow").live('click', function() {
		console.log("Sending getTemp");
		socket.emit('getTemp');
	});

})()
