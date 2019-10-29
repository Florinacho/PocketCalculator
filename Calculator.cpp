#include <string.h>

#include "Calculator.hpp"

/*****************************************************************************/
/* Utility functions.                                                        */
/*****************************************************************************/
bool IsEndOfStream(const char c) {
	return (c == '\0');
}

bool IsSpace(const char c) {
	switch (c) {
	case ' ' :
	case '\n' :
	case '\t' :
		return true;
	}
	return false;
}

bool IsSeparator(const char c) {
	switch (c) {
	case '(' :
	case ')' :
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

bool IsOperator(const char c) {
	switch (c) {
	case '+' :
	case '-' :
	case '*' :
	case '/' :
	case '=' :
		return true;
	}
	return false;
}

void SkipSpace(char** input) {
	while (IsSpace(**input)) {
		++(*input);
	}
}


bool GetToken(Token& token, char** input) {
	SkipSpace(input);

	if (IsEndOfStream(**input)) {
		token.type = Token::END_OF_STREAM;
		return false;
	}

	if (IsSeparator(**input)) {
		switch (**input) {
		case '(' :
			token.type = Token::LEFT_PARENTHESIS;
			++(*input);
			return true;
		case ')' :
			token.type = Token::RIGHT_PARENTHESIS;
			++(*input);
			return true;
		}
	}

	if (IsOperator(**input)) {
		switch (**input) {
		case '+' :
			token.type = Token::OPERATOR_ADD;
			++(*input);
			return true;
		case '-' :
			token.type = Token::OPERATOR_SUB;
			++(*input);
			return true;
		case '*' :
			token.type = Token::OPERATOR_MUL;
			++(*input);
			return true;
		case '/' :
			token.type = Token::OPERATOR_DIV;
			++(*input);
			return true;
		case '=' :
			token.type = Token::OPERATOR_ASSIGN;
			++(*input);
			return true;
		}
	}

	if (IsNum(**input) || (**input == '.')) {
		token.type = Token::NUMBER;

		double value = 0.0;
		double div = 1.0;
		bool decimalPoint = false;
		token.numberValue = 0.0;

		while(IsNum(**input) || (**input == '.')) {
			char c = **input;
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

			++(*input);
		}

		return true;
	}

	if (IsAlpha(**input)) {
		token.type = Token::IDENTIFIER;
		memset(token.identifier, 0, sizeof(token.identifier));
		int length = 0;
		do {
			token.identifier[length] = **input;
			++length;
			++*(input);
		} while (IsAlnum(**input) && length < sizeof(token.identifier));
		return true;
	}

	return false;
}

/*****************************************************************************/
/* Calculator functions.                                                     */
/*****************************************************************************/
bool Calculator::nextToken(char** input) {
	GetToken(token, input);
	return (token.type != Token::END_OF_STREAM);
}

bool Calculator::match(unsigned int tokenType, char** input) {
	if (token.type != tokenType) {
		printf("Syntax error! Expected token type %d.\n", tokenType);
		return false;
	}
	nextToken(input);
	return true;
}

bool Calculator::matchNumber(double* output, char** input) {
	if (token.type != Token::NUMBER) {
		printf("Syntax error! Expected number at: \"%s\"\n", *input);
		return false;
	}
	*output = token.numberValue;
	nextToken(input);
	return true;
}

bool Calculator::matchIdentifier(double* output, char** input) {
	if (token.type != Token::IDENTIFIER) {
		printf("Syntax error! Expected identifier at: \"%s\"\n", *input);
		return false;
	}
	if (getVariable(token.identifier, output) == false) {
		printf("Syntax error! Variable \"%s\" was not defined.\n", token.identifier);
		return false;
	}
	nextToken(input);
	return true;
}

bool Calculator::matchPrimaryExpression(double* output, char** input) {
	switch (token.type) {
	case Token::LEFT_PARENTHESIS :
		nextToken(input);

		if (matchExpression(output, input) == false) {
			return false;
		}

		if (match(Token::RIGHT_PARENTHESIS, input) == false) {
			return false;
		}

		return true;

	case Token::NUMBER :
		return matchNumber(output, input);

	case Token::IDENTIFIER :
		return matchIdentifier(output, input);

	default :
		printf("Syntax error! Expected primary expression.\n");
	}
	return false;
}

bool Calculator::matchUnaryExpression(double* output, char** input) {
	switch (token.type) {
	case Token::OPERATOR_SUB :
		nextToken(input);
		if (matchUnaryExpression(output, input) == false) {
			return false;
		}
		*output = -*output;
		return true;
	}

	return matchPrimaryExpression(output, input);
}

bool Calculator::matchMulExpressionTail(double* lvalue, char** input) {
	double rvalue = 0.0;

	switch (token.type) {
	case Token::OPERATOR_MUL :
		nextToken(input);
		if (matchUnaryExpression(&rvalue, input) == false) {
			return false;
		}
		*lvalue *= rvalue;
		return matchMulExpressionTail(lvalue, input);

	case Token::OPERATOR_DIV :
		nextToken(input);
		if (matchUnaryExpression(&rvalue, input) == false) {
			return false;
		}
		*lvalue /= rvalue;
		return matchMulExpressionTail(lvalue, input);
	}
	return true;
}

bool Calculator::matchMulExpression(double* output, char**input) {
	if (matchUnaryExpression(output, input) == false) {
		return false;
	}
	if (matchMulExpressionTail(output, input) == false) {
		return false;
	}
	return true;
}

bool Calculator::matchAddExpressionTail(double* lvalue, char** input) {
	double rvalue = 0.0;

	switch (token.type) {
	case Token::OPERATOR_ADD :
		nextToken(input);
		if (matchMulExpression(&rvalue, input) == false) {
			return false;
		}
		*lvalue += rvalue;
		return matchAddExpressionTail(lvalue, input);
	case Token::OPERATOR_SUB :
		nextToken(input);
		if (matchMulExpression(&rvalue, input) == false) {
			return false;
		}
		*lvalue -= rvalue;
		return matchAddExpressionTail(lvalue, input);
	}
	return true;
}

bool Calculator::matchAddExpression(double* output, char**input) {
	if (matchMulExpression(output, input) == false) {
		return false;
	}
	if (matchAddExpressionTail(output, input) == false) {
		return false;
	}
	return true;
}


bool Calculator::matchAssignExpression(double* output, char**input) {
	// TODO: implement assignment
	std::string identifier;
	double valueBackup = 0.0;
	bool exists = false;
	bool constant = false;

	switch (token.type) {
	case Token::IDENTIFIER :
		identifier = token.identifier;
		exists = getVariable(identifier.c_str(), &valueBackup);
		nextToken(input);
		if (token.type == Token::OPERATOR_ASSIGN) {
			nextToken(input);
			if (matchExpression(output, input) == false) {
				return false;
			}
			if (setVariable(identifier.c_str(), *output, constant) == false) {
				printf("Error! Cannot assign new value to a constant.\n");
				return false;
			}
		} else {
			if (exists) {
				*output = valueBackup;
				if (matchAddExpressionTail(output, input) == false) {
					return false;
				}
			} else {
				printf("Syntax error! Variable \"%s\" was not defined.\n", identifier.c_str());
				return false;
			}
		}
		return true;
	default :
		return matchAddExpression(output, input);
	}
	return false;
}

bool Calculator::matchExpression(double* output, char** input) {
	return matchAssignExpression(output, input);
}

bool Calculator::calculate(double* output, char** input) {
	*output = 0.0;
	nextToken(input);
	if (matchExpression(output, input) == false) {
		return false;
	}
	if (token.type != Token::END_OF_STREAM) {
		printf("Syntax error! Unknown token '%c'.\n", **input);
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
