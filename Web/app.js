/**
 * app.js
 */

var express = require('express'),
	http = require('http'),
	path = require('path'),
	util = require('util'),
	argv = require('optimist').argv,
	app = express(),
	server = http.createServer(app),
	io = require('socket.io').listen(server),
	routes = require('./routes'),
	remoteProbe = require('./remoteProbe.js'),
	ezlog = function (data) { console.log("[GroMon]	" + data); };

app.configure(function(){
	app.set('port', process.env.PORT || 3000);
	app.set('views', __dirname + '/views');
	app.set('view engine', 'ejs');
	app.use(express.favicon());
	app.use(express.logger('dev'));
	app.use(express.bodyParser());
	app.use(express.methodOverride());
	app.use(express.cookieParser('your secret here'));
	app.use(express.session());
	app.use(app.router);
	app.use(express.static(path.join(__dirname, 'public')));
});

app.configure('development', function(){
	ezlog("Development mode");
	app.use(express.errorHandler());
});

// Define some routes.
app.get('/', routes.index);

// Slap in some socket.io
io.sockets.on('connection', function(socket){

	socket.on('connect', function() {
		ezlog("New socket connection.");
	});

	socket.on('getTemp', function(){
		ezlog("Socket request to getTemp.");

		// Send reply to client.
		io.socket.emit('getTemp',  remoteProbe.getTemp());
	});

}); // end io.sockets.on()

// Start it up!
server.listen(app.get('port'), function(){
  ezlog("Server listening on port " + app.get('port'));
});


