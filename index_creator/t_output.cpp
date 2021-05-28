#include "t_output.h"

namespace cons {
	std::mutex output_m;

	void print(std::string str, std::string color) {
		std::lock_guard<std::mutex> lg(output_m);
		std::cout << color << str << RESET << std::endl;
	}

	void enable_console_colors() {
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode = 0;
		GetConsoleMode(hOut, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);
	}
}
