#ifndef __NODE_H__
#define __NODE_H__

#include "mongoose/mgconnection.h"


class Node {
	public:
		Node(const char* str);
		bool handle(MgConnection& conn, const char* url);
	protected:

		bool match(const char* url) = 0;

		void execute(MgConnection& conn, const char* url) = 0;

		/** Chequea el metodo (METHOD HTTP) de la connecion y ejecuta el metodo correspondiente
			 */
		void execute(MgConnection& conn, const char* url);

			/** Metodo que se ejecuta si la connecion es POST
			 */
		void executePost(MgConnection& conn, const char* url);

			/** Metodo que se ejecuta si la connecion es GET
			 */
		void executeGet(MgConnection& conn, const char* url);

			/** Metodo que se ejecuta si la connecion es DELETE
			 */
		void executeDelete(MgConnection& conn, const char* url);

			/** Manejador de metodo no implementado
			 */
		void methodNotAllowed(MgConnection& conn, const char* url);

		std::string uri;
};


#endif
