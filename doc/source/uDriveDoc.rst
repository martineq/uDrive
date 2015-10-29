============================================

UDrive - Entrega 
============================================

**30/10/2015**

**Grupo Nro 5**

**Ayudante asignado: Pablo Rodríguez Manzi**

**Integrantes:**

+-------------------------------------+--------------------------------------+
|       Apellido y Nombre             |              Padrón                  |
+=====================================+======================================+
|       Cortazzo Romina Pamela        |              80915                   |
+-------------------------------------+--------------------------------------+
|           D'Onofrio Martín          |              82106                   |
+-------------------------------------+--------------------------------------+
|          Liva María Eugenia         |              86123                   |
+-------------------------------------+--------------------------------------+
|            Quiroz Martín            |              86012                   |
+-------------------------------------+--------------------------------------+

**Tag a descargar:**


============================================

Manual de proyecto
============================================

Gestión del proyecto
--------------------------------------------

Las tareas se gestionaron mediante el sistema de tickets que nos proveé GitHub. Por cada tarea a realizar se crea un issue que se asigna a un integrante del grupo.  
Para tener una trazabilidad del estado de las tareas y el código que corresponde a cada corrección, al realizar un commit se indica el número de issue al que corresponde. Por tal razón al momento de  definir las tareas se tuvo en cuenta que sean lo suficientemente atómicas como para cerrarlas con un único commit.

División de tareas
--------------------------------------------
Para el desarrollo de UDrive se conformaron los módulos Cliente y Servidor, asignando a dos integrantes del equipo a cargo principalmente de uno de los módulos. Lo cual puede cambiar de ser necesario. Las asignaciones fueron:

+ Cliente:  Liva María Eugenia y Cortazzo Romina Pamela
+ Servidor:  Quiroz Martín y D'Onofrio Martín

Tareas que componen el checklist nro 2
--------------------------------------------
Son todas las pertenecientes al milestone `Checkpoint #2 <https://github.com/martineq/tp7552/milestones/Checkpoint%20%232>`_

Funcionalidad entregada en el checklist nro 2
----------------------------------------------

En esta entrega se incluye a nivel funcionalidad: 

**Cliente:**

+ Configuración de la ip del servidor del cual se consumen los servicios (Connection Settings). 
+ Registración de un nuevo usuario (Sign Up).
+ Carga de un archivo (Add file).
+ Creación de una carpeta (Add folder).
+ Administración de perfil de usuario (My profile).

**Servidor:**

+ Integración con la base de datos del servicio de optención de token y lista de archichos.
+ Implementación de servicio de creación de carpeta.
+ Implementación de servicio de carga de archivo.
+ Implementación de servicio de registración de usuario.
	
Control de versiones - Repositorio
--------------------------------------------

El control de versiones del proyecto fue mediante Github.
Se utilizaron un mismo repositorio para el proyecto de servidor y del cliente.

`Repositorio <https://github.com/martineq/tp7552.git>`_

El manejo de branches que realizamos es el siguiente:

+ Master: Sólo se hará commit sobre este branch al momento de cerrar un checkpoint.
+ Develop: Se subirán a este branch las funcionalidades que se vayan cerrando.
+ Features branches: Cada integrante trabajará en una branch local y una vez que tenga la funcionalidad terminada, hará merge con Develop y volcará sobre este los cambios.

============================================

Manual Técnico
============================================

API REST
--------------------------------------------

Para la comunicación entre Cliente y Servidor se definieron los siguientes servicios REST:

- **Agrega un archivo en el directorio indicado**

 + URL: /users/{userId}/dir/{dirId}   
 + Verbo: POST
 + Ejemplo de entrada (multipart): 
	"file": binario 	
        "filename": "archivo.txt" 
 + Ejemplo de salida (JSON):  
	[
	{
	"id": 1,
	"name": "Carpeta1",
	"size": 1234,
	"type": "d",
	"cantItems": 2,
	"shared": true,
	"lastModDate": "10/08/2015"
	},
	{
	"id": 2
	"name": "Carpeta2",
	"size": 500,
	"type": "a",
	"cantItems": 1
	"shared": false,
	"lastModDate": "10/08/2015"
	},
	...
	]

