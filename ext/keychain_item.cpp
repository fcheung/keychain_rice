#include "keychain_item.hpp"
#include "cf_utils.hpp"
#include "rice/Hash.hpp"
#include "keychain.hpp"
using namespace Rice;


KeychainItem::KeychainItem(SecKeychainItemRef keychain_item):m_attributes(NULL),m_unsaved_password(NULL){
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
  m_unsaved_password = copy.m_unsaved_password;
  if(m_unsaved_password){
    CFRetain(m_unsaved_password);
  }
}

KeychainItem::~KeychainItem(void){
  CFRelease(m_keychain_item);
  if(m_attributes){
    CFRelease(m_attributes);
  }
  if(m_unsaved_password){
    CFRelease(m_unsaved_password);
  }
}

KeychainItem::KeychainItem(CFDictionaryRef dict): m_unsaved_password(NULL){
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
  if(m_unsaved_password){CFRelease(m_unsaved_password);}
  m_unsaved_password  = rb_create_cf_data(value);
}
void KeychainItem::set_klass(String r_value){
  CFStringRef value = rb_create_cf_string(r_value);
  CFDictionarySetValue(m_attributes, kSecClass, value);
  CFRelease(value);
}

CFDataRef KeychainItem::password() const {
  if(m_unsaved_password){
    return m_unsaved_password;
  }else{
    void *data;
    UInt32 dataLength;
    OSStatus result = SecKeychainItemCopyAttributesAndData(m_keychain_item, NULL , NULL, NULL, &dataLength, &data);
    Keychain::CheckOSStatusOrRaise(result);

    CFDataRef cf_data = CFDataCreate(NULL, (UInt8*)data, dataLength);
    SecKeychainItemFreeAttributesAndData(NULL,data);
    return cf_data;
  }
}
CFMutableDictionaryRef KeychainItem::sec_query_identifying_self() const{
  CFMutableDictionaryRef query = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  CFArrayRef itemArray = CFArrayCreate(NULL, (const void**)&m_keychain_item, 1, &kCFTypeArrayCallBacks);
  CFDictionarySetValue(query, kSecMatchItemList, itemArray);
  CFRelease(itemArray);
  return query;
}


CFStringRef KeychainItem::copy_class() const{
  SecItemClass secItemClass;
  SecKeychainItemCopyContent(m_keychain_item, &secItemClass, NULL, NULL, NULL);
  secItemClass = CFSwapInt32HostToBig(secItemClass);
  CFStringRef cfclass = CFStringCreateWithBytes(NULL, (UInt8*)&secItemClass, sizeof(secItemClass), kCFStringEncodingUTF8, false);
  return cfclass;
}
void KeychainItem::save(){
  CFMutableDictionaryRef query = sec_query_identifying_self();

  CFMutableDictionaryRef attributes = NULL;
  if(m_attributes){
    attributes = CFDictionaryCreateMutableCopy(NULL, 0, m_attributes);
  }else{
    attributes = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  }
  CFDictionaryRemoveValue(attributes, kSecValueRef);
  CFDictionaryRemoveValue(attributes, kSecClass);
  CFDictionaryRemoveValue(attributes, kSecAttrModificationDate);
  CFDictionaryRemoveValue(attributes, kSecAttrCreationDate);
  if(m_unsaved_password){
    CFDictionarySetValue(attributes,kSecValueData,m_unsaved_password);
  }
  CFStringRef cfclass = copy_class();
  CFDictionarySetValue(query, kSecClass, cfclass);
  CFRelease(cfclass);
  OSStatus result = SecItemUpdate(query, attributes);

   CFRelease(query);
   CFRelease(attributes);
  Keychain::CheckOSStatusOrRaise(result);
}

Keychain KeychainItem::keychain() const{
  SecKeychainRef keychain_ref;
  OSStatus result = SecKeychainItemCopyKeychain(m_keychain_item, &keychain_ref);
  Keychain::CheckOSStatusOrRaise(result);
  Keychain keychain = Keychain(keychain_ref);
  CFRelease(keychain_ref);
  return keychain;
}

void KeychainItem::destroy(){
  OSStatus result = SecKeychainItemDelete(m_keychain_item);
  Keychain::CheckOSStatusOrRaise(result);
}
