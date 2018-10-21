#include <termios.h>
#include <string>
class SerialPort {
	private:
		int fd;
		struct termios newtio, oldtio;

	public:
		SerialPort(const char *portPath);
		~SerialPort();
		bool open();
		void close();
		char readChar();
		void writeChar(uint8_t ch);
		int writeStr(uint8_t *str, int length);
		void flush();
};
