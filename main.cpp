#include <stdio.h>
#include <string.h>
#include <iostream>

#include "Calculator.hpp"

int main() {
	double output = 0.0;
	std::string line;
	char command[16];
	Calculator calculator;

	calculator.setVariable("PI", 3.141592653589793, true);

	while (true) {
		printf("Input:");
		getline(std::cin, line);

		if (sscanf(line.c_str(), "%s", command) == 1) {
			if (strcmp(command, "quit") == 0) {
				return 0;
			}
			if (strcmp(command, "rem") == 0) {
				if (sscanf(line.c_str(), "rem %s", command) == 1) {
					calculator.removeVariable(command);
					continue;
				}
			}
			if (strcmp(command, "set") == 0) {
				double value = 0.0;
				if (sscanf(line.c_str(), "set %s %lf", command, &value) == 2) {
					calculator.setVariable(command, value, false);
					continue;
				}
			}
		}
		if (line == "quit") {
			return 0;
		}

		if (calculator.calculate(&output, line.c_str())) {
			printf("Output: %f\n\n", output);
		}
	}
	return 0;
}
