###################################################
# Archivo de configuración CMake para el servidor #
###################################################

#############################
# Seteos de nombres y rutas #
#############################

# Versión de CMake mínima usada para generar el proyecto, si no la encuentra, finaliza.
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(NOMBRE_PROYECTO "server")  # Seteo el nombre del ejecutable
project(${NOMBRE_PROYECTO} C CXX) # Definición del proyecto

set(RUTA_RAIZ "${${NOMBRE_PROYECTO}_SOURCE_DIR}") # Ruta del directorio raíz
set(RUTA_LIB "${RUTA_RAIZ}/lib") # Ruta del directorio donde se encuentran los headers de las librerías usadas
set(RUTA_SRC "${RUTA_RAIZ}/src") # Ruta del directorio donde se encuentran los archivos fuente
set(RUTA_TOOLS "${RUTA_RAIZ}/tools") # Ruta del directorio donde se encuentran las herramientas para usar en el proyecto
set(RUTA_TEST "${RUTA_RAIZ}/test") # Ruta del directorio donde se encuentran los archivos de test


######################################################
### Seteos referidos a la compilación del servidor ###
######################################################

# Seteo distintos paquetes de archivos fuente, se agregan líneas nuevas según necesidad
# Ejemplo para agregar paquetes: set(SRC_PACK_NOMBRE_DEL_PACK ${RUTA_SRC}/nombre_fuente_1.cc ${RUTA_SRC}/nombre_fuente2.cc)
set(SRC_PACK_MAIN ${RUTA_SRC}/main.cc)
set(SRC_PACK_WEB_SERVER ${RUTA_SRC}/web_server.cc ${RUTA_SRC}/mg_connection_w.cc ${RUTA_LIB}/mongoose/mongoose.c)
set(SRC_PACK_LOG ${RUTA_SRC}/util/log.cc)
set(SRC_PACK_DB ${RUTA_SRC}/db_handler.cc ${RUTA_SRC}/data_handler.cc ${RUTA_SRC}/request_dispatcher.cc)
set(SRC_PACK_UTILS ${RUTA_SRC}/util/md5.cc ${RUTA_SRC}/util/random_number.cc ${RUTA_SRC}/util/bin2hex.cc)
set(SRC_PACK_NODOS ${RUTA_SRC}/rest/node.cc ${RUTA_SRC}/rest/token_node.cc ${RUTA_SRC}/rest/info_node.cc
        ${RUTA_SRC}/rest/receive_file_node.cc ${RUTA_SRC}/rest/signup_node.cc ${RUTA_SRC}/rest/create_dir_node.cc
        ${RUTA_SRC}/rest/profile_node.cc ${RUTA_SRC}/rest/update_photo_node.cc ${RUTA_SRC}/rest/delete_file_node.cc
        ${RUTA_SRC}/rest/send_file_node.cc ${RUTA_SRC}/rest/update_userfullname_node.cc ${RUTA_SRC}/rest/send_dir_node.cc
        ${RUTA_SRC}/rest/info_trash_node.cc ${RUTA_SRC}/rest/delete_trash_node.cc ${RUTA_SRC}/rest/delete_file_trash_node.cc
        ${RUTA_SRC}/rest/restore_file_trash_node.cc ${RUTA_SRC}/rest/restore_trash_node.cc ${RUTA_SRC}/rest/delete_dir_node.cc
        ${RUTA_SRC}/rest/list_tags_node.cc ${RUTA_SRC}/rest/list_tags_user_node.cc ${RUTA_SRC}/rest/update_tags_node.cc
        ${RUTA_SRC}/rest/update_collaborators_node ${RUTA_SRC}/rest/list_collaborators_node ${RUTA_SRC}/rest/search_users_node.cc
        ${RUTA_SRC}/rest/search_filename_node.cc ${RUTA_SRC}/rest/search_extension_node.cc ${RUTA_SRC}/rest/search_owner_node.cc
        ${RUTA_SRC}/rest/search_tag_node.cc ${RUTA_SRC}/rest/list_owners_node.cc ${RUTA_SRC}/rest/list_revisions_node.cc
        ${RUTA_SRC}/rest/list_info_elem_node.cc ${RUTA_SRC}/rest/update_location_node.cc)
