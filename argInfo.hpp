#include <cstdint>
namespace ArgInfo {
	extern char *binPath;
	extern int portFd;
	extern uint32_t baud;
	enum struct Operation {
		Write, Monitor,
	};
	extern Operation op;
} // namespace ArgInfo
