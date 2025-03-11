
#include <cassert>
#include <iostream>

#include "Application.h"

int main(int argc, char *argv[]) {
	assert(argc == 2);

	Application app = Application();
	app.run(std::filesystem::path(std::string(argv[1])));

	return 0;
}
