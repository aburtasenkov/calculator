#include "std_lib_facilities.h"

class Token_stream {
public:
	void putback(string word);
	string get();
private:
	bool full = false;
	string buffer = "";
};

string Token_stream::get() {
	if (full) {
		full = false;
		return buffer;
	}
	string word;
	cin >> word;
	return word;
}

void Token_stream::putback(string word) {
	if (full) throw runtime_error("Buffer is full");
	buffer = word;
	full = true;
}

Token_stream ts;

bool noun(string word) {
	if (word == "birds" || word == "fish" || word == "C++") return true;
	ts.putback(word);
	return false;
}

bool verb(string word) {
	if (word == "rules" || word == "fly" || word == "swim") return true;
	ts.putback(word);
	return false;
}

bool conj(string word) {
	if (word == "and" || word == "or" || word == "but") return true;
	ts.putback(word);
	return false;
}

bool sentence() {
	string word = ts.get();
	if (noun(word))
	{
		word = ts.get();
		if (verb(word)) return true;
		return false;
	}
	else if (conj(word)) {
		return sentence();
	}
	return false;
}

int main()
try {
	string word = "";
	while (cin) {
		bool b = sentence();
		if (b) cout << "OK\n";
		else cout << "Not OK\n";
	}
	return 0;
}
catch (runtime_error& e) {
	cerr << "Error detected: " << e.what() << "\n";
	return 1;
}
catch (...) {
	cerr << "Error detected: Unknown\n";
	return 2;
}