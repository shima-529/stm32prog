#include <iostream>
#include <fstream>
#include <string>
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
		std::ifstream binary(ArgInfo::binPath, std::ifstream::binary | std::ios::in);
		if( !binary ) {
			std::cerr << "failed to open file '" << ArgInfo::binPath << "'" << std::endl;
			return 1;
		}

		uint32_t binSize = fileSize(binary);
		if( binSize % 4 != 0 ) {
			binSize = (binSize & ~0x03) + (1 << 2);
		}
		uint8_t buf[binSize] = {};

		SerialPort port(portPath, ArgInfo::baud, true);
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

		auto f = [](uint32_t val) {
			return (val & 255) != 0;
		};

		std::cout << std::endl << "Flash: " << std::endl;
		offset = 0;
		while( offset * 256 < binSize ) {
			stm32.writeMemory(0x08000000 + offset * 256, &buf[offset * 256], std::min(256u, binSize - offset * 256));
			offset++;
			Progress::show(const_cast<char *>(""), const_cast<char *>(""), offset, (binSize / 256) + f(binSize));
		}
		std::cout << std::endl;

		std::cout << "Resetting..." << std::endl;
		stm32.go(0x08000000);
		return 0;
	}

	int read(char *portPath) {
		std::ofstream binary(ArgInfo::binPath, std::ofstream::binary | std::ios::out);
		if( !binary ) {
			std::cerr << "failed to open file '" << ArgInfo::binPath << "'" << std::endl;
			return 1;
		}


		uint32_t size = std::atoi(ArgInfo::sizeStrForRead);
		std::string str(ArgInfo::sizeStrForRead);
		if( str.find('k') != std::string::npos) {
			size *= 1024;
		}
		uint8_t buf[size];

		SerialPort port(portPath, ArgInfo::baud, true);
		STM32 stm32(&port);
		if( !stm32.init() ) {
			std::cerr << "Initialization failed" << std::endl;
			return 1;
		}

		std::cout << "Binary Size: " << std::dec << size << std::endl;

		std::cout << std::endl << "Read: " << std::endl;
		uint32_t offset;
		offset = 0;

		auto f = [](uint32_t val) {
			return (val & 255) != 0;
		};

		while( offset * 256 < size ) {
			stm32.readMemory(0x08000000 + offset * 256, &buf[offset * 256], std::min(256u, size - offset * 256));
			offset++;
			Progress::show(const_cast<char *>(""), const_cast<char *>(""), offset, (size / 256) + f(size));
		}
		binary.write(reinterpret_cast<char *>(buf), size);

		std::cout << "Resetting..." << std::endl;
		stm32.go(0x08000000);
		std::cout << std::endl;
		return 0;
	}

	int monitor(char *portPath) {
		SerialPort port(portPath, ArgInfo::baud, false);

		SerialMonitor monitor(&port);
		monitor.startMonitor();
		return 0;
	}
} // namespace Operation
