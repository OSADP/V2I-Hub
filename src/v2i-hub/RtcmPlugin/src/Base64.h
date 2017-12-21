/*
 * Base64.h
 *
 *  Created on: Aug 19, 2014
 *      Author: ivp
 */

#ifndef BASE64_H_
#define BASE64_H_

#include <string>

namespace base64 {
	std::string encode(unsigned char const* , unsigned int len);
	std::string decode(std::string const& s);
}

#endif /* BASE64_H_ */
