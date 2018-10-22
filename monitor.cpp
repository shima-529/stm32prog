#include "monitor.hpp"

SerialMonitor::SerialMonitor(SerialPort *port) : port(port) {}

void SerialMonitor::startMonitor() {
	setbuf(stdout, NULL);
	while( true ) {
		putchar(this->port->readChar());
	}
}
