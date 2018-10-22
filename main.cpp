#include <iostream>
#include <fstream>
#include <unistd.h>
#include "stm32.hpp"
#include "argInfo.hpp"
#include "monitor.hpp"
#include "signal.hpp"
#include <signal.h>

bool isValidArguments(int argc, char **argv) {
	if( argc < 2 ) return false;

	for(int i=1; i<argc; i++){
		if( argv[i][0] == '-' ) {
			switch(argv[i][1]) {
				case 'w':
					if( i + 1 >= argc ) {
						return false;
					}
					ArgInfo::binPath = argv[i+1];
					ArgInfo::op = ArgInfo::Operation::Write;
					i++;
					break;
				case 'b':
					if( i + 1 >= argc ) {
						return false;
					}
					ArgInfo::baud = std::atoi(argv[i+1]);
					i++;
					break;
				case 'm':
					ArgInfo::op = ArgInfo::Operation::Monitor;
					break;
				default:
					return false;
					break;
			}
		}
	}
	return true;
}

uint32_t fileSize(std::ifstream &ifs) {
	ifs.seekg(0, std::ifstream::end);
	uint32_t ret = ifs.tellg();
	ifs.seekg(0, std::ifstream::beg);
	return ret;
}

int writeOperation(char *portPath) {
	std::ifstream binary(ArgInfo::binPath, std::ifstream::binary);
	if( !binary ) {
		std::cerr << "failed to open file '" << ArgInfo::binPath << "'" << std::endl;
		return 1;
	}

	uint32_t binSize = fileSize(binary);
	uint8_t buf[binSize];

	SerialPort port(portPath, 115200, true);
	STM32 stm32(&port);
	if( !stm32.init() ) {
		std::cerr << "Initialization failed" << std::endl;
		return 1;
	}
	stm32.globalErase();
	std::cout << "Erased" << std::endl;

	std::cout << "Binary Size: " << std::dec << binSize << std::endl;
	uint32_t offset;
	binary.read(reinterpret_cast<char *>(buf), binSize);

	std::cout << std::endl << "Flash: " << std::endl;
	offset = 0;
	while( offset * 256 < binSize ) {
		std::cout << std::dec << offset << "/" << binSize / 256 << "\r";
		std::cout.flush();
		bool ret = stm32.writeMemory(0x08000000 + offset * 256, &buf[offset * 256], std::min(256u, binSize - offset * 256));
		if( ret == false ) std::cout << "Fail" << std::endl;
		offset++;
	}
	std::cout << std::endl;
	stm32.go(0x08000000);
	return 0;
}

int monitorOperation(char *portPath) {
	SerialPort port(portPath, ArgInfo::baud, false);

	SerialMonitor monitor(&port);
	monitor.startMonitor();
	return 0;
}

/* TODO
 * - impl baud
 * - file output
 * - write => serial
 * - SIGINT
 * - read
 */

int main(int argc, char *argv[]){
	struct sigaction action;
	action.sa_sigaction = signal_handler;
	action.sa_flags = SA_SIGINFO;
	if ( sigaction(SIGINT, &action, NULL) < 0 ) {
		return 1;
	}

	if( !isValidArguments(argc, argv) ) {
		std::cerr << "Usage: " << argv[0] << " <portName(cu)> [-w <fileName>]" << std::endl;
		return 1;
	}


	int ret;
	switch(ArgInfo::op) {
		case ArgInfo::Operation::Write:
			ret = writeOperation(argv[1]);
			break;
		case ArgInfo::Operation::Monitor:
			ret = monitorOperation(argv[1]);
			break;
	}
	return ret;
}
