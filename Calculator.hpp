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

		OPERATOR_COMMA,
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

	// Number, Identifier
	bool matchPrimaryExpression(double* output);

	// Operators: -
	bool matchUnaryExpression(double* output);

	// Operators: *, /
	bool matchMulExpressionTail(double* lvalue);

	bool matchMulExpression(double* output);

	// Operators: +, -
	bool matchAddExpressionTail(double* lvalue);

	bool matchAddExpression(double* output);

	// Operators: <, <=, >, >=
	bool matchRelExpressionTail(double* output);

	bool matchRelExpression(double* output);

	// Operators: ==, !=
	bool matchEqualExpressionTail(double* output);

	bool matchEqualExpression(double* output);

	// Operators: =
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
