#!/bin/sh

# This is a shell program for installing Kalava server
# Written by TERRY-V

kalava_dir=/server/kalava

echo "Welcome to the kalava service installler"
echo "This script will help you easily set up a running kalava server"

chmod +x asciilogo
./asciilogo

echo -n "Are you sure to install the Kalava service? Please answer yes to continue:"
read yesorno
if [ "$yesorno" = "yes" ]; then
	echo
else
	echo "canceled!"
	exit 1
fi

echo "Kalava sentinel service compiling..."
cd $kalava_dir/sentinel
echo "Kalava sentinel uses gsoap to generate servers and clients..."
./gsoap_setup.sh
echo "Kalava sentinel uses cmake to compile the project..."
cd build
cmake ..
make clean
make
echo "Kalava sentinel building okay..."
echo ""

sleep 1

echo "Kalava master service compiling..."
cd $kalava_dir/master
echo "Kalava master service deleting rdb files..."
rm build/bin/vb.bss
echo "Kalava master uses gsoap to generate servers and clients..."
./gsoap_setup.sh
echo "Kalava master uses cmake to compile the project..."
cd build
cmake ..
make clean
make
echo "Kalava master building okay..."
echo ""

sleep 1

echo "Kalava worker service compiling..."
cd $kalava_dir/worker
echo "Kalava worker service deleting rdb files..."
rm build/bin/__*.rdb
echo "Kalava worker uses gsoap to generate servers and clients..."
./gsoap_setup.sh
echo "Kalava worker uses cmake to compile the project..."
cd build
cmake ..
make clean
make
echo "Kalava master building okay..."
echo ""

sleep 1

echo "Starting up the Kalava sentinel service..."
echo -n "Are you sure to install the Kalava sentinel service? Please answer yes to continue:"
read yesorno
if [ "$yesorno" = "yes" ]; then
	echo "Checking the Kalava sentinel service..."
	ps x | grep "sentinel" | grep "yunshitu"
	if [ $? -ne 0 ]; then
		echo "Kalava sentinel service is not running..."
	else
		echo "We checked the Kalava sentinel service is already running..."
		echo -n "Are you sure to stop the service and restart? Please answer yes to continue:"
		read restart_yes_or_no

		if [ "$restart_yes_or_no" = "yes" ]; then
			echo "Stopping the Kalava sentinel service..."
			ps x | grep "sentinel" | grep "yunshitu" | awk '{print $1}' | xargs kill -9
			echo "Kalava sentinel service stops successfully..."
		else
			echo "canceled!"
			exit 1
		fi
	fi
else
	echo "canceled!"
	exit 1
fi

sleep 1

echo "entering the Kalava sentinel directory..."
cd $kalava_dir/sentinel/build/bin

sleep 1

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
