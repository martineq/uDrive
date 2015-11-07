#ifndef __NODE_H__
#define __NODE_H__

#include "../mg_connection_w.h"
#include <string>
#include "../util/log.h"
#include "../request_dispatcher.h"

class Node {
	public:
		Node(MgConnectionW& conn);
		void execute();
		void setRequestDispatcher(RequestDispatcher* rd);
		RequestDispatcher* getRequestDispatcher();

	protected:
			/**
			/ Method that is executed if the connection is POST
			*/
		virtual void executePost();

			/** 
			/ Method that is executed if the connection is POST
			*/
		virtual void executeGet();

			/** 
			/ Method that is executed if the connection is POST
			*/
		virtual void executeDelete();

		virtual void executePut();
			/** 
			/ Method that is executed if the connection is POST
			*/
		virtual void methodNotAllowed();

		virtual std::string handlerError(int status);

        virtual std::string defaultResponse();

		const char* getUri();

		MgConnectionW getConnection();

		void setConnection(MgConnectionW& conn);

        virtual bool auth(int &status);

        virtual void requestForbidden(int status);
private:
		RequestDispatcher* rd= nullptr;

		virtual std::string getUserId();

		MgConnectionW connection= nullptr;
};


#endif
