#ifndef __CF_UTILS__
#define __CF_UTILS__
#include <CoreFoundation/CoreFoundation.h>
#include "rice/Class.hpp"
#include "rice/String.hpp"
#include "ruby/encoding.h"

using namespace Rice;

template<> Object to_ruby<CFStringRef>(const CFStringRef & string);
template<> Object to_ruby<CFDateRef>(const CFDateRef & time);
template<> Object to_ruby<CFDataRef>(const CFDataRef &data);
template<> Object to_ruby<CFBooleanRef>(const CFBooleanRef &boolean);
template<> Object to_ruby<CFNumberRef>(const CFNumberRef &number);
template<> Object to_ruby<CFTypeRef>(const CFTypeRef &ref);



CFStringRef rb_create_cf_string(String string);

CFDataRef rb_create_cf_data(String string);


class TypeException : public std::exception{
  public:
    std::string m_message;
    TypeException(std::string message): m_message(message){
    }
    ~TypeException() throw(){};
};

CFTypeRef to_cf(VALUE value);

#endif