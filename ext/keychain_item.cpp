#include "keychain_item.hpp"
#include "cf_utils.hpp"
#include "rice/Hash.hpp"
#include "keychain.hpp"
using namespace Rice;


KeychainItem::KeychainItem(SecKeychainItemRef keychain_item):m_attributes(NULL){
  m_keychain_item = keychain_item;
  CFRetain(m_keychain_item);
}

KeychainItem::KeychainItem(const KeychainItem& copy){
  m_keychain_item = copy.m_keychain_item;
  if(copy.m_attributes){
    m_attributes = CFDictionaryCreateMutableCopy(NULL, CFDictionaryGetCount(copy.m_attributes),copy.m_attributes);
  }
  else{
    m_attributes = NULL;
  }
  CFRetain(m_keychain_item);
}

KeychainItem::~KeychainItem(void){
  CFRelease(m_keychain_item);
  if(m_attributes){
    CFRelease(m_attributes);
  }
}

KeychainItem::KeychainItem(CFDictionaryRef dict){
  m_keychain_item = (SecKeychainItemRef) CFDictionaryGetValue(dict, kSecValueRef);
  CFRetain(m_keychain_item);
  m_attributes = (CFMutableDictionaryRef)CFDictionaryCreateMutableCopy(NULL,CFDictionaryGetCount(dict), dict);
}

CFDateRef KeychainItem::created_at() const {
  return (CFDateRef)CFDictionaryGetValue(m_attributes, kSecAttrCreationDate);
}
CFDateRef KeychainItem::updated_at() const {
  return (CFDateRef)CFDictionaryGetValue(m_attributes, kSecAttrModificationDate);
}
CFStringRef KeychainItem::description() const {
  return (CFStringRef)CFDictionaryGetValue(m_attributes, kSecAttrDescription);
}
CFStringRef KeychainItem::comment() const {
  return (CFStringRef)CFDictionaryGetValue(m_attributes, kSecAttrComment);
}
CFStringRef KeychainItem::account() const {
  return (CFStringRef)CFDictionaryGetValue(m_attributes, kSecAttrAccount);
}
CFStringRef KeychainItem::service() const {
  return (CFStringRef)CFDictionaryGetValue(m_attributes, kSecAttrService);
}
CFStringRef KeychainItem::server() const {
  return (CFStringRef)CFDictionaryGetValue(m_attributes, kSecAttrServer);
}
CFStringRef KeychainItem::security_domain() const {
  return (CFStringRef)CFDictionaryGetValue(m_attributes, kSecAttrSecurityDomain);
}
CFBooleanRef KeychainItem::negative() const {
  return (CFBooleanRef)CFDictionaryGetValue(m_attributes, kSecAttrIsNegative);
}
CFBooleanRef KeychainItem::invisible() const {
  return (CFBooleanRef)CFDictionaryGetValue(m_attributes, kSecAttrIsInvisible);
}
CFStringRef KeychainItem::label() const {
  return (CFStringRef)CFDictionaryGetValue(m_attributes, kSecAttrLabel);
}
CFStringRef KeychainItem::path() const {
  return (CFStringRef)CFDictionaryGetValue(m_attributes, kSecAttrPath);
}
CFNumberRef KeychainItem::port() const {
  return (CFNumberRef)CFDictionaryGetValue(m_attributes, kSecAttrPort);
}

CFNumberRef KeychainItem::protocol() const {
  return (CFNumberRef)CFDictionaryGetValue(m_attributes, kSecAttrProtocol);
}
CFDataRef KeychainItem::password() const {
  return (CFDataRef)CFDictionaryGetValue(m_attributes, kSecValueData);
}
CFStringRef KeychainItem::klass() const {
  return (CFStringRef)CFDictionaryGetValue(m_attributes, kSecClass);
}


