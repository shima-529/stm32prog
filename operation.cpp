#include <iostream>
#include <fstream>
#include "argInfo.hpp"
#include "serialport.hpp"
#include "stm32.hpp"
#include "monitor.hpp"
#include "progress.hpp"

namespace Operation {
	uint32_t fileSize(std::ifstream &ifs) {
		ifs.seekg(0, std::ifstream::end);
		uint32_t ret = ifs.tellg();
		ifs.seekg(0, std::ifstream::beg);
		return ret;
	}

	int write(char *portPath) {
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
			Progress::show(const_cast<char *>(""), const_cast<char *>(""), offset, binSize / 256);
			// std::cout << std::dec << offset << "/" << binSize / 256 << "\r";
			// std::cout.flush();
			bool ret = stm32.writeMemory(0x08000000 + offset * 256, &buf[offset * 256], std::min(256u, binSize - offset * 256));
			// if( ret == false ) std::cout << "Fail" << std::endl;
			offset++;
		}
		std::cout << std::endl;
		stm32.go(0x08000000);
		return 0;
	}

	int monitor(char *portPath) {
		SerialPort port(portPath, ArgInfo::baud, false);

		SerialMonitor monitor(&port);
		monitor.startMonitor();
		return 0;
	}
} // namespace Operation
