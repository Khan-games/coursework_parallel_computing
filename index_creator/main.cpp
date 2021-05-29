#include <iostream>

#include "t_output.h"
#include "IndexCreator.h"



using namespace std;

int main() {
	cons::enable_console_colors();
	cons::print("\t--- INDEX CREATION STARTED ---", GREEN);

	IndexCreator ic("dataset", 1);
	ic.join_all();

	cons::print("\t--- INDEX CREATION STOPPED ---", RED);
	std::getchar(); // pause console
}