#ifndef __KEYCHAIN_ITEM_H_
#define __KEYCHAIN_ITEM_H_
#include <Security/Security.h>
#include "rice/Class.hpp"
#include "rice/String.hpp"

using namespace Rice;

class Keychain;

class KeychainItem{

  public:
    SecKeychainItemRef m_keychain_item;
    CFMutableDictionaryRef m_attributes;
    CFDataRef m_unsaved_password;

    KeychainItem(SecKeychainItemRef keychain);
    KeychainItem(const KeychainItem& copy);
    KeychainItem(CFDictionaryRef data);
    ~KeychainItem(void);

    void destroy();
    Keychain keychain() const;
    CFDateRef created_at() const;
    CFDateRef updated_at() const;
    CFStringRef description() const;
    CFStringRef comment() const;
    CFStringRef service() const;
    CFStringRef account() const;
    CFStringRef server() const;
    CFStringRef security_domain() const;
    CFBooleanRef negative() const;
    CFBooleanRef invisible() const;
    CFStringRef path() const;
    CFStringRef label() const;
    CFDataRef password() const;
    CFNumberRef protocol() const;
    CFNumberRef port() const;
    CFStringRef klass() const;
    
    void set_created_at(Object date);
    void set_updated_at(Object date);
    void set_description(String);
    void set_comment(String);
    void set_service(String);
    void set_account(String);
    void set_server(String);
    void set_security_domain(String);
    void set_negative(bool);
    void set_invisible(bool);
    void set_path(String);
    void set_label(String);
    void set_password(String);
    void set_protocol(SInt32);
    void set_port(int);
    void set_klass(String);

};
#endif