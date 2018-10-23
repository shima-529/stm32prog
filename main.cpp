#include <iostream>
#include <fstream>
#include <unistd.h>
#include "stm32.hpp"
#include "argInfo.hpp"
#include "operation.hpp"
#include "monitor.hpp"
#include "progress.hpp"
#include "signal.hpp"
#include <signal.h>

bool isValidArguments(int argc, char **argv) {
	if( argc < 2 ) return false;

	for(int i=1; i<argc; i++){
		if( argv[i][0] == '-' ) {
			switch(argv[i][1]) {
				case 'w':
					if( i + 1 >= argc ) {
						return false;
					}
					ArgInfo::binPath = argv[i+1];
					ArgInfo::op = ArgInfo::Operation::Write;
					i++;
					break;
				case 'b':
					if( i + 1 >= argc ) {
						return false;
					}
					ArgInfo::baud = std::atoi(argv[i+1]);
					i++;
					break;
				case 'm':
					ArgInfo::op = ArgInfo::Operation::Monitor;
					break;
				default:
					return false;
					break;
			}
		}
	}
	return true;
}


/* TODO
 * - impl baud
 * - file output
 * - write => serial
 * - SIGINT
 * - read
 */

int main(int argc, char *argv[]){
	struct sigaction action;
	action.sa_sigaction = signal_handler;
	action.sa_flags = SA_SIGINFO;
	if ( sigaction(SIGINT, &action, NULL) < 0 ) {
		return 1;
	}

	if( !isValidArguments(argc, argv) ) {
		std::cerr << "Usage: " << argv[0] << " <portName(cu)> [-w <fileName>]" << std::endl;
		return 1;
	}


	int ret;
	switch(ArgInfo::op) {
		case ArgInfo::Operation::Write:
			ret = Operation::write(argv[1]);
			break;
		case ArgInfo::Operation::Monitor:
			ret = Operation::monitor(argv[1]);
			break;
	}
	return ret;
}
