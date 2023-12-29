
/*
	calculator08buggy.cpp

	Helpful comments removed.

	We have inserted 3 bugs that the compiler will catch and 3 that it won't.

	Grammar:
		Calculation:
			Statement
			Print
			Quit
			Calculation Statement
		Statement:
			Assignment
			Declaration
			Expression
		Assignment:
			Name "=" Expression
		Declaration:
			"let" Name "=" Expression
		Expression:
			Term
			Expression "+" Term
			Expression "-" Term
		Term:
			Primary
			Term "*" Primary
			Term "/" Primary
			Term "%" Primary
		Primary:
			Number
			Variable
			"(" Expression ")"
			"sqrt(" Expression ")"
			"pow(" Expression "," Expression ")"
		Number:
			Floating-Point Value
		Variable:
			Name
*/

#include "std_lib_facilities.h"
#include <math.h>

class Token {
public:
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string s) :kind(ch), name(s) { }
};

class Token_stream {
	bool full;
	Token buffer;
public:
	Token_stream() :full(0), buffer(0) { }

	Token get();
	void unget(Token t) { buffer = t; full = true; }

	void ignore(char);
};

const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char square_root = '?';
const char power = '&';
const char constant = 'C';

Token Token_stream::get()
{
	if (full) { full = false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '=':
	case ',':
		return Token(ch);
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{	cin.unget();
	double val;
	cin >> val;
	return Token(number, val);
	}
	default:
		if (isalpha(ch)) { // isalpha(ch) returns true if ch is a character
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch =='_')) s += ch;
			cin.unget();
			if (s == "let") return Token(let);
			if (s == "quit") return Token(name);
			if (s == "sqrt") return Token(square_root);
			if (s == "pow") return Token(power);
			if (s == "const") return Token(constant);
			return Token(name, s);
		}
		error("Bad token");
	}
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch;
	while (cin >> ch)
		if (ch == c) return;
}

class Variable {
public:
	string name;
	double value;
	bool constant;
	Variable(string n, double v, bool c = false) :name(n), value(v), constant(c) { }
};

vector<Variable> names;

bool is_declared(string s);

void define_name(string name, double value, bool constant = false)
// function to define a variable before running the code
{
	if (is_declared(name)) error("define_name(): existing variable");
	names.push_back(Variable(name, value, constant));
}

double get_value(string s)
// return value of an existing variable
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return names[i].value;
	error("get: undefined name ", s);
}

void set_value(string s, double d)
// change value of an existing variable
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) {
			names[i].value = d;
			return;
		}
	error("set: undefined name ", s);
}

bool is_declared(string s)
// return true if a variable is declared
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}

bool is_constant(string s)
// return true if a variable is declared an it is a constant
{
	if (is_declared(s)) {
		for (int i = 0; i < names.size(); ++i) {
			if (names[i].name == s) return names[i].constant;
		}
		return false;
	}
	error("is_constant: undefined name");
}

Token_stream ts;

double expression();
double assignment(string s);

double power_get() {
	double val1;
	int val2;
	Token t = ts.get();
	if (t.kind == '(') {
		val1 = expression();
		t = ts.get();
		if (t.kind == ',') {
			val2 = narrow_cast<int>(expression());
			t = ts.get();	
			if (t.kind == ')') return pow(val1, val2); // ignore the ")" at the end of function call "pow(val1, val2);"
			else ts.unget(t);
		}
	}
	error("pow(val1, val2): Bad Input");
}

double primary()
{
	Token t = ts.get();
	switch (t.kind) {
		case '(':
		{
			double d = expression();
			t = ts.get();
			if (t.kind != ')') error("'(' expected");
			return d;
		}
		case '-':
			return -primary();
		case number:
			return t.value;
		case name:
		{
			Token t2 = ts.get();
			if (t2.kind == '=') {
				return assignment(t.name);
			}
			ts.unget(t2);
			return get_value(t.name);
		}
		case square_root:
		{
			double d = primary();
			if (d < 0) error("sqrt(n): n should be a positive number");
			return sqrt(d);
		}
		case power:
		{
			return power_get();
		}
		default:
			error("primary expected");
	}
}

double term()
{
	double left = primary();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '*':
			left *= primary();
			break;
		case '/': {
			double d = primary();
			if (d == 0) error("divide by zero");
			left /= d;
			break;
		}
		default:
			ts.unget(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		default:
			ts.unget(t);
			return left;
		}
	}
}

double declaration(bool c = false)
{
	Token t = ts.get();
	if (t.kind != 'a') error("name expected in declaration");
	string name = t.name;
	if (is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ", name);
	double d = expression();
	names.push_back(Variable(name, d, c));
	return d;
}

double assignment(string name) {
	if (is_constant(name)) error("assignment: const variable");
	double d = expression();
	set_value(name, d);
	return d;
}

double statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration();
	case constant:
		return declaration(true);
	default:
		ts.unget(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{
	while (true) try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();
		if (t.kind == quit) return;
		ts.unget(t);
		cout << result << statement() << endl;
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()

try {
	define_name("pi", 3.14159, true);
	cout << names[0].name << "\t" << names[0].constant << "\n";
	calculate();
	return 0;
}
catch (exception& e) {
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != ';');
	return 1;
}
catch (...) {
	cerr << "exception\n";
	char c;
	while (cin >> c && c != ';');
	return 2;
}
