#!/bin/bash

##
# Script para crear el ejecutable del servidor
if [ -d ./build ];
	then
		echo 'La carpeta build ya esta creada'
	else
		echo 'creo la carpeta'
		mkdir build
	fi
cd build
cmake ../ cmake -DEXE=SI
make 
exit 0

