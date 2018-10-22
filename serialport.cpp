#include "serialport.hpp"
#include "argInfo.hpp"

#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

SerialPort::SerialPort(const char *portPath, uint32_t baud, bool parity) {
	this->fd = ::open(portPath, O_RDWR | O_NOCTTY | O_NDELAY);

	this->open(baud, parity);
}

SerialPort::~SerialPort() {
	this->close();
}
//           tcflag_t c_iflag;      /* input modes */
//           tcflag_t c_oflag;      /* output modes */
//           tcflag_t c_cflag;      /* control modes */
//           tcflag_t c_lflag;      /* local modes */
//           cc_t     c_cc[NCCS];   /* special characters */

bool SerialPort::open(uint32_t baud, bool parity) {
	if( fd < 0 ) {
		throw "Failed to open port";
		return false;
	}
	fcntl(fd, F_SETFL, 0);
	ArgInfo::portFd = fd;

	tcgetattr(fd, &this->oldtio); // store the current tty attr
	tcgetattr(fd, &this->newtio); // store the current tty attr
	cfmakeraw(&newtio);
	cfsetispeed(&newtio, 115200);
	cfsetospeed(&newtio, 115200);

	newtio.c_oflag = 0;
	newtio.c_cflag = CS8 | CREAD | CLOCAL; // enable read, 115200, 8bit, ignore model control line
	newtio.c_lflag = 0; // non-canonical
	if( parity ) {
		newtio.c_iflag = INPCK; // enable input parity check
		newtio.c_cflag |= PARENB; // parity
	}else{
		newtio.c_iflag = 0;
	}

	// non-blocking read
	newtio.c_cc[VTIME] = 5;
	newtio.c_cc[VMIN] = 1;

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

int SerialPort::readStr(char *dest, int size) {
	int ret;
	ret = read(fd, dest, size);
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
