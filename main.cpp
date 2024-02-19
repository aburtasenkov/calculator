/*
	calculator08buggy.cpp

	Grammar:
		Calculation:
			Statement
			Print
			Quit
			Help
			Calculation Statement
		Statement:
			"from" Filename
			"to" Filename
			Assignment
			Declaration
			Expression
		Assignment:
			Name "=" Expression
		Declaration:
			"let" Name "=" Expression
			"const" Name "=" Expression
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
			Roman Numeral
			Number
			Number "!"
			Variable
			"(" Expression ")"
			"sqrt(" Expression ")"
			"pow(" Expression "," Expression ")"
		Number:
			Floating-Point Value
		Variable:
			Name
		Filename:
			String
*/
#include "std_lib_facilities.h"
#include "Roman.h"
#include <math.h>

const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char square_root = '?';
const char power = '&';
const char constant = 'C';
const char help = 'H';
const char roman_number = 'R';
const char input_keyword = 'I';
const char output_keyword = 'O';
const char filename = 'F';
const char end_of_file = 'E';

class Token {
public:
	char kind;
	double value;
	string name;
	Roman::Roman_int roman;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string s) :kind(ch), value(0), name(s) { }
	Token(char ch, Roman::Roman_int r) :kind(ch), roman(r) {	}
};

class Token_stream {
public:
	Token_stream(istream& is) :full(0), buffer(0), stream(is) { }
	Token get();
	void putback(Token t) { buffer = t; full = true; }

	void ignore(char);
private:
	bool full;
	Token buffer;
	istream& stream;
};

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch;
	while (stream >> ch)
		if (ch == c) return;
}

bool is_roman_numeral(string);

Token Token_stream::get()
{
	if (full) { full = false; return buffer; }

	char ch;
	stream >> ch;

	if (stream.eof()) return Token(end_of_file);

	switch (ch) {
	case '(': case ')':	case '+': case '-':	case '*':
	case '/': case '%':	case ';': case '=':	case ',': case '!': case'_':
		return Token(ch);
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	{
		stream.putback(ch);
		double val;
		stream >> val;
		return Token(number, val);
	}
	default:
		if (isalpha(ch)) { // isalpha(ch) returns true if ch is a character
			string s;
			s += ch;
			while (stream.get(ch) && (isalpha(ch) || isdigit(ch))) s += ch;
			stream.putback(ch);
			if (s == "let") return Token(let);
			if (s == "quit") return Token(quit);
			if (s == "sqrt") return Token(square_root);
			if (s == "pow") return Token(power);
			if (s == "const") return Token(constant);
			if (s == "help") return Token(help);
			if (is_roman_numeral(s)) return Token(roman_number, Roman::string_to_roman(s));
			if (s == "from")
			{
				if (stream.get(ch) && isspace(ch))
				{
					string filename;
					while (stream.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '.')) filename += ch;
					stream.putback(ch); // putback ';'
					return Token(input_keyword, filename);
				}
			}
			if (s == "to") return Token(output_keyword);
			return Token(name, s);
		}
		if (isspace(ch)) {
			if (ch == '\n') return Token(print);
		}
		error("Bad token");
	}
}

class Variable {
public:
	string name;
	double value;
	bool constant;
	Variable(string n, double v, bool c = false) :name(n), value(v), constant(c) { }
};

class Symbol_table {
public:
	double get(string s);
	void set(string s, double d);
	void define_name(string name, double value, bool constant);
	bool is_declared(string s);
	bool is_constant(string s);

private:
	vector<Variable> var_table;
};

double Symbol_table::get(string s)
// return value of an existing variable
{
	for (int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == s) return var_table[i].value;
	error("get: undefined name ", s);
}

void Symbol_table::set(string s, double d)
// change value of an existing variable
{
	for (int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == s) {
			var_table[i].value = d;
			return;
		}
	error("set: undefined name ", s);
}

void Symbol_table::define_name(string name, double value, bool constant = false)
// function to define a variable before running the code
{
	if (is_declared(name)) error("define_name(): existing variable");
	var_table.push_back(Variable(name, value, constant));
	return;
}

bool Symbol_table::is_declared(string s)
// return true if a variable is declared
{
	for (int i = 0; i < var_table.size(); ++i)
		if (var_table[i].name == s) return true;
	return false;
}

