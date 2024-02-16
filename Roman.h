#include <iostream>
#include <vector>

namespace Roman {
	class Roman_int {
	public:
		Roman_int(int i)
			:integer(i) {	}
		Roman_int() {}

		int as_int() const { return integer; }
	private:
		int integer;
	};

	vector<std::string> roman_numeral_tbl{ "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I" };
	vector<int> roman_number_tbl{ 1000, 900, 500, 400,  100,  90,  50,   40,  10,   9,   5,    4,    1 };

	std::string get_roman_numeral(Roman_int r)
	{
		int number = r.as_int();
		std::string roman;

		for (int index = 0; index < Roman::roman_numeral_tbl.size(); ++index)
		{
			while (number - roman_number_tbl[index] >= 0)
			{
				roman += roman_numeral_tbl[index];
				number -= roman_number_tbl[index];
			}
		}

		return roman;
	}

	int value(char roman)
	{
		switch (roman)
		{
		case 'I': return 1;
		case 'V': return 5;
		case 'X': return 10;
		case 'L': return 50;
		case 'C': return 100;
		case 'D': return 500;
		case 'M': return 1000;
		}
	}

	Roman_int string_to_roman(std::string s)
	{
		int sum = 0;
		int last = 0;
		for (int index = s.length() - 1; index >= 0; --index)
		{
			if (value(s[index]) >= last) sum += value(s[index]);
			else sum -= value(s[index]);
			last = value(s[index]);
		}
		return sum;
	}

	std::ostream& operator<<(std::ostream& os, Roman_int& r)
	{
		return os << get_roman_numeral(r);
	}

	std::istream& operator>>(std::istream& is, Roman_int& r)
	{
		int i;
		is >> i;

		r = Roman_int{ i };
		return is;
	}
}
