#include <string.h>
#include <stdarg.h>

#include "Calculator.hpp"

/*****************************************************************************/
/* Utility functions.                                                        */
/*****************************************************************************/

bool IsSpace(const char c) {
	switch (c) {
	case ' ' :
	case '\n' :
	case '\t' :
		return true;
	}
	return false;
}

bool IsNum(const char c) {
	switch (c) {
	case '0' :
	case '1' :
	case '2' :
	case '3' :
	case '4' :
	case '5' :
	case '6' :
	case '7' :
	case '8' :
	case '9' :
		return true;
	}
	return false;
}

bool IsAlpha(const char c) {
	return ((c >= 'a' && c <= 'z') 
		|| (c >= 'A' && c <= 'Z'));
}

bool IsAlnum(const char c) {
	return ((c >= 'a' && c <= 'z') 
		|| (c >= 'A' && c <= 'Z')
		|| (c >= '0' && c <= '9'));
}

bool GetToken(Token& token, const char* input, int& inputIndex) {
	token.type = Token::UNKNOWN;
	token.numberValue = 0.0;
	memset(token.identifier, 0, sizeof(token.identifier));

	while (IsSpace(input[inputIndex])) {
		++inputIndex;
	}

	switch (input[inputIndex]) {
	case '\0' :
		token.type = Token::END_OF_STREAM;
		sprintf(token.identifier, "end of stream");
		return false;
	case '(' :
		token.type = Token::LEFT_PARENTHESIS;
		token.identifier[0] = input[inputIndex];
		++inputIndex;
		return true;
	case ')' :
		token.type = Token::RIGHT_PARENTHESIS;
		token.identifier[0] = input[inputIndex];
		++inputIndex;
		return true;
	case '+' :
		token.type = Token::OPERATOR_ADD;
		token.identifier[0] = input[inputIndex];
		++inputIndex;
		return true;
	case '-' :
		token.type = Token::OPERATOR_SUB;
		token.identifier[0] = input[inputIndex];
		++inputIndex;
		return true;
	case '*' :
		token.type = Token::OPERATOR_MUL;
		token.identifier[0] = input[inputIndex];
		++inputIndex;
		return true;
	case '/' :
		token.type = Token::OPERATOR_DIV;
		token.identifier[0] = input[inputIndex];
		++inputIndex;
		return true;
//	case '=' :
//		token.type = Token::OPERATOR_ASSIGN;
//		token.identifier[0] = input[inputIndex];
//		++inputIndex;
//		return true;
	}

	if (IsNum(input[inputIndex]) || (input[inputIndex] == '.')) {
		token.type = Token::NUMBER;

		double value = 0.0;
		double div = 1.0;
		bool decimalPoint = false;
		token.numberValue = 0.0;
		int length = 0;

		while(IsNum(input[inputIndex]) || (input[inputIndex] == '.')) {
			char c = input[inputIndex];
			if (c == '.') {
				if (decimalPoint == true) {
					return false;
				} else {
					decimalPoint = true;
				}
			} else {
				if (decimalPoint) {
					div /= 10.0;
					token.numberValue += ((double)(c - '0')) * div;
				} else {
					token.numberValue *= 10.0;
					token.numberValue += c - '0';
				}
			}
			token.identifier[length++] = input[inputIndex];
			++inputIndex;
		}

		return true;
	}

	if (IsAlpha(input[inputIndex])) {
		token.type = Token::IDENTIFIER;
		memset(token.identifier, 0, sizeof(token.identifier));
		int length = 0;
		do {
			token.identifier[length] = input[inputIndex];
			++length;
			++inputIndex;
		} while (IsAlnum(input[inputIndex]) && length < sizeof(token.identifier));
		return true;
	}

	token.identifier[0] = input[inputIndex];

	return false;
}

/*****************************************************************************/
/* Calculator functions.                                                     */
/*****************************************************************************/
bool Calculator::nextToken() {
	GetToken(token, input, inputIndex);
	return (token.type != Token::END_OF_STREAM);
}

bool Calculator::match(unsigned int tokenType) {
	if (token.type != tokenType) {
		error("Syntax error! Expected token type %d before '%s'.", tokenType, token.identifier);
		return false;
	}
	nextToken();
	return true;
}

bool Calculator::matchNumber(double* output) {
	if (token.type != Token::NUMBER) {
		error("Syntax error! Expected number before '%s'.", token.identifier);
		return false;
	}
	*output = token.numberValue;
	nextToken();
	return true;
}

