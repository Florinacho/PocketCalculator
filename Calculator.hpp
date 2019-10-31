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
		COMMA,

		NUMBER,
		IDENTIFIER,

		OPERATOR_ASSIGN,
		OPERATOR_EQUAL,
		OPERATOR_LEQ,
		OPERATOR_LTN,
		OPERATOR_GEQ,
		OPERATOR_GTN,
		OPERATOR_NOT_EQUAL,
		OPERATOR_ADD,
		OPERATOR_SUB,
		OPERATOR_MUL,
		OPERATOR_DIV,
		OPERATOR_NOT
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

	Token token[2];

	bool getToken(Token& token);

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

	bool matchCompExpressionTail(double* output);

	bool matchCompExpression(double* output);

	bool matchEqualExpressionTail(double* output);

	bool matchEqualExpression(double* output);

	bool matchAssignExpression(double* output);

	bool matchExpression(double* output);

	void error(const char* format, ...);
public:

	bool parse(double* output, const char* input);

	bool setVariable(const char* identifier, double value, bool constant = false);

	unsigned int getVariableCount() const;

	bool getVariable(const char* identifier, double* output = NULL, bool* constant = NULL) const;

	bool getVariable(const unsigned int index, std::string* identifier = NULL, double* output = NULL, bool* constant = NULL) const;

	bool removeVariable(const char* identifier);

	void removeAllVariables();
};

#endif //__CALCULATOR_HPP__
