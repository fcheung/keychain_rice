#ifndef __KEYCHAIN_ITEM_H_
#define __KEYCHAIN_ITEM_H_
#include <Security/Security.h>
#include "rice/Class.hpp"
#include "rice/String.hpp"

using namespace Rice;


class KeychainItem{

  public:
    SecKeychainItemRef m_keychain_item;

    KeychainItem(SecKeychainItemRef keychain);
    KeychainItem(const KeychainItem& copy);

    ~KeychainItem(void);

};
#endif