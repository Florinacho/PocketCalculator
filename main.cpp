#include <stdio.h>
#include <iostream>

#include "Calculator.hpp"

int main() {
	double output = 0.0;
	std::string input;
	Calculator calculator;

	calculator.setVariable("PI", 3.141592653589793, true);

	while (true) {
		printf("Input:");
		getline(std::cin, input);

		if (input == "quit") {
			return 0;
		} else if (input == "list") {
			const unsigned int variableCount = calculator.getVariableCount();
			std::string identifier;
			double value;
			bool constant;
			for (unsigned int index = 0; index < variableCount; ++index) {
				calculator.getVariable(index, &identifier, &value, &constant);
				printf("%s %s = %f\n", constant ? "constant" : "variable", identifier.c_str(), value);
			}
		} else if (input == "") {
		}else {
			if (calculator.parse(&output, input.c_str())) {
				printf("Output: %f\n\n", output);
			}
		}
	}
	return 0;
}
