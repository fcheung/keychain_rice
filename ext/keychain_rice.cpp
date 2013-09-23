#include "rice/Class.hpp"
#include "rice/Data_Type.hpp"
#include "rice/Constructor.hpp"
#include "rice/String.hpp"
#include "rice/Exception.hpp"
#include "keychain.hpp"
#include "rice/Builtin_Object.hpp"

using namespace Rice;


void handle_keychain_exception(KeychainException const &exception){
  throw Exception(rb_eRuntimeError, "Keychain error %d: %s", exception.m_code,exception.m_message.c_str());
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
        define_method("lock",&Keychain::lock).
        define_method("delete",&Keychain::destroy).
        define_method("unlock", &Keychain::unlock, (Arg("password")=Nil));

}

