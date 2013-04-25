/*
 * alert.js
 *
 * Send an alert (sms) via Twilio.
 *
 */

var util = require('util'),
	fs = require('fs'),
	twilio = require('twilio'),
	config = {
		twilio_acct_sid: "",
		twilio_auth_token: "",
		phone_from: "",
		phone_to: "",
		threshold: 0, // Number of times the alarm can trip before notification.
		resetTimeout: 15, // Auto-clear alarm in X minutes
		minTemp: 40,
		maxTemp: 95,
		uom: "F"
	},
	client,
	configFile
;

// Read the config file, the keys are listed in the "config" object defined above.
var cfgFile = fs.readFileSync('./alert-config.json', {encoding:"utf8"});
if (cfgFile) {
	configFile = JSON.parse(cfgFile);
}

// Merge defaults and config file values.
for (var key in configFile) {
	if (config.hasOwnProperty(key)) {
		config[key] = configFile[key];
	}
}

// Create the client
client = new twilio.RestClient(config.twilio_acct_sid, config.twilio_auth_token);

// Alert logic
var alert = function() {

	var status = "armed",
		counter = 0; // armed - disarmed - tripped

	// Clear the alarm condition.
	function reset() {
		status = "armed";
	}

	function process(data) {
		if (config.uom = "F") {
			data.temperature = data.temperature * 9/5 + 32;
		}
		if (data.temperature > config.maxTemp || data.temperature < config.minTemp) {
			console.log("Temp exceeded: " + data.temperature);

			if (counter == config.threshold && status == "armed") {
				console.log("Sending message!");
				send("Temperature threshold exceeded! Temp: " + data.temperature + "F, Humidity: " + data.humidity + "%");
				status = "tripped";
				setTimeout(reset, 1000*60*config.resetTimeout);
				counter = 0;
			} else {
				counter++;
			}
		}
	}

	// Send a message via Twilio
	function send(message) {
		client.sms.messages.create({
				to: config.phone_to,
				from: config.phone_from,
				body: message
			},
			function(err, response) {
				if (!err) {
					console.log("Message sent! " + message);
				} else {
					console.log("Error sending message! " + util.inspect(err));
				}
			}
		);
	}

	function getStatus() {
		return status;
	}

	return {
		process: process,
		status: getStatus
	}; 
};

module.exports = alert;
