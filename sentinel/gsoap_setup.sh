#!/bin/sh

# launcher soapcpp2 setup
# Written by TERRY-V

foo() {
	cd ./src
	echo "Generating launcher gsoap codes..."
	../../gsoap-2.8/gsoap/bin/linux386/soapcpp2 serlaunch.h -S -L -I ../../gsoap-2.8/gsoap/import
	cd ..
}

foo

exit 0

