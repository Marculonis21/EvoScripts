#ifndef ESPARSER_HPP
#define ESPARSER_HPP

#include "lpu_addons.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <regex>

class ESParses {
  public:
	static std::vector<Instr> parseFile(const std::string &filename) {

		std::vector<Instr> resultVector;
		std::ifstream file(filename);

        std::regex del("//");

		if (file.is_open()) {
			std::string line;
			while (std::getline(file, line)) {
				if (line.starts_with("//")) {
					printf("Comment: %s\n", line.c_str());
				}
				else {
					std::sregex_token_iterator partIterator(line.begin(), line.end(), del, -1);
					std::sregex_token_iterator end;

					printf("Something: ");
					while (partIterator != end) {
						std::string part = *partIterator;
						printf("part \"%s\", ", part.c_str());
						++partIterator;
					}
					printf("\n");
				}

                printf("For input: %s\n\n", line.c_str());
			}

			file.close();
		} else {
			std::cout << "Problem opening a file - " << filename << std::endl;
		}

		return resultVector;
	}
};

#endif
