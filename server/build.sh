#!/bin/bash

##
# Script para crear el ejecutable del servidor
if [ -d ./build ];
	then
		echo 'La carpeta build ya esta creada'
	else
		echo 'creo la carpeta'
		mkdir build
		cd build
	fi
cmake ./
make
exit 0

