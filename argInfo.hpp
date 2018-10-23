#include <cstdint>
namespace ArgInfo {
	extern char *binPath;
	extern char *sizeStrForRead;
	extern int portFd;
	extern uint32_t baud;
	enum struct Operation {
		Write, Read, Monitor,
	};
	extern Operation op;
} // namespace ArgInfo
