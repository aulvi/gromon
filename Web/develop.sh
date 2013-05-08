#!/bin/bash
#
# Start up socat:
#
# socat -d -d pty,raw,echo=0 pty,raw,echo=0
#
# You will see the paths in the output

nodemon dummy-probe.js --port=/dev/pts/6
nodemon app.js --probePort=/dev/pts/7

