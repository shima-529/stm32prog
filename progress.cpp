#include <iostream>
namespace Progress {
	constexpr int width = 40;
	void show(char *prefix, char *postfix, uint32_t val, uint32_t max) {
		uint32_t outputLen = (val + 0.0) / max * width;

		std::cout << prefix << " ";
		std::cout << "[";
		for(uint32_t i=0; i <= outputLen; i++){
			std::cout << "=";
		}
		for(uint32_t i=0; i < width-outputLen; i++){
			std::cout << " ";
		}
		std::cout << "] " << postfix << "\r";
		std::cout.flush();
	}
} // namespace Progress
