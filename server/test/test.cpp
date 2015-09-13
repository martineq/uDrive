// test.cpp

#include "../src/LibraryTest.h"

int main(int argc, char **argv) {

    LibraryTest lt;
    lt.EjemploRocksDB();
    //lt.EjemploMongoose();
    lt.EjemploJson();

    // Test siempre positivo (Sólo de prueba)
    return 0;
}

