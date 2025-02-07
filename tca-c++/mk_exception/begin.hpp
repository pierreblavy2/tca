/*
 * mk_exception.hpp
 *
 *  Created on: May 5, 2015
 *      Author: pierre
 */

#include <exception>
#include <string>


#ifndef MK_EXCEPTION_BASE
#define MK_EXCEPTION_BASE(name)\
	struct name : virtual std::exception{\
    std::string msg;\
    virtual ~name(){}\
    explicit name(const char *s):msg(s){}\
    explicit name(const std::string &s):msg(s){}\
    const char*what() const throw() override{return msg.c_str();}\
	protected:\
	name(){}\
};
#endif


#ifndef MK_EXCEPTION
#define MK_EXCEPTION(name,base)\
	struct name : virtual base{\
		typedef base base_t;\
		explicit name(const std::string &s){msg=s;}\
		explicit name(const char *s){msg=s;}\
		const char*what() const throw() override{return msg.c_str();}\
		protected:\
		name(){}\
	};
#endif
