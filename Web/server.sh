#!/bin/bash

# Grab the command line argument.
ARG=$1

# Ensure we are running via sudo.
if [ -z ${SUDO_USER} ]; then
	ARG=end
fi

case "$1" in
 
start)
	# Grab the hardware ID of the Bluetooth Bee. 
	BTNAME=ProbeMon
	HWID=`hcitool scan|grep ${BTNAME}|awk '{print $1}'`

	echo -e "\nPlease wait..\n"

	if [ ! -z "$HWID" -a "$HWID" != " " ]; then
		echo "	${BTNAME} found [${HWID}]"
		echo "	Starting up bluetooth-agent"
		bluetooth-agent 0000 &
		sleep 1
		echo "	Setting up RFCOMM"
		rfcomm release 0 > /dev/null
		rfcomm bind 0 ${HWID} 1
		sleep 1
		echo "	Starting up Node.js"
		nohup su ${SUDO_USER} -c "node ./app.js" &
		echo -e "\nDone!\n"
	else
		echo "	Bluetooth device ${BTNAME} could not be found, quitting.\n"
	fi
	;;

stop)
	echo -e "\n	NOT IMPLEMENTED\n"
	;;

*)
	echo "Usage: sudo $0 [start|stop]"
	;;

esac
		

