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
		int readStr(char *dest, int size);
		void writeChar(uint8_t ch);
		int writeStr(uint8_t *str, int length);
		void flush();
};
