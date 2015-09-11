#include    <iostream>
#include    "LibraryTest.h"

using namespace std;
 
int main(int argc, char** argv) {

	LibraryTest lt;
	lt.RocksDBExample();
	lt.MongooseExample();
	lt.JsonExample();

	return 0;
}

