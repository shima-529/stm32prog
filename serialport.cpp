#include "serialport.hpp"

#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

SerialPort::SerialPort(const char *portPath) {
	this->fd = ::open(portPath, O_RDWR | O_NOCTTY | O_NDELAY);
	// this->fd = ::open(portPath, O_RDWR | O_NOCTTY | O_NONBLOCK);

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
		throw "Failed to open port";
		return false;
	}
	fcntl(fd, F_SETFL, 0);

	tcgetattr(fd, &this->oldtio); // store the current tty attr
	tcgetattr(fd, &this->newtio); // store the current tty attr
	cfmakeraw(&newtio);
	cfsetispeed(&newtio, B115200);
	cfsetospeed(&newtio, B115200);

	newtio.c_iflag = INPCK; // enable input parity check
	newtio.c_oflag = 0;
	newtio.c_cflag = CS8 | CREAD | CLOCAL | PARENB; // enable read, 115200, 8bit, ignore model control line, parity
	newtio.c_lflag = 0; // non-canonical

	// non-blocking read
	newtio.c_cc[VTIME] = 5;
	newtio.c_cc[VMIN] = 0;

	tcflush(fd, TCIFLUSH); // flush
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

void SerialPort::writeChar(uint8_t ch) {
	int ret = write(fd, &ch, 1);
	if( ret < -1 ) {
		throw "Transmission failed";
	}
}

int SerialPort::writeStr(uint8_t *str, int length) {
	int ret = write(fd, str, length);
	return ret;
}

void SerialPort::flush() {
	tcflush(fd, TCIOFLUSH);
}
