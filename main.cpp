#include <stdio.h>
#include <string.h>

#include "Calculator.hpp"

int main() {
	double output = 0.0;
	char buffer[512];
	char* input;
	Calculator calculator;

	while (true) {
		printf("Input: ");
		memset(buffer, 0, sizeof(buffer));
		scanf("%s", buffer);
		input = buffer;

		if ((strcmp(buffer, "quit") == 0)) {
			return 0;
		}

		calculator.setVariable("PI", 3.141592653589793, true);
		// calculator.setFunction("MIN, 2, &CMinFunction);
		// calculator.setFunction("SQRT", 1, &CSqrtFunction);
		if (calculator.calculate(&output, &input)) {
			printf("Output: %f\n\n", output);
		}
	}
	return 0;
}
