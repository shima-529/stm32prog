#include <unistd.h>
#include <iostream>
#include "stm32.hpp"

STM32::STM32(SerialPort *port) : port(port) {}

bool STM32::isACK() {
	return port->readChar() == ACK;
}

bool STM32::init() {
	this->port->writeChar(INIT);
	if( !this->isACK() ) {
		return false;
	}
	std::cout << "Connection to STM32 is established." << std::endl;

	this->port->writeChar(0x00);
	this->port->writeChar(0xFF);
	if( !this->isACK() ) {
		return false;
	}
	std::cout << "Number of available commands: " << +this->port->readChar() << std::endl;
	std::cout << "Bootloader Version: 0x" << std::hex << +this->port->readChar() << std::endl;
	for(int i=0; i<11; i++){
		this->command[i] = this->port->readChar();
	}
	if( !this->isACK() ) {
		return false;
	}
	std::cout << std::endl;
	return true;
}

void STM32::readAllData(uint8_t *dest) {
	char ch;
	int idx = 0;
	while( (ch = this->port->readChar()) != ACK ) {
		dest[idx++] = ch;
	}
}

bool STM32::globalErase() {
	this->port->writeChar(command[ERASE]);
	this->port->writeChar(~command[ERASE]);
	if( !this->isACK() ) {
		return false;
	}
	if( command[ERASE] == 0x43 ) {
		this->port->writeChar(0xFF);
		this->port->writeChar(0x00);
	}
	if( command[ERASE] == 0x44 ) {
		this->port->writeChar(0xFF);
		this->port->writeChar(0xFF);
		this->port->writeChar(0x00);
	}

	if( !this->isACK() ) {
		return false;
	}
	return true;
}

[[deprecated]]
bool STM32::eraseAt(unsigned int *pages, int size) {
	this->port->writeChar(command[ERASE]);
	this->port->writeChar(~command[ERASE]);
	if( !this->isACK() ) {
		return false;
	}
	if( command[ERASE] == 0x43 ) {
		unsigned char checksum = size - 1;
		unsigned char ch = size - 1;
		for(int i=0; i<size; i++) {
			checksum ^= pages[i];
			ch += pages[i];
		}
		ch += checksum;

		this->port->writeChar(ch);
	}
	if( command[ERASE] == 0x44 ) {
	}
	if( !this->isACK() ) {
		return false;
	}
	
	return true;
}

bool STM32::readMemory(uint32_t addr, uint8_t *dest, int datSize) {
	this->port->writeChar(command[READ_MEMORY]);
	this->port->writeChar(~command[READ_MEMORY]);
	if( !this->isACK() ) {
		return false;
	}
	uint8_t addrs[4];
	uint8_t checksum;

	checksum = 0;
	for(int i=0; i<4; i++) {
		addrs[i] = addr >> (8*(3-i));
		checksum ^= addrs[i];
	}
	this->port->writeStr(addrs, 4);
	this->port->writeChar(checksum);
	if( !this->isACK() ) {
		return false;
	}

	this->port->writeChar(datSize - 1);
	this->port->writeChar(~(datSize - 1));
	if( !this->isACK() ) {
		return false;
	}
	this->port->readStr(dest, datSize);

	return true;
}

bool STM32::writeMemory(uint32_t addr, uint8_t *dat, int datSize) {
	if( datSize % 4 != 0 ) {
		std::cerr << "Data size must be 4bit-aligned." << std::endl;
		return false;
	}
	if( addr % 4 != 0 ) {
		std::cerr << "Target address must be 4bit-aligned." << std::endl;
		return false;
	}
	this->port->writeChar(command[WRITE_MEMORY]);
	this->port->writeChar(~command[WRITE_MEMORY]);
	if( !this->isACK() ) {
		return false;
	}
	uint8_t addrs[4];
	uint8_t checksum;

	checksum = 0;
	for(int i=0; i<4; i++) {
		addrs[i] = addr >> (8*(3-i));
		checksum ^= addrs[i];
	}
	this->port->writeStr(addrs, 4);
	this->port->writeChar(checksum);
	if( !this->isACK() ) {
		return false;
	}

	this->port->writeChar(datSize - 1);
	checksum = datSize - 1;
	for(int i=0; i<datSize; i++) {
		checksum ^= dat[i];
	}
	this->port->writeStr(dat, datSize);
	this->port->writeChar(checksum);
	if( !this->isACK() ) {
		return false;
	}

	return true;
}

bool STM32::go(uint32_t addr) {
	this->port->writeChar(command[GO]);
	this->port->writeChar(~command[GO]);
	if( !this->isACK() ) {
		return false;
	}

	uint8_t addrs[4];
	uint8_t checksum;

	checksum = 0;
	for(int i=0; i<4; i++) {
		addrs[i] = addr >> (8*(3-i));
		checksum ^= addrs[i];
	}
	this->port->writeStr(addrs, 4);
	this->port->writeChar(checksum);
	if( !this->isACK() ) {
		return false;
	}
	return true;
}
