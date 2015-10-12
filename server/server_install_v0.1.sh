#!/bin/bash

########################################################################
## Script de instalación del server SIN Docker                        ##
## Aclaración. Sistema operativo utilizado: Ubuntu 14.04 LTS          ##
## Para correr este script, ejecutar:                                 ##  
## chmod 111 server_install_v0.1.sh                                   ##
## ./server_install_v0.1.sh                                           ##
########################################################################

# Limpio porque pueden ser de una versión distinta a la usada
echo "Búsqueda y limpieza de librerías RocksDB y jsoncpp existentes..."
find / -type f -name "librocksdb.a" -exec rm {} \;
find / -type f -name "libjsoncpp.a" -exec rm {} \;
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

# Instala las librerías
echo "Instalación de librerías RocksDB, y jsoncpp..."
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
cd .. && \
rm -rf temp_install

# Bajo el repositorio
echo "Obtención de código fuente..."
git clone --quiet --branch v0.1 --depth 1  https://github.com/martineq/tp7552.git tp7552

echo "Compilación y ejecución del servidor..."
cd tp7552/server && mkdir build && cd build
cmake -DEXE=SI .. && make
