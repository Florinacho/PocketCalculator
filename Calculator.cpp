#include <string.h>
#include <stdarg.h>
#include <math.h>

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

/*****************************************************************************/
/* Calculator functions.                                                     */
/*****************************************************************************/
bool Calculator::getToken(Token& token) {
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
	case ',' :
		token.type = Token::COMMA;
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
	case '=' :
		switch (input[inputIndex + 1]) {
		case '=' :
			token.type = Token::OPERATOR_EQUAL;
			token.identifier[0] = input[inputIndex];
			token.identifier[1] = input[inputIndex + 1];
			inputIndex += 2;
			break;
		default :
			token.type = Token::OPERATOR_ASSIGN;
			token.identifier[0] = input[inputIndex];
			++inputIndex;
			break;
		}
		return true;
	case '!' :
		switch (input[inputIndex + 1]) {
		case '=' :
			token.type = Token::OPERATOR_NOT_EQUAL;
			token.identifier[0] = input[inputIndex];
			token.identifier[1] = input[inputIndex + 1];
			inputIndex += 2;
			break;
		default :
			token.type = Token::OPERATOR_NOT;
			token.identifier[0] = input[inputIndex];
			++inputIndex;
			break;
		}
		return true;
	case '<' :
		switch (input[inputIndex + 1]) {
		case '=' :
			token.type = Token::OPERATOR_LEQ;
			token.identifier[0] = input[inputIndex];
			token.identifier[1] = input[inputIndex + 1];
			inputIndex += 2;
			break;
		default :
			token.type = Token::OPERATOR_LTN;
			token.identifier[0] = input[inputIndex];
			++inputIndex;
			break;
		}
		return true;
	case '>' :
		switch (input[inputIndex + 1]) {
		case '=' :
			token.type = Token::OPERATOR_GEQ;
			token.identifier[0] = input[inputIndex];
			token.identifier[1] = input[inputIndex + 1];
			inputIndex += 2;
			break;
		default :
			token.type = Token::OPERATOR_GTN;
			token.identifier[0] = input[inputIndex];
			++inputIndex;
			break;
		}
		return true;
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
					error("Too many decimal points in number.");
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

	return true;
}

bool Calculator::nextToken() {
	token[0] = token[1];
	if (getToken(token[1]) == false) {
		return false;
	}
	return (token[0].type != Token::END_OF_STREAM);
}

bool Calculator::match(unsigned int tokenType) {
	if (token[0].type != tokenType) {
		error("Syntax error! Expected token type %d before '%s'.", tokenType, token[0].identifier);
		return false;
	}
	nextToken();
	return true;
}

bool Calculator::matchNumber(double* output) {
	if (token[0].type != Token::NUMBER) {
		error("Syntax error! Expected number before '%s'.", token[0].identifier);
		return false;
	}
	*output = token[0].numberValue;
	nextToken();
	return true;
}

bool Calculator::matchIdentifier(double* output) {
	if (token[0].type != Token::IDENTIFIER) {
		error("Syntax error! Expected identifier before '%s'.", token[0].identifier);
		return false;
	}
	if (getVariable(token[0].identifier, output) == false) {
		error("Syntax error! Variable \"%s\" was not defined.", token[0].identifier);
		return false;
	}
	nextToken();
	return true;
}

