#include "keychain.hpp"
#include "rice/String.hpp"

#include "rice/Builtin_Object.hpp"
#include "rice/Module.ipp"
#include "cf_utils.hpp"
using namespace Rice;
extern Data_Type<Keychain> rb_cKeychain;
void Keychain::CheckOSStatusOrRaise(OSStatus err){
  if(err != 0){
    CFStringRef description = SecCopyErrorMessageString(err, NULL);
    CFIndex bufferSize = CFStringGetMaximumSizeForEncoding(CFStringGetLength(description), kCFStringEncodingUTF8);
    char *buffer = (char*)malloc(bufferSize + 1);
    CFStringGetCString(description, buffer, bufferSize + 1, kCFStringEncodingUTF8);
    CFRelease(description);
    std::string message = std::string(buffer);
    free(buffer);
    throw KeychainException(message, err);
  }
}

Keychain::Keychain(SecKeychainRef keychain){
  m_keychain = keychain;
  CFRetain(keychain);
}

Keychain::Keychain(const Keychain& copy){
  m_keychain = (SecKeychainRef)CFRetain(copy.m_keychain);
}

Keychain::Keychain(const char *path, const Object password_or_nil){
  OSStatus result;
  if(password_or_nil.is_nil()){
    result = SecKeychainCreate(path, 0, NULL, true,NULL,&m_keychain);
  }else{
    String password = password_or_nil;
    result = SecKeychainCreate(path, (UInt32)password.length(), password.c_str(),false,NULL,&m_keychain);
  }
  CheckOSStatusOrRaise(result);
}

Keychain Keychain::create(const String path, const Object password_or_nil){
  SecKeychainRef keychain_ref=NULL;
  OSStatus result;
  if(password_or_nil.is_nil()){
    result = SecKeychainCreate(path.c_str(), 0, NULL, true,NULL,&keychain_ref);
  }else{
    String password = password_or_nil;
    result = SecKeychainCreate(path.c_str(), (UInt32)password.length(), password.c_str(),false,NULL,&keychain_ref);
  }
  CheckOSStatusOrRaise(result);
  Keychain keychain = Keychain(keychain_ref);

  CFRelease(keychain_ref);
  return keychain;
}

Keychain Keychain::open(const String path){
  SecKeychainRef keychain_ref=NULL;
  OSStatus result =SecKeychainOpen(path.c_str(), &keychain_ref);
  CheckOSStatusOrRaise(result);
  Keychain keychain = Keychain(keychain_ref);
  CFRelease(keychain_ref);
  return keychain;
}

Keychain Keychain::default_keychain(){
  SecKeychainRef keychain_ref=NULL;
  OSStatus result =SecKeychainCopyDefault(&keychain_ref);
  CheckOSStatusOrRaise(result);
  Keychain keychain =  Keychain(keychain_ref);
  CFRelease(keychain_ref);
  return keychain;
}
Keychain::~Keychain(void){
  CFRelease(m_keychain);
}

String Keychain::path() const{
  UInt32 pathLength = PATH_MAX;
  char path[PATH_MAX];
  OSStatus result = SecKeychainGetPath(m_keychain, &pathLength, path);
  CheckOSStatusOrRaise(result);
  return path;
}

SecKeychainStatus Keychain::status() const{
  SecKeychainStatus out_status;
  OSStatus result = SecKeychainGetStatus(m_keychain, &out_status);
  CheckOSStatusOrRaise(result);
  return out_status;
}

bool Keychain::locked() const{
  return !(status() & kSecUnlockStateStatus);
}

String Keychain::inspect() const{
  return String::format("<Keychain:%s %s>", path().c_str(), locked() ? "Locked" : "Unlocked");
}

void Keychain::lock(){
  OSStatus result = SecKeychainLock(m_keychain);
  CheckOSStatusOrRaise(result);
}


void Keychain::unlock(const Object password_or_nil){
  OSStatus result = 0;
  if(password_or_nil.is_nil()){
    result = SecKeychainUnlock(m_keychain,0,NULL,false);
  }
  else{
    String password = password_or_nil.call("encode","UTF-8");
    result = SecKeychainUnlock(m_keychain,(UInt32)password.length(), (UInt8*)password.c_str(),true);
  }
  CheckOSStatusOrRaise(result);
}

