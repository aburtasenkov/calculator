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
	string word = "";
	if (cin) cin >> word;
	return word;
}

void Token_stream::putback(string word) {
	if (full) throw runtime_error("Buffer is full");
	buffer = word;
	full = true;
}

Token_stream ts;

bool noun(string word) {
	// return true if word is a noun or "the" + noun
	if (word == "birds" || word == "fish" || word == "C++") return true;
	else if (word == "the") {
		word = ts.get();
		if (word == "birds" || word == "fish" || word == "C++") return true;
		ts.putback(word);
	}
	return false;
}

bool verb(string word) {
	// return true if word is a verb
	if (word == "rules" || word == "fly" || word == "swim") return true;
	return false;
}

bool conj(string word) {
	// return true if word is a conjunction
	if (word == "and" || word == "or" || word == "but") return true;
	return false;
}

bool sentence() {
	// return true if input is a legit sentence
	string word = ts.get();
	if (noun(word))
	{
		word = ts.get();
		if (verb(word)) return true;
	}
	else if (conj(word)) {
		if (sentence()) return true;
	}
	return false;
}

int main()
try {
	string word = "";
	while (cin) {
		if (sentence()) cout << "OK\n";
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