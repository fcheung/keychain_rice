#include "keychain.hpp"
#include "rice/String.hpp"

#include "rice/Builtin_Object.hpp"

using namespace Rice;

void Keychain::CheckOSStatusOrRaise(OSStatus err){
  if(err != 0){
    CFStringRef description = SecCopyErrorMessageString(err, NULL);

    CFIndex bufferSize = CFStringGetMaximumSizeForEncoding(CFStringGetLength(description), kCFStringEncodingUTF8);
    char *buffer = (char*)malloc(bufferSize + 1);
    CFStringGetCString(description, buffer, bufferSize + 1, kCFStringEncodingUTF8);
    CFRelease(description);

    throw KeychainException(buffer, err);
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

Keychain Keychain::create(const char *path, const Object password_or_nil){
  SecKeychainRef keychain_ref=NULL;
  OSStatus result;
  if(password_or_nil.is_nil()){
    result = SecKeychainCreate(path, 0, NULL, true,NULL,&keychain_ref);
  }else{
    String password = password_or_nil;
    result = SecKeychainCreate(path, (UInt32)password.length(), password.c_str(),false,NULL,&keychain_ref);
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
    String password = password_or_nil;
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
