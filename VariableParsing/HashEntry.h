//
// Created by user on 24.03.2017.
//

#ifndef NPS_C_TRANSLATOR_HASHENTRY_H
#define NPS_C_TRANSLATOR_HASHENTRY_H

#include "HashTable.h"

template <class K, class V>
class HashEntry
{
private:
    K *key;
    V *value;

public:
    K *getKey() const {
        return key;
    }

    void setKey(K *key) {
        this->key = key;
    }

    V *getValue() const {
        return value;
    }

    void setValue(V *value) {
        this->value = value;
    }


};

#endif //NPS_C_TRANSLATOR_HASHENTRY_H
