#!/bin/sh

# This is a shell program for killing Kalava services.
# Written by TERRY-V

foo() {
	echo "Preparing for killing Kalava master and worker services..."
	echo -n "Are you sure to stop all Kalava services? Please answer yes to continue:"
	read yesorno
	if [ "$yesorno" = "yes" ]; then
		ps aux | grep "recgservice" | grep "yunshitu"
		if [ $? -ne 0 ]; then
			echo "No Kalava master or worker service is running!"
		else
			ps aux | grep "recgservice" | grep "yunshitu" | awk '{print $2}' | xargs kill -9
		fi
		echo "done"
	else
		echo "canceled!"
		exit 1
	fi
}

foo

exit 0
