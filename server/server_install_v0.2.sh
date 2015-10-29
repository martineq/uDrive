#!/bin/bash

########################################################################
## Script de instalación del server SIN Docker                        ##
## Sistema operativo utilizado: Ubuntu 14.04 LTS                      ##
## Para correr este script, ejecutar:                                 ##  
## chmod 777 server_install_v0.2.sh                                   ##
## ./server_install_v0.2.sh                                           ##
########################################################################

# Limpieza de librerías debido a posibles versiones conflictivas
echo "Búsqueda y limpieza de librerías RocksDB, jsoncpp, yaml-cpp y gtest existentes..."
find / -type f -name "librocksdb.a" -exec rm -i {} \;
find / -type f -name "librocksdb.so" -exec rm -i {} \;
find / -type f -name "libjsoncpp.a" -exec rm -i {} \;
find / -type f -name "libjsoncpp.so" -exec rm -i {} \;
find / -type f -name "libyaml-cpp.a" -exec rm -i {} \;
find / -type f -name "libyaml-cpp.so" -exec rm -i {} \;
find / -type f -name "libgtest.a" -exec rm -i {} \;
find / -type f -name "libgtest_main.a" -exec rm -i {} \;
find / -type f -name "libgtest.so" -exec rm -i {} \;
find / -type f -name "libgtest_main.so" -exec rm -i {} \;
echo "Fin de búsqueda..."

# Instalación de dependencias y herramientas
echo "Instalación de dependencias..."
apt-get update && apt-get install -y \
build-essential \
python \
git \
cmake \
wget \
unzip \
curl \
libsnappy-dev \
zlib1g-dev \
libbz2-dev \
libgflags-dev

# Instalación de librerías
echo "Instalación de librerías RocksDB, jsoncpp, yaml-cpp y gtest..."
apt-get install -y wget
mkdir temp_install && \
cd temp_install && \
wget https://github.com/facebook/rocksdb/archive/v3.13.1.zip && \
unzip v3.13.1.zip && \
cd rocksdb-3.13.1 && \
make static_lib && \
sudo cp librocksdb.a /usr/lib && \
cd .. && \
rm v3.13.1.zip && \
wget https://github.com/open-source-parsers/jsoncpp/archive/1.6.5.zip && \
unzip 1.6.5.zip && \
cd jsoncpp-1.6.5 && \
python amalgamate.py && \
cd dist && \
gcc -c jsoncpp.cpp && \
ar rvs libjsoncpp.a jsoncpp.o && \
sudo cp libjsoncpp.a /usr/lib && \
cd ../.. && \
rm 1.6.5.zip && \
wget https://github.com/jbeder/yaml-cpp/archive/release-0.3.0.zip && \
unzip release-0.3.0.zip && \
cd yaml-cpp-release-0.3.0 && \
mkdir build && \
cd build && \
cmake .. && \
make && \
cp libyaml-cpp.a /usr/lib && \
cd ../.. && \
rm -rf yaml-cpp-release-0.3.0 && \
rm release-0.3.0.zip && \
wget https://googletest.googlecode.com/files/gtest-1.7.0.zip && \
unzip gtest-1.7.0.zip && \
cd gtest-1.7.0 && \
./configure && \
make && \
cp -a lib/.libs/libgtest.a /usr/lib && \
cp -a lib/.libs/libgtest_main.a /usr/lib && \
cd .. && \
rm -rf gtest-1.7.0 && \
rm gtest-1.7.0.zip && \
cd .. && \
rm -rf temp_install
echo "Librerías instaladas en /usr/lib/"

# Bajo el repositorio
echo "Obtención de código fuente..."
git clone --quiet --branch v0.2 --depth 1  https://github.com/martineq/tp7552.git tp7552

echo "Compilación y ejecución del servidor..."
cd tp7552/server && mkdir build && cd build
cmake -DEXE=SI .. && make