void KeychainItem::set_created_at(Object date){
  double abstime = rb_float_value(date.call("to_f"));
  CFDateRef cfdate = CFDateCreate(NULL, abstime);
  CFDictionarySetValue(m_attributes, kSecAttrCreationDate, cfdate);
  CFRelease(cfdate);
}
void KeychainItem::set_updated_at(Object date){
  double abstime = rb_float_value(date.call("to_f"));
  CFDateRef cfdate = CFDateCreate(NULL, abstime);
  CFDictionarySetValue(m_attributes, kSecAttrModificationDate, cfdate);
  CFRelease(cfdate);
}
void KeychainItem::set_description(String r_value){
  CFStringRef value = rb_create_cf_string(r_value);
  CFDictionarySetValue(m_attributes, kSecAttrDescription, value);
  CFRelease(value);
}
void KeychainItem::set_comment(String r_value){
  CFStringRef value = rb_create_cf_string(r_value);
  CFDictionarySetValue(m_attributes, kSecAttrComment, value);
  CFRelease(value);
}
void KeychainItem::set_account(String r_value){
  CFStringRef value = rb_create_cf_string(r_value);
  CFDictionarySetValue(m_attributes, kSecAttrAccount, value);
  CFRelease(value);
}
void KeychainItem::set_service(String r_value){
  CFStringRef value = rb_create_cf_string(r_value);
  CFDictionarySetValue(m_attributes, kSecAttrService, value);
  CFRelease(value);
}
void KeychainItem::set_server(String r_value){
  CFStringRef value = rb_create_cf_string(r_value);
  CFDictionarySetValue(m_attributes, kSecAttrServer, value);
  CFRelease(value);
}
void KeychainItem::set_security_domain(String r_value){
  CFStringRef value = rb_create_cf_string(r_value);
  CFDictionarySetValue(m_attributes, kSecAttrSecurityDomain, value);
  CFRelease(value);
}
void KeychainItem::set_negative(bool value){
  CFDictionarySetValue(m_attributes, kSecAttrIsNegative, value ? kCFBooleanTrue : kCFBooleanFalse);
}
void KeychainItem::set_invisible(bool value){
  CFDictionarySetValue(m_attributes, kSecAttrIsInvisible, value ? kCFBooleanTrue : kCFBooleanFalse);
}
void KeychainItem::set_label(String r_value){
  CFStringRef value = rb_create_cf_string(r_value);
  CFDictionarySetValue(m_attributes, kSecAttrLabel, value);
  CFRelease(value);
}
void KeychainItem::set_path(String r_value){
  CFStringRef value = rb_create_cf_string(r_value);
  CFDictionarySetValue(m_attributes, kSecAttrPath, value);
  CFRelease(value);
}
void KeychainItem::set_port(int port){
  CFNumberRef cf_port = CFNumberCreate(NULL,kCFNumberIntType,&port);
  CFDictionarySetValue(m_attributes, kSecAttrPort, cf_port);
  CFRelease(cf_port);
}

void KeychainItem::set_protocol(SInt32 protocol){
  CFNumberRef value = CFNumberCreate(NULL,kCFNumberSInt32Type,&protocol);
  CFDictionarySetValue(m_attributes, kSecAttrProtocol, value);
  CFRelease(value);
}
void KeychainItem::set_password(String value){
  CFDataRef data = rb_create_cf_data(value);
  CFDictionarySetValue(m_attributes, kSecValueData, data);
  CFRelease(data);
}
void KeychainItem::set_klass(String r_value){
  CFStringRef value = rb_create_cf_string(r_value);
  CFDictionarySetValue(m_attributes, kSecClass, value);
  CFRelease(value);
}

Keychain KeychainItem::keychain() const{
  SecKeychainRef keychain_ref;
  OSStatus result = SecKeychainItemCopyKeychain(m_keychain_item, &keychain_ref);
  Keychain::CheckOSStatusOrRaise(result);
  Keychain keychain = Keychain(keychain_ref);
  CFRelease(keychain_ref);
  return keychain;
}

