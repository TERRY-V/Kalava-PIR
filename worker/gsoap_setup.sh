#!/bin/sh

# worker soapcpp2 setup
# Written by TERRY-V

foo() {
	cd ./src
	echo "Generating worker gsoap codes..."
	../../gsoap-2.8/gsoap/bin/linux386/soapcpp2 search.h -S -L -I ../../gsoap-2.8/gsoap/import
	cd ..
}

foo

exit 0

