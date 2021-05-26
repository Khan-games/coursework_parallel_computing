#include "t_output.h"

namespace cons {
	std::mutex output_m;
	void print(std::string str, std::string color) {
		std::lock_guard<std::mutex> lg(output_m);
		std::cout << color << str << RESET << std::endl;
	}
}
