#ifndef __LOG_H__
#define __LOG_H__

#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

/** Clase para logear.
 * Por defecto el nivel de log es LogMsgError y la salida de log es std::cout
 */
class Log {
	public:
		typedef enum {
			LogMsgDebug=0,
			LogMsgWarning,
			LogMsgInfo,
			LogMsgError
		} LogMsgType;

		/** Clase utilizada para imprimir a un vector como si fuera una string
		 */
		class VectorStr {
			public:
				VectorStr(const std::vector<char> &vec) : v(vec){}
				const char& operator[] (size_t n) const { return this->v[n]; }
				size_t size() const { return this->v.size(); }
				std::vector<char>::const_iterator begin() const { return this->v.begin(); }
				std::vector<char>::const_iterator end() const { return this->v.end(); }

			protected:
				const std::vector<char> &v;
		};

		/** Clase de logging
		 */
		Log(LogMsgType type = LogMsgDebug);
		~Log();

		/** Escribe un VectorStr al log
		 */
		Log &operator<<(const VectorStr &v){
			if(Log::logLevel <= this->level)
				for(auto it = v.begin(); it != v.end(); it++)
					this->ss << (*it);
			return *this;
		}

		/** Escribe un vector<char> al log imprimiendo en hexa
		 */
		Log &operator<<(const std::vector<char> &v){
			if(Log::logLevel <= this->level)
				for(auto it = v.begin(); it != v.end(); it++)
					this->ss << std::setfill('0') << std::setw(2) << std::hex << (unsigned int) ((unsigned char) (*it));
			return *this;
		}

		/** Escribe un vector<unsigned char> al log imprimiendo en hexa
		 */
		Log &operator<<(const std::vector<unsigned char> &v){
			if(Log::logLevel <= this->level)
				for(auto it = v.begin(); it != v.end(); it++)
					this->ss << std::setfill('0') << std::setw(2) << std::hex << (unsigned int) (*it);
			return *this;
		}

		/** Imprime cualquiercosa al log, utilizando el operador << del ostringstream
		 */
		template<typename T>
			Log &operator<<(const T &t){
				if(Log::logLevel <= this->level)
					this->ss << t;
				return *this;
			}

		/** Setea el nivel de log
		 * @param nivel de log
		 */
		static void setLogLevel(LogMsgType);

		/**
		 * "debug" -> LogMsgDebug
		 * "warning" -> LogMsgWarning
		 * "info" -> LogMsgInfo
		 * "error" -> LogMsgError
		 */
		static void setLogLevel(const std::string&);

		/** Setea el output
		 * El usuario debe ser el encargado de liberar el ostream proporcionado.
		 * La libreria solo usa una referencia, si el usuario elimina el objeto y usa Log dsp, explota todo :(
		 * @param referencia a un ostream donde se escribira.
		 */
		static void setOutput(std::ostream&);

	protected:
		std::ostringstream ss; ///< Buffer
		LogMsgType level; ///< Nivel de log del mensaje

		static LogMsgType logLevel; ///< Nivel de log actual
		static std::ostream* output; ///< Output
};

#endif
