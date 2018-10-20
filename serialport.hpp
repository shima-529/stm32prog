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
		void writeChar(char ch);
		void writeStr(std::string str);
};