bool Symbol_table::is_constant(string s)
// return true if a variable is declared an it is a constant
{
	if (is_declared(s)) {
		for (int i = 0; i < var_table.size(); ++i) {
			if (var_table[i].name == s) return var_table[i].constant;
		}
		return false;
	}
	error("is_constant: undefined name");
}

vector<char> roman_numeral_tbl{ 'M', 'D', 'C', 'L', 'X', 'V', 'I' };

bool is_roman_numeral(string s)
{
	for (const char& ch : s)
	{
		bool status = false;

		for (const char& roman_numeral : roman_numeral_tbl)
		{
			if (ch == roman_numeral)
			{
				status = true;
				break;
			}
		}

		if (!status) return false;
	}
	return true;
}

Symbol_table names;

double expression(Token_stream& ts);
double assignment(Token_stream& ts, string s);

double power_get(Token_stream& ts) {
	double val1;
	int val2;
	Token t = ts.get();
	if (t.kind == '(') {
		val1 = expression(ts);
		t = ts.get();
		if (t.kind == ',') {
			val2 = narrow_cast<int>(expression(ts));
			t = ts.get();
			if (t.kind == ')') return pow(val1, val2); // ignore the ")" at the end of function call "pow(val1, val2);"
			else ts.putback(t);
		}
	}
	error("pow(val1, val2): Bad Input");
}

double factorial(double factor)
// calculate factorial of a number
{
	if (factor == 0) return 1;
	int return_value = factor;
	for (int i = 1; i < factor; ++i) {
		return_value *= i;
	}
	return return_value;
}

double primary(Token_stream& ts)
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{
		double d = expression(ts);
		t = ts.get();
		if (t.kind != ')') error("'(' expected");
		return d;
	}
	case '-':
		return -primary(ts);
	case number:
	{
		Token next = ts.get();
		if (next.kind == '!') return factorial(t.value);
		else ts.putback(next);
		return t.value;
	}
	case roman_number:
	{
		Token next = ts.get();
		if (next.kind == '!') return factorial(t.roman.as_int());
		else ts.putback(next);
		return t.roman.as_int();
	}
	case name:
	{
		Token t2 = ts.get();
		if (t2.kind == '=') {
			return assignment(ts, t.name);
		}
		ts.putback(t2);
		return names.get(t.name);
	}
	case square_root:
	{
		double d = expression(ts);
		if (d < 0) error("sqrt(n): n should be a positive number");
		return sqrt(d);
	}
	case power:
	{
		return power_get(ts);
	}
	default:
		error("primary expected");
	}
}

double term(Token_stream& ts)
{
	double left = primary(ts);
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '*':
			left *= primary(ts);
			break;
		case '/': {
			double d = primary(ts);
			if (d == 0) error("divide by zero");
			left /= d;
			break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}

double expression(Token_stream& ts)
{
	double left = term(ts);
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '+':
			left += term(ts);
			break;
		case '-':
			left -= term(ts);
			break;
		default:
			ts.putback(t);
			return left;
		}
	}
}

double declaration(Token_stream& ts, bool c = false)
{
	Token t = ts.get();
	if (t.kind != 'a') error("name expected in declaration");
	string name = t.name;
	if (names.is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ", name);
	double d = expression(ts);
	names.define_name(name, d, c);
	return d;
}

double assignment(Token_stream& ts, string name) {
	if (names.is_constant(name)) error("assignment: const variable");
	double d = expression(ts);
	names.set(name, d);
	return d;
}

double statement(Token_stream& ts)
{
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration(ts);
	case constant:
		return declaration(ts, true);
	default:
		ts.putback(t);
		return expression(ts);
	}
}

void clean_up_mess(Token_stream& ts)
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate(Token_stream& ts)
{
	while (true) try {
		Token t = ts.get();
		while (t.kind == print)	t = ts.get();
		if (t.kind == end_of_file) return;
		else if (t.kind == quit) return;
		else if (t.kind == help) cout << "This is me, calculator!\n"
			<< "I can read these operators:\t+\t-\t/\t*\t(\t)\n"
			<< "I can execute these functions: pow(x,i)\tsqrt(x)\n";
		else if (t.kind == input_keyword)
		{
			ifstream ifs{ t.name };
			Token_stream file_input{ ifs };
			t = ts.get();
			if (t.kind == print) calculate(file_input);
		}
		else {
			ts.putback(t);
			cout << result << statement(ts) << endl << prompt;
		}
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess(ts);
	}
}

int main()
try {
	names.define_name("pi", 3.14159, true);
	Token_stream ts{ cin };
	calculate(ts);
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
