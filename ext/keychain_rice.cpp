#include "rice/Class.hpp"
#include "rice/Data_Type.hpp"
#include "rice/Constructor.hpp"
#include "rice/String.hpp"
#include "rice/Exception.hpp"
#include "keychain.hpp"
#include "keychain_item.hpp"
#include "rice/Builtin_Object.hpp"
#include "cf_utils.hpp"
#include <CoreFoundation/CoreFoundation.h>
using namespace Rice;

VALUE rb_eKeychainAuthFailedError;
VALUE rb_eKeychainError;
VALUE rb_eKeychainDuplicateItemError;
VALUE rb_eKeychainNoSuchKeychainError;
Data_Type<Keychain> rb_cKeychain;

static VALUE exception_class_for_code(SecKeychainStatus code){
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

static void handle_keychain_exception(KeychainException const &exception){
  Object klass = exception_class_for_code(exception.m_code);
  Object ruby_exception = klass.call("new", exception.m_message, exception.m_code);
  throw Exception(ruby_exception);
}

static void handle_type_exception(TypeException const &exception){
  throw Exception(rb_eRuntimeError, "Couldn't convert type %s",exception.m_message.c_str());
}

static void build_protocols(void){
  Module protocols = define_module_under(rb_cKeychain, "Protocols");
  protocols.const_set("FTP", to_ruby((UInt32)kSecProtocolTypeFTP       ));
  protocols.const_set("FTPAccount", to_ruby((UInt32)kSecProtocolTypeFTPAccount));
  protocols.const_set("HTTP", to_ruby((UInt32)kSecProtocolTypeHTTP      ));
  protocols.const_set("IRC", to_ruby((UInt32)kSecProtocolTypeIRC       ));
  protocols.const_set("NNTP", to_ruby((UInt32)kSecProtocolTypeNNTP      ));
  protocols.const_set("POP3", to_ruby((UInt32)kSecProtocolTypePOP3      ));
  protocols.const_set("SMTP", to_ruby((UInt32)kSecProtocolTypeSMTP      ));
  protocols.const_set("SOCKS", to_ruby((UInt32)kSecProtocolTypeSOCKS     ));
  protocols.const_set("IMAP", to_ruby((UInt32)kSecProtocolTypeIMAP      ));
  protocols.const_set("LDAP", to_ruby((UInt32)kSecProtocolTypeLDAP      ));
  protocols.const_set("AppleTalk", to_ruby((UInt32)kSecProtocolTypeAppleTalk ));
  protocols.const_set("AFP", to_ruby((UInt32)kSecProtocolTypeAFP       ));
  protocols.const_set("Telnet", to_ruby((UInt32)kSecProtocolTypeTelnet    ));
  protocols.const_set("SSH", to_ruby((UInt32)kSecProtocolTypeSSH       ));
  protocols.const_set("FTPS", to_ruby((UInt32)kSecProtocolTypeFTPS      ));
  protocols.const_set("HTTPS", to_ruby((UInt32)kSecProtocolTypeHTTPS     ));
  protocols.const_set("HTTPProxy", to_ruby((UInt32)kSecProtocolTypeHTTPProxy ));
  protocols.const_set("HTTPSProxy", to_ruby((UInt32)kSecProtocolTypeHTTPSProxy));
  protocols.const_set("FTPProxy ", to_ruby((UInt32)kSecProtocolTypeFTPProxy  ));
  protocols.const_set("CIFS", to_ruby((UInt32)kSecProtocolTypeCIFS      ));
  protocols.const_set("SMB", to_ruby((UInt32)kSecProtocolTypeSMB       ));
  protocols.const_set("RTSP ", to_ruby((UInt32)kSecProtocolTypeRTSP      ));
  protocols.const_set("RTSPProxy", to_ruby((UInt32)kSecProtocolTypeRTSPProxy ));
  protocols.const_set("DAAP", to_ruby((UInt32)kSecProtocolTypeDAAP      ));
  protocols.const_set("EPPC", to_ruby((UInt32)kSecProtocolTypeEPPC      ));
  protocols.const_set("IPP", to_ruby((UInt32)kSecProtocolTypeIPP       ));
  protocols.const_set("NNTPS", to_ruby((UInt32)kSecProtocolTypeNNTPS     ));
  protocols.const_set("LDAPS", to_ruby((UInt32)kSecProtocolTypeLDAPS     ));
  protocols.const_set("TelnetS", to_ruby((UInt32)kSecProtocolTypeTelnetS   ));
  protocols.const_set("IMAPS", to_ruby((UInt32)kSecProtocolTypeIMAPS     ));
  protocols.const_set("IRCS", to_ruby((UInt32)kSecProtocolTypeIRCS      ));
  protocols.const_set("POP3S", to_ruby((UInt32)kSecProtocolTypePOP3S     ));
  protocols.const_set("CVSpserver", to_ruby((UInt32)kSecProtocolTypeCVSpserver));
  protocols.const_set("SVN", to_ruby((UInt32)kSecProtocolTypeSVN       ));
  protocols.const_set("ANY", to_ruby((UInt32)kSecProtocolTypeAny       ));
}


extern "C"
void Init_keychain_rice(){
  Data_Type<Keychain> rb_cKeychain = 
    define_class<Keychain>("Keychain").
        add_handler<KeychainException>(handle_keychain_exception).
        add_handler<TypeException>(handle_type_exception).
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
        define_method("lock_interval=", &Keychain::set_lock_interval).
        define_method("add_password",&Keychain::add_password,(Arg("kind"),Arg("options")=Nil)).
        define_method("==", &Keychain::equals).
        define_singleton_method("find",&Keychain::find,(Arg("first_or_all"),Arg("kind"),Arg("options")=Nil));

  Data_Type<KeychainItem> rb_cKeychainItem = 
    define_class_under<KeychainItem>(rb_cKeychain,"Item").
      define_method("created_at",&KeychainItem::created_at).
      define_method("updated_at",&KeychainItem::updated_at).
      define_method("description",&KeychainItem::description).
      define_method("comment",&KeychainItem::comment).
      define_method("protocol",&KeychainItem::created_at).
      define_method("label",&KeychainItem::label).
      define_method("server",&KeychainItem::server).
      define_method("service",&KeychainItem::service).
      define_method("port",&KeychainItem::port).
      define_method("path",&KeychainItem::path).
      define_method("account",&KeychainItem::account).
      define_method("security_domain",&KeychainItem::security_domain).
      define_method("negative",&KeychainItem::negative).
      define_method("invisible",&KeychainItem::invisible).
      define_method("klass",&KeychainItem::klass).
      define_method("password",&KeychainItem::password).
      define_method("created_at=",&KeychainItem::set_created_at).
      define_method("updated_at=",&KeychainItem::set_updated_at).
      define_method("description=",&KeychainItem::set_description).
      define_method("comment=",&KeychainItem::set_comment).
      define_method("protocol=",&KeychainItem::set_created_at).
      define_method("label=",&KeychainItem::set_label).
      define_method("server=",&KeychainItem::set_server).
      define_method("service=",&KeychainItem::set_service).
      define_method("port=",&KeychainItem::set_port).
      define_method("path=",&KeychainItem::set_path).
      define_method("account=",&KeychainItem::set_account).
      define_method("security_domain=",&KeychainItem::set_security_domain).
      define_method("negative=",&KeychainItem::set_negative).
      define_method("invisible=",&KeychainItem::set_invisible).
      define_method("klass=",&KeychainItem::set_klass).
      define_method("password=",&KeychainItem::set_password);



  define_module_under(rb_cKeychainItem, "Classes").
                const_set("INTERNET",to_ruby((CFStringRef)kSecClassInternetPassword)).
                const_set("GENERIC", to_ruby((CFStringRef)kSecClassGenericPassword));

  build_protocols();
  Keychain::keychain_map();

  rb_eKeychainAuthFailedError = rb_const_get(rb_cKeychain.value(), rb_intern("AuthFailedError"));
  rb_eKeychainError           = rb_const_get(rb_cKeychain.value(), rb_intern("Error"));
  rb_eKeychainNoSuchKeychainError = rb_const_get(rb_cKeychain.value(), rb_intern("NoSuchKeychainError"));
  rb_eKeychainDuplicateItemError = rb_const_get(rb_cKeychain.value(), rb_intern("DuplicateItemError"));
}

