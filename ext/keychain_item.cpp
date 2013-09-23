#include "keychain_item.hpp"

using namespace Rice;


KeychainItem::KeychainItem(SecKeychainItemRef keychain_item){
  m_keychain_item = keychain_item;
  CFRetain(m_keychain_item);
}

KeychainItem::KeychainItem(const KeychainItem& copy){
  m_keychain_item = copy.m_keychain_item;
  CFRetain(m_keychain_item);
}

KeychainItem::~KeychainItem(void){
  CFRelease(m_keychain_item);
}