bool Calculator::matchIdentifier(double* output) {
	if (token.type != Token::IDENTIFIER) {
		error("Syntax error! Expected identifier before '%s'.", token.identifier);
		return false;
	}
	if (getVariable(token.identifier, output) == false) {
		error("Syntax error! Variable \"%s\" was not defined.", token.identifier);
		return false;
	}
	nextToken();
	return true;
}

bool Calculator::matchPrimaryExpression(double* output) {
	switch (token.type) {
	case Token::LEFT_PARENTHESIS :
		nextToken();

		if (matchExpression(output) == false) {
			return false;
		}

		if (match(Token::RIGHT_PARENTHESIS) == false) {
			return false;
		}

		return true;

	case Token::NUMBER :
		return matchNumber(output);

	case Token::IDENTIFIER :
		return matchIdentifier(output);

	default :
		error("Syntax error! Expected primary expression before '%s'.", token.identifier);
	}
	return false;
}

bool Calculator::matchUnaryExpression(double* output) {
	switch (token.type) {
	case Token::OPERATOR_SUB :
		nextToken();
		if (matchUnaryExpression(output) == false) {
			return false;
		}
		*output = -*output;
		return true;
	}

	return matchPrimaryExpression(output);
}

bool Calculator::matchMulExpressionTail(double* lvalue) {
	double rvalue = 0.0;

	switch (token.type) {
	case Token::OPERATOR_MUL :
		nextToken();
		if (matchUnaryExpression(&rvalue) == false) {
			return false;
		}
		*lvalue *= rvalue;
		return matchMulExpressionTail(lvalue);

	case Token::OPERATOR_DIV :
		nextToken();
		if (matchUnaryExpression(&rvalue) == false) {
			return false;
		}
		*lvalue /= rvalue;
		return matchMulExpressionTail(lvalue);
	}
	return true;
}

bool Calculator::matchMulExpression(double* output) {
	if (matchUnaryExpression(output) == false) {
		return false;
	}
	if (matchMulExpressionTail(output) == false) {
		return false;
	}
	return true;
}

bool Calculator::matchAddExpressionTail(double* lvalue) {
	double rvalue = 0.0;

	switch (token.type) {
	case Token::OPERATOR_ADD :
		nextToken();
		if (matchMulExpression(&rvalue) == false) {
			return false;
		}
		*lvalue += rvalue;
		return matchAddExpressionTail(lvalue);
	case Token::OPERATOR_SUB :
		nextToken();
		if (matchMulExpression(&rvalue) == false) {
			return false;
		}
		*lvalue -= rvalue;
		return matchAddExpressionTail(lvalue);
	}
	return true;
}

bool Calculator::matchAddExpression(double* output) {
	if (matchMulExpression(output) == false) {
		return false;
	}
	if (matchAddExpressionTail(output) == false) {
		return false;
	}
	return true;
}


bool Calculator::matchAssignExpression(double* output) {
	// TODO: implement assignment
	return matchAddExpression(output);
}

bool Calculator::matchExpression(double* output) {
	return matchAssignExpression(output);
}

void Calculator::error(const char* format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	printf("\n%s\n", input);
	if (inputIndex > 0) {
		for (unsigned int index = 0; index < inputIndex - 1; ++index) {
			printf(" ");
		}
	}
	printf("^\n");
	va_end(args);
}

bool Calculator::calculate(double* output, const char* ninput) {
	input = ninput;
	inputIndex = 0;

	*output = 0.0;
	nextToken();
	if (matchExpression(output) == false) {
		return false;
	}
	if (token.type != Token::END_OF_STREAM) {
		error("Syntax error! Unknown token '%c'.", input[inputIndex]);
		return false;
	}
	return true;
}

bool Calculator::setVariable(const char* identifier, double value, bool constant) {
	for (unsigned int index = 0; index < variableList.size(); ++index) {
		if (variableList[index].identifier == identifier) {
			return variableList[index].setValue(value);
		}
	}

	variableList.push_back(Variable(identifier, value, constant));
	return true;
}

bool Calculator::getVariable(const char* identifier, double* output) const {
	for (unsigned int index = 0; index < variableList.size(); ++index) {
		if (variableList[index].identifier == identifier) {
			if (output != NULL) {
				*output = variableList[index].getValue();
			}
			return true;
		}
	}

	return false;
}

bool Calculator::removeVariable(const char* identifier) {
	for (unsigned int index = 0; index < variableList.size(); ++index) {
		if (variableList[index].identifier == identifier) {
			variableList.erase(variableList.begin() + index);
			return true;
		}
	}
	return false;
}

void Calculator::removeVariableAll() {
	variableList.clear();
}
