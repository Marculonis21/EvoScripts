#pragma once

#include "lpu_addons.hpp"
#include <fstream>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

class ESParses {
  public:
	static std::vector<Instr> parseFile(const std::string &filename) {
		auto stringCommands = parseCommands(filename);
		std::vector<Instr> instrCommands;

		for (int line = 0; line < stringCommands.size(); ++line) {
			auto comm = stringCommands[line];
			if (!stringToInstrMap.contains(comm)) {
				throw std::invalid_argument(
					"In file: " + filename + " - invalid command " + comm +
					" at line " + std::to_string(line + 1));
			}

			instrCommands.push_back(stringToInstrMap.at(comm));
		}

		return instrCommands;
	}

  private:
	static std::vector<std::string> parseCommands(const std::string &filename) {
		std::vector<std::string> resultVector;

		std::ifstream file(filename);

		std::regex del("//");

		if (file.is_open()) {
			std::string line;
			while (std::getline(file, line)) {
				line = trim(line);
				if (!line.starts_with("//")) {
					std::sregex_token_iterator partIterator(
						line.begin(), line.end(), del, -1);
					std::string firstToken = *partIterator;
					firstToken = trim(firstToken);

					if (firstToken != "") {
						resultVector.emplace_back(firstToken);
					}
				}
			}

			file.close();
		} else {
			throw std::invalid_argument("Problem opening a file " + filename);
		}

		return resultVector;
	}

	std::string static trim(std::string &str) {
		int i = 0;

		// left trim
		while (isspace(str[i]) != 0)
			i++;
		str = str.substr(i, str.length() - i);

		// right trim
		i = str.length() - 1;
		while (isspace(str[i]) != 0)
			i--;
		str = str.substr(0, i + 1);

		return str;
	}
};
