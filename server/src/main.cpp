#include    <iostream>
#include    "LibraryTest.h"

using namespace std;
 
int main(int argc, char** argv) {

	LibraryTest lt;
	lt.EjemploRocksDB();
	lt.EjemploMongoose();
	lt.EjemploJson();

	return 0;
}

