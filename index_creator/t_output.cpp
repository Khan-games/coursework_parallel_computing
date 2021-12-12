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

	std::string toString(boost::multiprecision::cpp_int number)
	{
		std::deque<unsigned char> temp;
		std::string result;
		while (number > 0) {
			temp.push_front((unsigned char)(number % 10));
			number /= 10;
		}
		result.resize(temp.size());
		for (size_t i = 0; i < result.size(); i++) {
			result[i] = temp.front() + '0';
			temp.pop_front();
		}
		return result;
	}

	boost::multiprecision::cpp_int fromString(std::string str)
	{
		boost::multiprecision::cpp_int result = 0;
		for (size_t i = 0; i < str.size(); i++) {
			result *= 10;
			result += str[i] - '0';
		}
		return result;
	}
}
