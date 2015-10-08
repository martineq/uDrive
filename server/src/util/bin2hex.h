#ifndef __BIN2HEX_H__
#define __BIN2HEX_H__
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

/** Transforma un binario a una representacion en hexa
 * @param bin: buffer binario
 * @return representacion hexa
 */
std::string bin2hex(const std::vector<char> &bin);

/** @see bin2hex()
 * Se itera siempre que first != last
 * @param first: ppcio
 * @param last: final 
 * @return representacion hexa
 */
template <typename T>
std::string bin2hex(T first, T last){
	std::stringstream ss;
	while(first!=last){
		ss << std::setfill('0') << std::setw(2) << std::hex << (unsigned int) ((unsigned char) (*first));
		++first;
	}
	return ss.str();
}

/** @see bin2hex()
 * @param first: de donde se comienza
 * @param size: tama~no hasta el cual se imprimira
 * @return representacion hexa
 */
template <typename T>
std::string bin2hex(T first, size_t size){
	return bin2hex((char*) first, ((char*)first)+size);
}

/** @see bin2hex()
 * devuelve representacion hexa del parametro
 * Llama bin2hex(&first, sizeof(T))
 * @param 
 * @return representacion hexa
 */
template <typename T>
std::string bin2hex(T first){
	return bin2hex(&first, sizeof(T));
}

/** Transforam una representacion hexa en binario.
 * Funcion inversa de @see bin2hex()
 * @param hex[in]: representacion hexa
 * @return binario
 */
std::vector<char> hex2bin(const std::string &hex);

#endif
