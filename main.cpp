#include <iostream>
#include "serialport.hpp"

bool isValidArguments(int argc, char **argv) {
	return argc == 2;
}

int main(int argc, char *argv[]){
	if( !isValidArguments(argc, argv) ) {
		return 1;
	}

	SerialPort port(argv[1]);

	port.writeChar(0x7F);
	char buf[10];
	sprintf(buf, "%X", port.readChar());
	std::cout << buf << std::endl;

	return 0;
}
