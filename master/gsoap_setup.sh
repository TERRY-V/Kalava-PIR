#!/bin/sh

# master soapcpp2 setup
# Written by TERRY-V

foo() {
	cd ./src
	echo "Generating master gsoap codes..."
	../../gsoap-2.8/gsoap/bin/linux386/soapcpp2 search.h -L -I ../../gsoap-2.8/gsoap/import
	cd ..
}

foo

exit 0

