#include <iostream>

#include "t_output.h"
#include "IndexCreator.h"

using namespace std;

int main() {
	cons::enable_console_colors();
	cons::print("\t--- INDEX CREATION STARTED ---", GREEN);

	// timing
	auto ch_start = std::chrono::high_resolution_clock::now();

	// run index creation
	IndexCreator ic("dataset", 6); // pass data folder and number of threadss
	ic.join_all();

	// timing
	auto ch_end = std::chrono::high_resolution_clock::now();
	int time = std::chrono::duration_cast<std::chrono::milliseconds>(ch_end - ch_start).count();
	cons::print("[TIME] Index creation took " + std::to_string(time) + "ms", CYAN);

	// saving
	ic.save("index.index");

	// timing
	ch_end = std::chrono::high_resolution_clock::now();
	time = std::chrono::duration_cast<std::chrono::milliseconds>(ch_end - ch_start).count();
	cons::print("[TIME] Index creation with saving took " + std::to_string(time) + "ms", CYAN);

	cons::print("\t--- INDEX CREATION ENDED ---", RED);
	std::getchar(); // pause console
}