bool Calculator::matchPrimaryExpression(double* output) {
	switch (token[0].type) {
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
		if (strcmp(token[0].identifier, "sqrt") == 0) {
			nextToken();
			if (match(Token::LEFT_PARENTHESIS) == false) {
				return false;
			}
			if (matchExpression(output) == false) {
				return false;
			}
			if (match(Token::RIGHT_PARENTHESIS) == false) {
				return false;
			}
			*output = sqrt(*output);
			return true;
		} else if (strcmp(token[0].identifier, "min") == 0) {
			double min = 0.0;
			nextToken();
			if (match(Token::LEFT_PARENTHESIS) == false) {
				return false;
			}
			if (matchExpression(output) == false) {
				return false;
			}
			min = *output;
			while (token[0].type == Token::COMMA) {
				nextToken();
				if (matchExpression(output) == false) {
					return false;
				}
				if (*output < min) {
					min = *output;
				}
			}
			if (match(Token::RIGHT_PARENTHESIS) == false) {
				return false;
			}
			*output = min;
			return true;
		} else if (strcmp(token[0].identifier, "max") == 0) {
			double max = 0.0;
			nextToken();
			if (match(Token::LEFT_PARENTHESIS) == false) {
				return false;
			}
			if (matchExpression(output) == false) {
				return false;
			}
			max = *output;
			while (token[0].type == Token::COMMA) {
				nextToken();
				if (matchExpression(output) == false) {
					return false;
				}
				if (*output > max) {
					max = *output;
				}
			}
			if (match(Token::RIGHT_PARENTHESIS) == false) {
				return false;
			}
			*output = max;
			return true;
		} else{
			return matchIdentifier(output);
		}
		break;

	default :
		error("Syntax error! Expected primary expression before '%s'.", token[0].identifier);
	}
	return false;
}

bool Calculator::matchUnaryExpression(double* output) {
	switch (token[0].type) {
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

	switch (token[0].type) {
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

	switch (token[0].type) {
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

bool Calculator::matchEqualExpressionTail(double* lvalue) {
	double rvalue = 0.0;

	switch (token[0].type) {
	case Token::OPERATOR_EQUAL :
		nextToken();
		if (matchAddExpression(&rvalue) == false) {
			return false;
		}
		*lvalue = (*lvalue == rvalue);
		return matchAddExpressionTail(lvalue);
	case Token::OPERATOR_NOT_EQUAL :
		nextToken();
		if (matchAddExpression(&rvalue) == false) {
			return false;
		}
		*lvalue = (*lvalue != rvalue);
		return matchAddExpressionTail(lvalue);
	}
	return true;
}

bool Calculator::matchEqualExpression(double* output) {
	if (matchAddExpression(output) == false) {
		return false;
	}
	if (matchEqualExpressionTail(output) == false) {
		return false;
	}
	return true;
}

bool Calculator::matchAssignExpression(double* output) {
	if (token[0].type == Token::IDENTIFIER 
		&& token[1].type == Token::OPERATOR_ASSIGN) {
		const std::string identifier = token[0].identifier;
		nextToken();
		nextToken();
		if (matchExpression(output) == false) {
			return false;
		}
		if (setVariable(identifier.c_str(), *output, false) == false) {
			error("Cannot update constant '%s'.", identifier.c_str());
			return false;
		}
		return true;
	}

	return matchEqualExpression(output);
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

bool Calculator::parse(double* output, const char* ninput) {
	input = ninput;
	inputIndex = 0;

	*output = 0.0;
	nextToken();
	nextToken();
	if (matchExpression(output) == false) {
		return false;
	}
	if (token[0].type != Token::END_OF_STREAM) {
		error("Syntax error! Unknown token '%s'.", token[0].identifier);
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

bool Calculator::getVariable(const char* identifier, double* output, bool* constant) const {
	for (unsigned int index = 0; index < variableList.size(); ++index) {
		if (variableList[index].identifier == identifier) {
			if (output != NULL) {
				*output = variableList[index].getValue();
			}
			if (constant != NULL) {
				*constant = variableList[index].constant;
			}
			return true;
		}
	}

	return false;
}

unsigned int Calculator::getVariableCount() const {
	return variableList.size();
}

bool Calculator::getVariable(const unsigned int index, std::string* identifier, double* output, bool* constant) const {
	if (index > variableList.size()) {
		return false;
	}

	if (identifier != NULL) {
		*identifier = variableList[index].identifier;
	}

	if (output != NULL) {
		*output = variableList[index].getValue();
	}

	if (constant != NULL) {
		*constant = variableList[index].constant;
	}

	return true;
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

void Calculator::removeAllVariables() {
	variableList.clear();
}