void Keychain::destroy(){
  OSStatus result = SecKeychainDelete(m_keychain);
  CheckOSStatusOrRaise(result);
}

bool Keychain::lock_on_sleep() const{
  return settings().lockOnSleep;
}

int  Keychain::lock_interval() const{
  return settings().lockInterval;
}

bool Keychain::set_lock_on_sleep(bool newValue){
  SecKeychainSettings current_settings = settings();
  current_settings.lockOnSleep = newValue;
  set_settings(&current_settings);
  return newValue;
}
int Keychain::set_lock_interval(int newValue){
  SecKeychainSettings current_settings = settings();
  current_settings.lockInterval = newValue;
  set_settings(&current_settings);
  return newValue;
}


SecKeychainSettings Keychain::settings() const{
  SecKeychainSettings _settings;
  _settings.version = SEC_KEYCHAIN_SETTINGS_VERS1;
  OSStatus result = SecKeychainCopySettings(m_keychain, &_settings);
  CheckOSStatusOrRaise(result);
  return _settings;
}

void Keychain::set_settings(SecKeychainSettings *new_settings){
  OSStatus result = SecKeychainSetSettings(m_keychain, new_settings);
  CheckOSStatusOrRaise(result);
}

const Hash Keychain::keychain_map(){

  if(!rb_cKeychain.const_defined("KEYCHAIN_MAP")){
    Hash id_to_strings;
    id_to_strings[Symbol("created_at")] = to_ruby((CFStringRef)kSecAttrCreationDate);
    id_to_strings[Symbol("updated_at")] = to_ruby((CFStringRef)kSecAttrModificationDate);
    id_to_strings[Symbol("description")] = to_ruby((CFStringRef)kSecAttrDescription);
    id_to_strings[Symbol("comment")] = to_ruby((CFStringRef)kSecAttrComment);
    id_to_strings[Symbol("account")] = to_ruby((CFStringRef)kSecAttrAccount);
    id_to_strings[Symbol("service")] = to_ruby((CFStringRef)kSecAttrService);
    id_to_strings[Symbol("server")] = to_ruby((CFStringRef)kSecAttrServer);
    id_to_strings[Symbol("port")] = to_ruby((CFStringRef)kSecAttrPort);
    id_to_strings[Symbol("security_domain")] = to_ruby((CFStringRef)kSecAttrSecurityDomain);
    id_to_strings[Symbol("negative")] = to_ruby((CFStringRef)kSecAttrIsNegative);
    id_to_strings[Symbol("invisible")] = to_ruby((CFStringRef)kSecAttrIsInvisible);
    id_to_strings[Symbol("label")] = to_ruby((CFStringRef)kSecAttrLabel);
    id_to_strings[Symbol("path")] = to_ruby((CFStringRef)kSecAttrPath);
    id_to_strings[Symbol("protocol")] = to_ruby((CFStringRef)kSecAttrProtocol);
    id_to_strings[Symbol("password")] = to_ruby((CFStringRef)kSecValueData);
    id_to_strings[Symbol("klass")] = to_ruby((CFStringRef)kSecClass);
    rb_cKeychain.const_set("KEYCHAIN_MAP", id_to_strings);
  }
  return rb_cKeychain.const_get("KEYCHAIN_MAP");
}

void Keychain::map_ruby_options_to_cf_options(const Hash& options, CFMutableDictionaryRef attributes)
{
  Hash ruby_options_to_cf_options = Keychain::keychain_map();

  for( Hash::const_iterator iter = options.begin(); iter != options.end(); iter++){
    Object value = iter->value;
    Object key = ruby_options_to_cf_options[iter->key];
    if(key.is_nil()) continue;

    String string_key = key;

    CFTypeRef cf_value = to_cf(value);
    CFStringRef cf_key = rb_create_cf_string(string_key);
    CFDictionarySetValue(attributes, cf_key, cf_value);
    CFRelease(cf_value);
    CFRelease(cf_key);
  }
}

