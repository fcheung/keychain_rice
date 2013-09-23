#include <CoreFoundation/CoreFoundation.h>
#include "rice/Class.hpp"
#include "rice/String.hpp"
#include "ruby/encoding.h"

template<>
  Object to_ruby<CFStringRef>(const CFStringRef & string)
  {
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