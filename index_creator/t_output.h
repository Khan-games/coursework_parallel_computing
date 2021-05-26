#pragma once

#include <iostream>
#include <mutex>
#include <string>

// colors
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[0m"		/* White */
#define RESET   "\033[0m"		/* Reset */

namespace cons {
	extern std::mutex output_m;
	void print(std::string str, std::string color = RESET);
}



