#ifndef __MG_CONNECTION_W_H__
#define __MG_CONNECTION_W_H__

#include "mongoose/mongoose.h"
#include <map>
#include <string>
#include "./util/log.h"

using std::string;

class MgConnectionW {
	public:
		MgConnectionW(struct mg_connection *conn);

		typedef enum StatusCodes {
			// 2xx Success
			STATUS_CODE_OK=200, // OK
			STATUS_CODE_CREATED=201,
			STATUS_CODE_NO_CONTENT=204, //No content

			// 4xx Client Error
			STATUS_CODE_BAD_REQUEST=400, //
			STATUS_CODE_UNAUTHORIZED=401, // Cliente no esta autorizado para lo que quiere hacer
			STATUS_CODE_FORBIDDEN=403, // El cliente tiene prohibido hacer eso (no es un tema de permisos)
			STATUS_CODE_NOT_FOUND=404, // No se encuentro
			STATUS_CODE_METHOD_NOT_ALLOWED=405, // Se pidio un GET/POST/DELETE y no se implementa ese metodo

			// 5xx Errores de Servidor
			STATUS_CODE_INTERNAL_ERROR=500 // Error del servidor

		} StatusCodes;

		/** Enum que representa los content types que puede tener la respuesta
		 */
		typedef enum ContentTypes {
			CONTENT_TYPE_JSON=0,
			CONTENT_TYPE_HTML,
			CONTENT_TYPE_TOTAL
		} ContentTypes;

		/**
		 * send status code connection
		 */
		void sendStatus(int code);

		/**
		 * Get transfer file with multipart mode.
		 */
		std::string getMultipartData(string& var_name, string& file_name);

		/**
		 * takes the token header
		 */
		std::string getAuthorization();

		/**
		 * It takes the value of the last field next json parameter in the header
		 */
		std::string getBodyJson(string field);

        /**
         * specifies the content type of the HTTP message
         */
		void sendContentType(MgConnectionW::ContentTypes type);

        size_t printfData(const char* fmt, ...);

		struct mg_connection* operator->();

		const char* getUri();
		void setUri(string uri);

		const char* getMethod();

		void setMethod(std::string method);

	protected:
		struct mg_connection *conn; 
		int multipartOffset;
};

#endif
