#include "cf_utils.hpp"
#include <Security/Security.h>
#include "rice/Hash.hpp"
template<>
  Object to_ruby<CFStringRef>(const CFStringRef & string)
  {
    if(!string){
      return Nil;
    }
    const char * fastBuffer = CFStringGetCStringPtr(string, kCFStringEncodingUTF8);
    if(fastBuffer){
      return String(rb_enc_str_new(fastBuffer, strlen(fastBuffer), rb_utf8_encoding()));
    }else{
      CFIndex bufferLength = CFStringGetMaximumSizeForEncoding(CFStringGetLength(string),kCFStringEncodingUTF8);
      char * buffer = (char*)malloc(bufferLength);
      CFIndex used = 0;
      CFStringGetBytes(string,CFRangeMake(0, CFStringGetLength(string)), kCFStringEncodingUTF8, 0, false, (UInt8*)buffer, bufferLength, &used);
      VALUE rb_string = rb_enc_str_new(buffer, used, rb_utf8_encoding());
      free(buffer);
      return String(rb_string);
    }
  }

template<> Object to_ruby<CFDateRef>(const CFDateRef & time){
  if(!time){
    return Nil;
  }
  CFAbsoluteTime abs_time = CFDateGetAbsoluteTime(time);
  double secondsSinceUnixEpoch = abs_time + kCFAbsoluteTimeIntervalSince1970;
  time_t seconds = (time_t)secondsSinceUnixEpoch;
  long usec = (secondsSinceUnixEpoch - seconds) * 1000000;
  return rb_time_new((time_t)secondsSinceUnixEpoch, usec);
}

template<> Object to_ruby<CFDataRef>(const CFDataRef &data){
  if(!data){
    return Nil;
  }
  return rb_enc_str_new((const char*)CFDataGetBytePtr(data),CFDataGetLength(data), rb_ascii8bit_encoding());
}

template<> Object to_ruby<CFBooleanRef>(const CFBooleanRef &boolean){
  if(!boolean){
    return Nil;
  }
  return CFBooleanGetValue(boolean);
}

template<> Object to_ruby<CFNumberRef>(const CFNumberRef &number){
  if(!number){
    return Nil;
  }
  VALUE rubyValue = Qnil;
  if(CFNumberIsFloatType(number))
  {
    double doubleValue;
    CFNumberGetValue(number, kCFNumberDoubleType, &doubleValue);
    rubyValue = rb_float_new(doubleValue);
  }else{
    long long longValue;
    CFNumberGetValue(number, kCFNumberLongLongType, &longValue);
    rubyValue = LL2NUM(longValue);
  }
  return rubyValue;
}

template<> Object to_ruby<CFTypeRef>(const CFTypeRef &value){
  if(!value){
    return Nil;
  }
  Object rubyValue = Nil;

  if(CFStringGetTypeID() == CFGetTypeID(value)){
    rubyValue = to_ruby<CFStringRef>((CFStringRef)value);
  }
  else if(CFDataGetTypeID() == CFGetTypeID(value)){
    rubyValue = to_ruby<CFDataRef>((CFDataRef)value);
  }
  else if(CFBooleanGetTypeID() == CFGetTypeID(value)){
    rubyValue = to_ruby<CFBooleanRef>((CFBooleanRef)value);
  }
  else if(CFNumberGetTypeID() == CFGetTypeID(value)){
    rubyValue = to_ruby<CFNumberRef>((CFNumberRef)value);
  }
  else if (CFDateGetTypeID() == CFGetTypeID(value)){
    rubyValue = to_ruby<CFDateRef>((CFDateRef)value);
  }
  return rubyValue;
}


CFDataRef rb_create_cf_data(String string){
  if(rb_enc_get(string) == rb_ascii8bit_encoding()){
    return CFDataCreate(NULL, (UInt8*)string.c_str(), string.length());
  }
  else{
    string = rb_str_export_to_enc(string, rb_utf8_encoding());
    return CFDataCreate(NULL, (UInt8*)string.c_str(), string.length());
  }
}


CFStringRef rb_create_cf_string(String string)
{

  VALUE utf_string = rb_str_export_to_enc(string.value(), rb_utf8_encoding());
  char * c_string= StringValueCStr(utf_string);
  return CFStringCreateWithCString(NULL, c_string, kCFStringEncodingUTF8);
}

CFTypeRef to_cf(VALUE value){
  switch(rb_type(value)){
    case T_STRING:
      {
        if(rb_enc_get(value) == rb_ascii8bit_encoding()){
          return rb_create_cf_data(value);
        }
        else{
          return rb_create_cf_string(value);
        }
      }
      break;
    case T_BIGNUM:
    case T_FIXNUM:
      {
        long long longLongValue = NUM2LL(value);
        CFNumberRef numberValue = CFNumberCreate(NULL,kCFNumberLongLongType,&longLongValue);
        return numberValue;
      }
    case T_DATA:
      {
        if(rb_obj_is_kind_of(value, rb_cTime)){
          VALUE floatTime = rb_funcall(value, rb_intern("to_f"),0);
          CFAbsoluteTime abstime = RFLOAT_VALUE(floatTime) - kCFAbsoluteTimeIntervalSince1970;
          return  CFDateCreate(NULL, abstime);
        }
      }
  }

  throw TypeException(rb_obj_classname(value));

}



