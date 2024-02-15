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

	const vector<std::string> roman_numeral_tbl{ "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I" };
	const vector<int> roman_number_tbl{ 1000, 900, 500, 400,  100,  90,  50,   40,  10,   9,   5,    4,    1 };

	std::string roman_numeral(Roman_int r)
	{
		int number = r.as_int();
		std::string roman;

		for (int index = 0; index < roman_numeral_tbl.size(); ++index)
		{
			while (number - roman_number_tbl[index] >= 0)
			{
				roman += roman_numeral_tbl[index];
				number -= roman_number_tbl[index];
			}
		}

		return roman;
	}

	std::ostream& operator<<(std::ostream& os, Roman_int& r)
	{
		return os << roman_numeral(r);
	}

	std::istream& operator>>(std::istream& is, Roman_int& r)
	{
		int i;
		is >> i;

		r = Roman_int{ i };
		return is;
	}
}
