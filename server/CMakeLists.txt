#######################################
# Archivo de configuración para CMake #
#######################################

##########
# Seteos #
##########

# Versión de CMake mínima usada para generar el proyecto, si no la encuentra, finaliza.
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Definición del proyecto. Con CXX indico que todos los archivos son tratados como C++.
set(NOMBRE_PROYECTO "server")  # Seteo el nombre del ejecutable
project(${NOMBRE_PROYECTO} C CXX)

# Seteo rutas del directorio de librerias y del directorio de código fuente
set(RUTA_RAIZ "${${NOMBRE_PROYECTO}_SOURCE_DIR}")
set(RUTA_LIB "${RUTA_RAIZ}/lib")
set(RUTA_SRC "${RUTA_RAIZ}/src")
set(RUTA_TOOLS "${RUTA_RAIZ}/tools")
set(RUTA_TEST "${RUTA_RAIZ}/test")


### Seteos referidos a la compilación del servidor ###

# Seteo distintos paquetes de archivos fuente, se agregan líneas nuevas según necesidad
# Ejemplo para agregar paquetes: set(PACK_NOMBRE_DEL_PACK ${RUTA_SRC}/nombre_fuente_1.cpp ${RUTA_SRC}/nombre_fuente2.cpp)
set(PACK_HOLA_MUNDO ${RUTA_SRC}/LibraryTest.cpp)
set(PACK_MONGOOSE ${RUTA_LIB}/mongoose/mongoose.c)
#...mas packs acá...

# Seteo todos los fuentes que serán testeados y tendrá análisis de code coverage
<<<<<<< HEAD
set(SRC_TESTEABLE ${PACK_HOLA_MUNDO})
=======
#set(SRC_TESTEABLE ${RUTA_SRC}/LibraryTest.cpp)
>>>>>>> refs/remotes/origin/feat/src-RestLogin

# Seteo todos los fuentes que no necesiten testeo y code coverage (por ejemplo el main, archivos fuente de librerías)
<<<<<<< HEAD
set(SRC_NO_TESTEABLE ${RUTA_SRC}/main.cpp ${PACK_MONGOOSE})
=======
set(SRC_NO_TESTEABLE ${RUTA_SRC}/main.cpp ${RUTA_SRC}/WEBserver.cpp ${RUTA_SRC}/util/log.cpp ${RUTA_LIB}/mongoose/mongoose.c)
>>>>>>> refs/remotes/origin/feat/src-RestLogin

# Seteo TODOS los fuentes que necesita el proyecto
set(SRC ${SRC_NO_TESTEABLE} ${SRC_TESTEABLE})

# Seteo los nombres de las librerías
set(LIBS rocksdb bz2 snappy z pthread rt jsoncpp)

# Flag para indicar si se quiere generar un ejecutable
<<<<<<< HEAD
option(EXE "Activa la creación de un ejecutable, sin tests" NO)
=======
option(SOLO_EXECUTABLE "Activa la creación de un ejecutable, sin tests" ON)

set(SOLO_EXECUTABLE "YES") #Pongo esto porque sino no funciona la linea anterior

message("SOLO EXECUTABLE " ${SOLO_EXECUTABLE})
>>>>>>> refs/remotes/origin/feat/src-RestLogin



### Seteos referidos al testing ###

# Seteo el nombre del test
set(NOMBRE_TESTING "test_server")  

# Seteo los archivos fuente que se usan para realizar el testeo
set(SRC_TESTING ${RUTA_TEST}/test.cpp)



### Comandos para el uso de coveralls-cmake ###

# Setea las opciones para la herramienta coveralls-cmake
option(COVERALLS "Activa el soporte para Coveralls.io" OFF)
option(COVERALLS_UPLOAD "Sube el json generado de coveralls" ON)

# Seteo la ruta donde se encuentran los scripts de la herramienta coveralls-cmake
set(RUTA_COVERALLS_CMAKE "${RUTA_TOOLS}/coveralls-cmake/cmake")

