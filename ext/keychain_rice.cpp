#include "rice/Class.hpp"
#include "rice/Data_Type.hpp"
#include "rice/Constructor.hpp"
#include "rice/String.hpp"
#include "rice/Exception.hpp"
#include "keychain.hpp"
#include "keychain_item.hpp"
#include "rice/Builtin_Object.hpp"
#include "cf_utils.hpp"
using namespace Rice;

VALUE rb_eKeychainAuthFailedError;
VALUE rb_eKeychainError;
VALUE rb_eKeychainDuplicateItemError;
VALUE rb_eKeychainNoSuchKeychainError;

VALUE exception_class_for_code(SecKeychainStatus code){
  switch(code){
    case errSecAuthFailed:
      return rb_eKeychainAuthFailedError;
    case errSecNoSuchKeychain:
      return rb_eKeychainNoSuchKeychainError;
    case errSecDuplicateItem:
      return rb_eKeychainDuplicateItemError;
    default:
      return rb_eKeychainError;
  }
}

void handle_keychain_exception(KeychainException const &exception){
  Object klass = exception_class_for_code(exception.m_code);
  Object ruby_exception = klass.call("new", exception.m_message, exception.m_code);
  throw Exception(ruby_exception);
}


extern "C"
void Init_keychain_rice(){
  Data_Type<Keychain> rb_cKeychain = 
    define_class<Keychain>("Keychain").
        add_handler<KeychainException>(handle_keychain_exception).
        define_singleton_method("default",&Keychain::default_keychain).
        define_singleton_method("open",&Keychain::open).
        define_singleton_method("create",&Keychain::create,(Arg("path"), Arg("password")=Nil)).
        define_method("path",&Keychain::path).
        define_method("inspect",&Keychain::inspect).
        define_method("lock!",&Keychain::lock).
        define_method("delete",&Keychain::destroy).
        define_method("unlock!", &Keychain::unlock, (Arg("password")=Nil)).
        define_method("status", &Keychain::status).
        define_method("lock_on_sleep?", &Keychain::lock_on_sleep).
        define_method("lock_on_sleep=", &Keychain::set_lock_on_sleep).
        define_method("lock_interval", &Keychain::lock_interval).
        define_method("lock_interval=", &Keychain::set_lock_interval);

  Data_Type<KeychainItem> rb_cKeychainItem = 
    define_class_under<KeychainItem>(rb_cKeychain,"Item");


  define_module_under(rb_cKeychainItem, "Classes").
                const_set("INTERNET",to_ruby((CFStringRef)kSecClassInternetPassword)).
                const_set("GENERIC", to_ruby((CFStringRef)kSecClassGenericPassword));


  rb_eKeychainAuthFailedError = rb_const_get(rb_cKeychain.value(), rb_intern("AuthFailedError"));
  rb_eKeychainError           = rb_const_get(rb_cKeychain.value(), rb_intern("Error"));
  rb_eKeychainNoSuchKeychainError = rb_const_get(rb_cKeychain.value(), rb_intern("NoSuchKeychainError"));
  rb_eKeychainDuplicateItemError = rb_const_get(rb_cKeychain.value(), rb_intern("DuplicateItemError"));
}

