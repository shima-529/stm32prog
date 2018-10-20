#include "serialport.hpp"

#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

SerialPort::SerialPort(const char *portPath) {
	this->fd = ::open(portPath, O_RDWR | O_NOCTTY);

	this->open();
}

SerialPort::~SerialPort() {
	this->close();
}
//           tcflag_t c_iflag;      /* input modes */
//           tcflag_t c_oflag;      /* output modes */
//           tcflag_t c_cflag;      /* control modes */
//           tcflag_t c_lflag;      /* local modes */
//           cc_t     c_cc[NCCS];   /* special characters */

bool SerialPort::open() {
	if( fd < 0 ) {
		return false;
	}

	tcgetattr(fd, &this->oldtio); // store the current tty attr

	newtio.c_iflag = IGNBRK | IGNPAR; // ignore BREAK and Parity
	newtio.c_oflag = 0;
	newtio.c_cflag = B115200 | CS8 | CREAD | CLOCAL; // enable read, 115200, 8bit, ignore model control line
	newtio.c_lflag = 0; // non-canonical

	// blocking read
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 1;

	tcflush(fd, TCIOFLUSH); // flush
	tcsetattr(fd, TCSANOW, &this->newtio); // set tty attr immediately
	return true;
}

void SerialPort::close() {
	tcsetattr(this->fd, TCSANOW, &this->newtio); // restore tty attr (TCSANOW: apply immediately)
	::close(this->fd);
}

char SerialPort::readChar() {
	char ret;
	read(fd, &ret, 1); // read 1 byte
	return ret;
}

void SerialPort::writeChar(char ch) {
	write(fd, &ch, 1);
}

void SerialPort::writeStr(std::string str) {
	for( auto ch : str ) {
		this->writeChar(ch);
	}
}
