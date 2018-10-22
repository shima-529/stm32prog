#include "serialport.hpp"
class SerialMonitor {
	SerialPort *port;
	char buffer[BUFSIZ];
	public:
		bool signalInvoked = false;
		SerialMonitor(SerialPort *port);
		void startMonitor();
};
