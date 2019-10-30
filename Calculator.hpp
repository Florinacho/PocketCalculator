#ifndef __CALCULATOR_HPP__
#define __CALCULATOR_HPP__

#include <string>
#include <vector>

struct Token {
	enum TokenType {
		END_OF_STREAM,

		UNKNOWN,

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
	char identifier[32];

	Token() {
		type = END_OF_STREAM;
		numberValue = 0.0;
	}
};

class Calculator {
	const char* input;
	int inputIndex;

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

	bool nextToken();

	bool match(unsigned int tokenType);

	bool matchNumber(double* output);

	bool matchIdentifier(double* output);

	bool matchPrimaryExpression(double* output);

	bool matchUnaryExpression(double* output);

	bool matchMulExpressionTail(double* lvalue);

	bool matchMulExpression(double* output);

	bool matchAddExpressionTail(double* lvalue);

	bool matchAddExpression(double* output);

	bool matchAssignExpression(double* output);

	bool matchExpression(double* output);

	void error(const char* format, ...);

public:
	bool calculate(double* output, const char* input);

	bool setVariable(const char* identifier, double value, bool constant = false);

	bool getVariable(const char* identifier, double* output = NULL) const;

	bool removeVariable(const char* identifier);

	void removeVariableAll();
};

#endif //__CALCULATOR_HPP__
