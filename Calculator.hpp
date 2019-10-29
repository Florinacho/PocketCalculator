#ifndef __CALCULATOR_HPP__
#define __CALCULATOR_HPP__

#include <string>
#include <vector>

struct Token {
	enum TokenType {
		END_OF_STREAM,

		LEFT_PARENTHESIS,
		RIGHT_PARENTHESIS,

		NUMBER,
		IDENTIFIER,

		OPERATOR_ASSIGN,
		OPERATOR_ADD,
		OPERATOR_SUB,
		OPERATOR_MUL,
		OPERATOR_DIV
	};

	TokenType type;
	double numberValue;
	char identifier[16];

	Token() {
		type = END_OF_STREAM;
		numberValue = 0.0;
	}
};

class Calculator {
	struct Variable {
		std::string identifier;
		double value;
		bool constant;

		Variable(const char* nidentifier, double nvalue, bool nconstant)
			: identifier(nidentifier), value(nvalue), constant(nconstant) {
		}

		bool setValue(double newValue) {
			if (constant == true) {
				return false;
			}
			value = newValue;
			return true;
		}

		double getValue() const {
			return value;
		}
	};
	std::vector<Variable> variableList;

	Token token;

	bool nextToken(char** input);

	bool match(unsigned int tokenType, char** input);

	bool matchNumber(double* output, char** input);

	bool matchIdentifier(double* output, char** input);

	bool matchPrimaryExpression(double* output, char** input);

	bool matchUnaryExpression(double* output, char** input);

	bool matchMulExpressionTail(double* lvalue, char** input);

	bool matchMulExpression(double* output, char**input);

	bool matchAddExpressionTail(double* lvalue, char** input);

	bool matchAddExpression(double* output, char**input);

	bool matchAssignExpression(double* output, char**input);

	bool matchExpression(double* output, char** input);

public:
	bool calculate(double* output, char** input);

	bool setVariable(const char* identifier, double value, bool constant = false);

	bool getVariable(const char* identifier, double* output = NULL) const;
};

#endif //__CALCULATOR_HPP__
