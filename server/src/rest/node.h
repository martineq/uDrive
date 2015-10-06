#ifndef __NODE_H__
#define __NODE_H__

#include "../mg_connection_w.h"


class Node {
	public:
		Node(const char* str);
		bool handle(MgConnectionW& conn, const char* url);

		/** Chequea el metodo (METHOD HTTP) de la connecion y ejecuta el metodo correspondiente
					 */
		void execute(MgConnectionW& conn, const char* url);

	protected:
			/** Metodo que se ejecuta si la connecion es POST
			 */
		virtual void executePost(MgConnectionW& conn, const char* url);
			/** Metodo que se ejecuta si la connecion es GET
			 */
		virtual void executeGet(MgConnectionW& conn, const char* url);

			/** Metodo que se ejecuta si la connecion es DELETE
			 */
		virtual void executeDelete(MgConnectionW& conn, const char* url);

			/** Manejador de metodo no implementado
			 */
		virtual void methodNotAllowed(MgConnectionW& conn, const char* url);

		std::string uri;
};


#endif
