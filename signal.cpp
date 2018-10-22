#include <cstdlib>
#include <signal.h>
#include <unistd.h>
#include "argInfo.hpp"
void signal_handler(int sig, siginfo_t *info, void *ctx) {
	if( sig == SIGINT ) {
		close(ArgInfo::portFd);
		exit(1);
	}
}
