#!/bin/bash

ADDR_WEBSOCKET="192.168.1.113"
ADDR_ART="192.168.2.3"
socket_ok=false
count=0

# Verify that none is launched

# Launch wsclient with address ADDR_WEBSOCKET
./wsclient $ADDR_WEBSOCKET&
sleep 1
./artnet -a $ADDR_ART&

pid_ws=`pidof wsclient`
DIR_WSCLIENT="/proc/$pid_ws"
pid_art=`pidof artnet`
DIR_ART="/proc/$pid_art"


# Loop forever doing the following
#	1. if wsclient disapear kill tradart
#	2. if it happened for the 5th time reboot the board
while true;
do
	if [ ! -d $DIR_WSCLIENT ]; then
		echo "wsclient killed $pid"
		killall artnet
		./wsclient $ADDR_WEBSOCKET&
		pid_ws=`pidof wsclient`
		DIR_WSCLIENT="/proc/$pid_ws"
		
		sleep 1
		
		./artnet -a $ADDR_ART&
		pid_art=`pidof artnet`
		DIR_ART="/proc/$pid_art"

		count=$((count+1))
	elif [ ! -d $DIR_ART ]; then
		echo "artnet killed $pid"
		killall wsclient
		./wsclient $ADDR_WEBSOCKET&
		pid_ws=`pidof wsclient`
		DIR_WSCLIENT="/proc/$pid_ws"
		

		sleep 1
		./artnet -a $ADDR_ART&
		pid_art=`pidof artnet`
		DIR_ART="/proc/$pid_art"

		count=$((count+1))

	fi

	if [ $count -ge 5 ]; then
		count=0
		echo "REEEEEBOOOOOOOOT"
	fi
done



	