KeychainItem Keychain::add_password(String kind, Object hash_or_nil){

  Hash options;
  if(!hash_or_nil.is_nil()){
    options = hash_or_nil;
  }

  if(!((Object)options[Symbol("password")]).is_nil())
  {
    String passworddata = String(options[Symbol("password")].value());
    options[Symbol("password")]= passworddata.call("b");
  }


  CFMutableDictionaryRef attributes = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  CFTypeRef cf_kind = rb_create_cf_string(kind);
  CFDictionarySetValue(attributes, kSecClass, cf_kind);
  CFRelease(cf_kind);
  CFDictionarySetValue(attributes, kSecReturnAttributes, kCFBooleanTrue);
  CFDictionarySetValue(attributes, kSecReturnRef, kCFBooleanTrue);
  CFDictionarySetValue(attributes, kSecUseKeychain, m_keychain);

  map_ruby_options_to_cf_options(options, attributes);
  CFDictionaryRef result;


  OSStatus status = SecItemAdd(attributes, (CFTypeRef*)&result);
  CFRelease(attributes);
  CheckOSStatusOrRaise(status);

  KeychainItem item = KeychainItem(result);
  CFRelease(result);
  return item;


}

Object Keychain::find(Symbol first_or_all, String kind, Object hash_or_nil){
  CFMutableDictionaryRef query = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

  CFDictionarySetValue(query, kSecReturnAttributes, kCFBooleanTrue);
  CFDictionarySetValue(query, kSecReturnRef, kCFBooleanTrue);
  

  if(first_or_all.to_id().id() == rb_intern("all")){
    CFDictionarySetValue(query, kSecMatchLimit, kSecMatchLimitAll);
  }

  CFTypeRef cf_kind = rb_create_cf_string(kind);
  CFDictionarySetValue(query, kSecClass, cf_kind);
  CFRelease(cf_kind);

  if(!hash_or_nil.is_nil()){
    Hash options = hash_or_nil;

    if(!((Object)options[Symbol("keychains")]).is_nil()){
      Array keychains_array = options[Symbol("keychains")].value();
      CFMutableArrayRef searchArray = CFArrayCreateMutable(NULL, keychains_array.size(), &kCFTypeArrayCallBacks);
      for(size_t index = 0; index < keychains_array.size(); index++){
        CFArrayAppendValue(searchArray, from_ruby<Keychain>(keychains_array[index]).m_keychain);
      }
      CFDictionarySetValue(query, kSecMatchSearchList,searchArray);
      CFRelease(searchArray);
    }  

    if(!((Object)options[Symbol("limit")]).is_nil()){
      long c_limit = from_ruby<long>(options[Symbol("limit")]);
      CFNumberRef cf_limit = CFNumberCreate(NULL, kCFNumberLongType, &c_limit);
      CFDictionarySetValue(query, kSecMatchLimit, cf_limit);
      CFRelease(cf_limit);
    }

    if(!((Object)options[Symbol("conditions")]).is_nil()){
      Hash conditions = Hash(options[Symbol("conditions")]);
      map_ruby_options_to_cf_options(conditions, query);
    }
  }
  CFDictionaryRef result;

  OSStatus status = SecItemCopyMatching(query, (CFTypeRef*)&result);
  CFRelease(query);

  Array keychain_items;

  switch(status){
    case errSecItemNotFound: 
      break;
    default:
      CheckOSStatusOrRaise(status);
      if(CFArrayGetTypeID() == CFGetTypeID(result)){
        CFArrayRef result_array = (CFArrayRef)result;
        for(CFIndex i = 0; i < CFArrayGetCount(result_array); i++){
          keychain_items.push(KeychainItem((CFDictionaryRef)CFArrayGetValueAtIndex(result_array,i)));
        }
      }
      else{
        keychain_items.push(KeychainItem((CFDictionaryRef)result));
      }
      CFRelease(result);
  }

  if(first_or_all.to_id().id() == rb_intern("first")){
    if(keychain_items.size() > 0)
      return keychain_items[0];
    else
      return Nil;
  }
  else{
    return keychain_items;
  }
}

bool Keychain::equals(const Object other) const{
  if(other.is_a(rb_cKeychain)){
    Keychain other_keychain = from_ruby<Keychain>(other);
    return CFEqual(m_keychain, other_keychain.m_keychain); 
  }else{
    return false;
  }
}