set(SRC_PACK_YAML ${RUTA_SRC}/util/config_parser.cc)
set(SRC_PACK_FILE ${RUTA_SRC}/file_handler.cc ${RUTA_SRC}/zip_handler.cc)
# ***** EN ESTE SECTOR SE AGREGAN TODOS LOS NUEVOS ARCHIVOS FUENTE DEL SERVIDOR *****



# Todos los fuentes que serán testeados y tendrán análisis de code coverage. NO AGREGAR RUTAS, sólo variables "${SRC_PACK_...}"
set(SRC_TESTEABLE ${SRC_PACK_DB} ${SRC_PACK_YAML} ${SRC_PACK_FILE} )

# Fuentes que no necesiten testeo ni code coverage (main, librerías, etc). NO AGREGAR RUTAS, sólo variables "${SRC_PACK_...}"
set(SRC_NO_TESTEABLE ${SRC_PACK_MAIN} ${SRC_PACK_WEB_SERVER} ${SRC_PACK_LOG} ${SRC_PACK_UTILS} ${SRC_PACK_NODOS}) 

# Reuno TODOS los fuentes que necesita el proyecto
set(SRC ${SRC_NO_TESTEABLE} ${SRC_TESTEABLE}) # *NO* modificar esta línea, cargar en SRC_TESTEABLE o SRC_NO_TESTEABLE

# Nombres de las librerías a usar en el proyecto
set(NOMBRE_LIBS rocksdb z snappy bz2 jsoncpp pthread rt gtest yaml-cpp)

# Flag para indicar si se quiere generar un ejecutable. Para activarlo usar desde la carpeta build "cmake -DEXE=SI .."
option(EXE "Activa la creación de un ejecutable, sin tests" NO)

# Línea agregada para hacer uso de ISO C++11 requerido por RocksDB. pthread requerido por GTest
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11 ")


###################################
### Seteos referidos al testing ###
###################################

# Seteo el nombre del test
set(NOMBRE_TEST "test_server")

# Seteo distintos paquetes de archivos de test, se agregan líneas nuevas según necesidad
set(SRC_TEST_PACK_SIMPLE ${RUTA_TEST}/test.cc)
# ***** EN ESTE SECTOR SE AGREGAN TODOS LOS NUEVOS ARCHIVOS FUENTE CON CÓDIGO DE PRUEBAS *****

# Seteo TODOS los archivos fuente que se usan para realizar el testeo. NO AGREGAR RUTAS, sólo variables "${SRC_TEST_...}"
set(SRC_TEST ${SRC_TEST_PACK_SIMPLE})


#############################################
### Seteos para el uso de coveralls-cmake ###
#############################################

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
foreach(LIB ${NOMBRE_LIBS})
  find_library(LIBRERIA${LIB} NAMES ${LIB} )
  if (LIBRERIA${LIB}) #False si es 0, OFF, NO, FALSE, N, IGNORE, NOTFOUND, vacio, o si termina en "-NOTFOUND". Caso contrario es TRUE.
    message(STATUS "Librería |${LIB}| encontrada en |${LIBRERIA${LIB}}|")
  else (LIBRERIA${LIB})
    message(FATAL_ERROR "No se encontró la librería |${LIB}|. Verificar existencia.")
  endif (LIBRERIA${LIB})
endforeach(LIB ${NOMBRE_LIBS})


#########################################
# Configuración de compilación y linkeo #
#########################################

# Verifico si debo crear un ejecutable o una librería
if (EXE)
  message(STATUS "*** Generación de ejecutable ***")
  # Nombre del ejecutable resultante de la compilación. Los demás argumentos son los archivos fuente (sin los headers)
  add_executable(${NOMBRE_PROYECTO} ${SRC})
else (EXE)
  message(STATUS "*** Generación de biblioteca para test ***")
  # Creo una biblioteca para correr los tests
  add_library(${NOMBRE_PROYECTO} ${SRC})
endif (EXE)

# Incluyo la ruta de las librerías para el compiler
include_directories(${RUTA_LIB})
target_link_libraries(${NOMBRE_PROYECTO} ${NOMBRE_LIBS})


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
  enable_testing()
  add_executable(${NOMBRE_TEST} ${SRC_TEST})
  target_link_libraries(${NOMBRE_TEST} ${NOMBRE_PROYECTO})
  add_test(${NOMBRE_PROYECTO} ${NOMBRE_TEST})
endif ()
