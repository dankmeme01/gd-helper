#include "instructions.h"

int main(int argc, const char** argv) {
	CURL* curl;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	SetCurl(curl);
	std::string _inp;
	std::cout << "Enter an instruction list ID, -1 for manual instruction or -2 to exit" << std::endl;
	while (true) {
		std::cout << ">";
		getline(std::cin, _inp);
		_inp = trim(_inp);
		if (_inp.size() == 0) continue;
		try {
			int dis = std::atoi(_inp.c_str());
			if (dis == -1) {
				std::cout << "Input instruction list (instructions separated by semicolons and arguments by commas)" << std::endl << ">";
				getline(std::cin, _inp);
				Instruction inst(_inp);
				inst.execute();
			}
			else if (dis == -2) {
				return 0;
			}
			else {
				Instruction inst(std::atoi(_inp.c_str()));
				inst.execute();
			}
		}
		catch (std::exception& e) {
			std::cout << "Oh no! An error has occured during executing instructions:" << std::endl;
			std::cerr << e.what() << std::endl;
		}
	}
	return 0;
}