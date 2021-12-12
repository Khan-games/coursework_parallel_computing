#pragma once

#include <iostream>
#include <mutex>
#include <string>
#include <windows.h>
#include <deque>

#include <boost/multiprecision/cpp_int.hpp>

// for color enabling
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

// colors
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[0m"		/* White */
#define RESET   "\033[0m"		/* Reset */

namespace cons {
	extern std::mutex output_m;
	void print(std::string str, std::string color = RESET);
	void enable_console_colors();

	// convertions
	std::string toString(boost::multiprecision::cpp_int number);
	boost::multiprecision::cpp_int fromString(std::string str);
}



