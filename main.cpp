#include <iostream>
#include <fstream>
#include <unistd.h>
#include "stm32.hpp"

std::ifstream binary;
bool isValidArguments(int argc, char **argv) {
	if( argc < 2 ) return false;

	for(int i=1; i<argc; i++){
		if( argv[i][0] == '-' ) {
			switch(argv[i][1]) {
				case 'w':
					binary.open(argv[i+1], std::ifstream::binary);
					i++;
					break;
				default:
					return false;
					break;
			}
		}
	}
	return true;
}

uint32_t fileSize() {
	binary.seekg(0, std::ifstream::end);
	uint32_t ret = binary.tellg();
	binary.seekg(0, std::ifstream::beg);
	return ret;
}

int main(int argc, char *argv[]){
	if( !isValidArguments(argc, argv) ) {
		std::cerr << "Usage: " << argv[0] << " <portName(cu)> [-w <fileName>]" << std::endl;
		return 1;
	}

	uint32_t binSize = fileSize();
	uint8_t buf[binSize];

	STM32 stm32(argv[1]);
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
}
