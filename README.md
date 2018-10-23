# stm32prog
A UART programmer for STM32

## Make & Run
```bash
$ make
$ ./stm32prog /dev/cu.SerialPort -w /path/to/.bin_file # erase, write and boot the program
```

## Operation
The following operation is now implemented.

- Baud Rate
	- `-b baud` to designate baud rate (by default **115200**bps)
- Write
	- `-w` (*e.g.*: `$ ./stm32prog /dev/cu.SerialPort -w /path/to/.bin_file`)
- Read
	- `-r` (*e.g.*: `$ ./stm32prog /dev/cu.SerialPort -r /path/to/.bin_file_for_save 64k`)
- Serial Monitor(Only for receiving data)
	- `-m` (*e.g.*: `$ ./stm32prog /dev/cu.SerialPort -m -b 19200`)
