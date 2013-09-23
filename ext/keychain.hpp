#ifndef __KEYCHAIN_H_
#define __KEYCHAIN_H_
#include <Security/Security.h>
#include "rice/Class.hpp"
#include "rice/String.hpp"
#include "rice/Builtin_Object.hpp"

using namespace Rice;

class KeychainException : public std::exception{
  public:
    std::string m_message;
    int m_code;
    KeychainException(std::string message, int code): m_message(message), m_code(code){
    }
    ~KeychainException() throw(){};
};

class Keychain{

  public:
    SecKeychainRef m_keychain;

    Keychain(SecKeychainRef keychain);
    Keychain(const Keychain& copy);
    Keychain(const char * path, const Object password_or_nil);

    ~Keychain(void);

    static void CheckOSStatusOrRaise(OSStatus err);
    static Keychain open(String path);
    static Keychain default_keychain();
    static Keychain create(const char *path, const Object password_or_nil);

    String path() const;
    SecKeychainStatus status() const;
    bool locked() const;
    String inspect() const;
    void lock();
    void unlock(const Object password_or_nil);
    void destroy();

    bool lock_on_sleep() const;
    int  lock_interval() const;
    bool set_lock_on_sleep(bool newValue);
    int set_lock_interval(int newValue);
  protected:
    SecKeychainSettings settings() const;
    void set_settings(SecKeychainSettings *new_settings);
};
#endif