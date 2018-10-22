#include "serialport.hpp"

class STM32 {
	SerialPort *port;
	constexpr static char ACK = 0x79;
	constexpr static char INIT = 0x7F;
	enum {
		GET, GET_VERSION_READ_PROTECTION, GET_ID, READ_MEMORY, GO, WRITE_MEMORY, ERASE, WRITE_PROTECT, WRITE_UNPROTECT, READOUT_PROTECT, READOUT_UNPROTECT, _SIZE
	};
	char command[_SIZE];
	bool isACK();

	public:
	STM32(SerialPort *port);
	bool init();
	void readAllData(uint8_t *dest);
	bool globalErase();
	bool eraseAt(unsigned int *pages, int size);
	bool writeMemory(uint32_t addr, uint8_t *dat, int datSize);
	bool go(uint32_t addr);
};
