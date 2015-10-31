#ifndef __NODE_H__
#define __NODE_H__

#include "../mg_connection_w.h"
#include <string>
#include "../util/log.h"


class Node {
	public:
		Node(const char* str);

		/** Method execute this node
					 */
		void execute(MgConnectionW& conn, const char* url);

	protected:

			/** 
			/ Method that is executed if the connection is POST
			*/
		virtual void executePost(MgConnectionW& conn, const char* url);

			/** 
			/ Method that is executed if the connection is POST
			*/
		virtual void executeGet(MgConnectionW& conn, const char* url);

			/** 
			/ Method that is executed if the connection is POST
			*/
		virtual void executeDelete(MgConnectionW& conn, const char* url);

			/** 
			/ Method that is executed if the connection is POST
			*/
		virtual void methodNotAllowed(MgConnectionW& conn, const char* url);

		std::string uri;

		virtual std::string handlerError(int status);
};


#endif
