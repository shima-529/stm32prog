#pragma once
#include <termios.h>
#include <string>
class SerialPort {
	private:
		int fd;
		struct termios newtio, oldtio;

	public:
		SerialPort(const char *portPath, uint32_t baud, bool parity);
		~SerialPort();
		bool open(uint32_t baud, bool parity);
		void close();
		char readChar();
		int readStr(uint8_t *dest, uint32_t size);
		void writeChar(uint8_t ch);
		int writeStr(uint8_t *str, uint32_t length);
		void flush();
};
