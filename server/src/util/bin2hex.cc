#include "bin2hex.h"

#include <sstream>
#include <iomanip>

using std::hex;
using std::setw;
using std::vector;
using std::string;
using std::setfill;
using std::stringstream;

string bin2hex(const vector<char> &bin){
	stringstream ss;

	for(auto it = bin.begin(); it != bin.end(); it++)
		ss << setfill('0') << setw(2) << hex << (unsigned int) ((unsigned char) (*it));

	return ss.str();
}

static unsigned char getBinVal(char c){
	switch(c){
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		case 'a':
		case 'A':
			return 10;
		case 'b':
		case 'B':
			return 11;
		case 'c':
		case 'C':
			return 12;
		case 'd':
		case 'D':
			return 13;
		case 'e':
		case 'E':
			return 14;
		case 'f':
		case 'F':
			return 15;
		default:
			return 0;
	}
}

vector<char> hex2bin(const string &hex){
	vector<char> ret;
	if(hex.size() % 2)
		return vector<char>();

	ret.reserve(hex.size()/2);

	for(size_t i=0; i < hex.size(); i+=2){
		unsigned char c = getBinVal(hex[i]) * 16 + getBinVal(hex[i+1]);
		ret.push_back(c);
	}

	return ret;
}
