#ifndef __COMMON_MD5_H__
#define __COMMON_MD5_H__
#define MD5_BYTE_LENGTH 16

#include <vector>
#include <string>

/** md5_digest: Algoritmo de reduccion criptografico.
 *
 * @param [in] initial_msg mensaje a hacerle el md5
 * @param [out] digest md5 del mensaje
 */
void md5(uint8_t* digest, const uint8_t *initial_msg, size_t initial_len);
void md5(std::vector<char>& out, const std::string& in);
void md5(std::vector<char>& out, const std::vector<char>& in);
void md5Str(std::string& digest, const std::vector<char>& msg);
void md5Str(std::string& digest, const std::string& msg);
#endif

