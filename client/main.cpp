#include <iostream>
#include <chrono>
#include <mutex>
#include <boost/thread.hpp>

#include "t_output.h"

std::thread t;

void func() {
	cons::print("TEXT", GREEN);
}

int main() {
	boost::thread_group tg;
	for (int i = 0; i < 10; i++) {
		tg.create_thread(func);
	}
	
	tg.join_all();
}