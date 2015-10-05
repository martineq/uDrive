// test.cc

#include <iostream>
#include "../src/helloworld.h"

using namespace std;

int main(int argc, char **argv) {
  
  HelloWorld hw;
  //hw.EjemploRocksDB();
  hw.EjemploMongoose();
  hw.EjemploJson();

  // Test siempre positivo (Sólo de prueba)
  return 0;
}

