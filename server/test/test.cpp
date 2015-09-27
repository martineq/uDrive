// test.cpp

#include <iostream>
#include "../src/LibraryTest.h"

using namespace std;

int main(int argc, char **argv) {

    LibraryTest lt;
    lt.EjemploRocksDB();
    lt.EjemploMongoose();
    lt.EjemploJson();

    // Test siempre positivo (Sólo de prueba)
    return 0;
}

