#!/bin/sh

# This is a shell program for installing Kalava server
# Written by TERRY-V

kalava_dir=/server/kalava

echo "This script will help you start a running kalava server"

echo "entering the Kalava sentinel directory..."
cd $kalava_dir/sentinel/build/bin

echo "Starting the Kalava sentinel service..."
./serlaunch

ps x | grep "sentinel" | grep "yunshitu"
if [ $? -ne 0 ]; then
	echo "Kalava sentinel service starts up failed..."
	exit 1
else
	echo "Kalava sentinel service starts up okay..."
fi

echo "Installation successful!"

exit 0
