namespace Operation {
	uint32_t fileSize(std::ifstream &ifs);
	int write(char *portPath);
	int read(char *portPath);
	int monitor(char *portPath);
} // namespace Operation