# Agrega directorios para la búsqueda de módulos CMake dentro de la variable CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${RUTA_COVERALLS_CMAKE})


########################
# Chequeo de Librerías #
########################

# Chequeo la existencia de todas las librerias agregadas
foreach(LIB ${LIBS})
  find_library(LIBRERIA${LIB} ${LIB})
  if (LIBRERIA${LIB}) #False si es 0, OFF, NO, FALSE, N, IGNORE, NOTFOUND, vacio, o si termina en "-NOTFOUND". Caso contrario es TRUE.
    message(STATUS "Librería |${LIB}| encontrada en |${LIBRERIA${LIB}}|")         
  else (LIBRERIA${LIB})
    message(FATAL_ERROR "No se encontró la librería |${LIB}|. Verificar existencia.")
  endif (LIBRERIA${LIB})
endforeach(LIB ${LIBS})


#########################################
# Configuración de compilación y linkeo #
#########################################

# Verifico si debo crear un ejecutable o una librería
<<<<<<< HEAD
if (EXE)
  message(STATUS "- Generación de ejecutable")
  # Nombre del ejecutable resultante de la compilación. Los demás argumentos son los archivos fuente (sin los headers)         
  add_executable(${NOMBRE_PROYECTO} ${SRC})
else (EXE)
  message(STATUS "- Generación de biblioteca")
  # Creo una biblioteca para correr los tests
  add_library(${NOMBRE_PROYECTO} ${SRC})
endif (EXE)
=======
if (SOLO_EXECUTABLE)
	message(STATUS "- Modo ejecutable")
	# Nombre del ejecutable resultante de la compilación. Los demás argumentos son los archivos fuente (sin los headers)         
	add_executable(${NOMBRE_PROYECTO} ${SRC})
else (SOLO_EXECUTABLE) 
	message(STATUS "- Modo biblioteca")
 	# Creo una biblioteca para correr los tests
	add_library(${NOMBRE_PROYECTO} ${SRC})
endif (SOLO_EXECUTABLE)
>>>>>>> refs/remotes/origin/feat/src-RestLogin

# Incluyo la ruta de las librerías para el compiler
include_directories(${RUTA_LIB})

# Vinculo la ruta de las librerías y los nombres de las mismas para el linker
link_directories(${RUTA_LIB})
target_link_libraries(${NOMBRE_PROYECTO} ${LIBS})

# Línea agregada para hacer uso de ISO C++11 requerido por RocksDB.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


###################################################
# Configuración de la herramienta coveralls-cmake #
###################################################

# Si activo Coveralls en la llamada de CMake 
if (COVERALLS)
  # Agrego el módulo CMake correspondiente...
  include(Coveralls)

  # ...Agrego los flags de coverage (-fprofile-arcs -ftest-coverage)...
  coveralls_turn_on_coverage()

  # ... Y preparo la herramienta coveralls-cmake
  # 1er argumento: Lista de archivos fuente
  # 2do argumento: "ON" para subir al sitio Coveralls.io
  # 3er argumento: Ruta alternativa de ubicación de la herramienta coveralls-cmake
  coveralls_setup("${SRC_TESTEABLE}" 
                   ${COVERALLS_UPLOAD}
                  "${RUTA_COVERALLS_CMAKE}")
endif(COVERALLS)


#########################
# Configuración de test #
#########################

if (NOT EXE)
  # Agregos los tests
<<<<<<< HEAD
  enable_testing()
  add_executable(${NOMBRE_TESTING} ${SRC_TESTING})
  target_link_libraries(${NOMBRE_TESTING} ${NOMBRE_PROYECTO})
  add_test(${NOMBRE_PROYECTO} ${NOMBRE_TESTING})
=======
#  enable_testing()
#  add_executable(test_ejemplo ${RUTA_TEST}/test.cpp)
#  target_link_libraries(test_ejemplo ${NOMBRE_PROYECTO})
#  add_test(${NOMBRE_PROYECTO} test_ejemplo)
>>>>>>> refs/remotes/origin/feat/src-RestLogin
endif ()