- **Crea un directorio en el directorio** 

 + URL: /users/{userId}/dir/{dirId}   
 + Verbo: POST
 + Ejemplo de entrada (JSON): 
	{"dirName": "nuevo_directorio"}
 + Ejemplo de salida (JSON):  
	[
	{
	"id": 1,
	"name": "Carpeta1",
	"size": 1234,
	"type": "d",
	"cantItems": 2,
	"shared": true,
	"lastModDate": "10/08/2015"
	},
	{
	"id": 2
	"name": "Carpeta2",
	"size": 500,
	"type": "a",
	"cantItems": 1
	"shared": false,
	"lastModDate": "10/08/2015"
	},
	...
	]

- **Registración de usuario**

 + URL: /signup  
 + Verbo: POST
 + Descripción: Chequea si el e-mail enviado existe o no. Si existe devuelve un código de error ( != 1).De lo contrario inserta todos los campos recibidos en la base de datos y devuelve un código de operación exitosa (1).
 + Ejemplo de entrada (JSON): 
	{
	    "firstname": "nombre",
	    "lastname": "apellido",
	    "email": "mail@mail.com",
	    "password": "xxxxxxxx"
	}
 + Ejemplo de salida (JSON):  
	{
	    "resultCode": 1
	}

- **Consulta de perfil de usuario**

 + URL: /profile/{userId}  
 + Verbo: GET
 + Descripción: Trae todos los campos pertenecientes al perfil del usuario logueado en la aplicación.
 + Ejemplo de salida (JSON):  
	{
	    "firstname": "nombre",
	    "lastname": "apellido",
	    "email": "mail@mail.com",
	    /*"photo": "dfd5g46s5gfGf",*/
	    /*"lastLocation": "",*/
	    "userId": "5",
	    "quotaAvailable": "570 MB"
	    "quotaTotal": "756 MB"
	    "quotaUsagePercent": "75.39%"
	}


Base de Datos
--------------------------------------------
Para persistir los datos se utilizó RocksDB

**Información que se necesitaba persistir**

- Relacionados a un cliente (user):

 + user_id
 + user_email
 + user_password
 + user_token
 + user_name
 + user_location
 + user_dir_root
 + user_shared_files

- Relacionados a un directorio (directory):

 + dir_id
 + dir_name
 + dir_date_last_mod
 + dir_tags
 + dir_owner
 + dir_parent_directory
 + dir_files_contained
 + dir_directories_contained

- Relacionados a un archivo (file):

 + file_id
 + file_name
 + file_extension
 + file_date_last_mod
 + file_user_last_mod
 + file_tags
 + file_owner
 + file_size (expresado en Kilobytes)
 + file_deleted_status
 + file_users_shared
 + file_revision

- Relacionados a la búsquedas por atributos que no son el ID (index):

 + index_user_id_from_user_email

- Relacionados al manejo de asignaciones de id para usuarios, directorios y archivos (ticket):

 + ticket_last_user_id
 + ticket_last_dir_id
 + ticket_last_file_id


Servidor
--------------------------------------------

**Diagrama de clases**

.. image:: Diagramas/diagrama_clase_serv.png


**Instalacón y configuración**

1. Descargar el `script de instalación <https://raw.githubusercontent.com/martineq/tp7552/master/server/server_install_v0.1.sh>`_

2. Para la instalación:
	a. chmod 777 server_install_v0.1.sh

	b. ./server_install_v0.1.sh

3. Luego ejecutar el servidor con:
	a. ./server


**Ejecución de pruebas**

Luego de haber levantado el servidor con los pasos indicados anteriormente, ejecutar:

+ cmake -DCOVERALLS=ON -DCMAKE_BUILD_TYPE=Debug ..
+ make
+ make coveralls

Cliente
--------------------------------------------

**Instalacón y configuración**

1. Descargar el `apk de instalación <https://github.com/martineq/tp7552/raw/344e1550a2eab0febc324910d4edaf2256522ac9/client/src/Udrive/app/app-release.apk>`_

2. Configurar ip del servidor entrando a Settings > Connection settings

.. image:: Screenshots/settings.png

.. image:: Screenshots/connectionSettings1.png

.. image:: Screenshots/connectionSettings2.png





