//
// Created by Alexander on 18-Apr-17.
//

#ifndef NPS_C_TRANSLATOR_KEYVALUEPAIR_H
#define NPS_C_TRANSLATOR_KEYVALUEPAIR_H

template <typename K, typename V>
struct KeyValuePair
{
    KeyValuePair(){}
    KeyValuePair(K *_key, V *_value){key = _key; value = _value;}
    K *key = nullptr;
    V *value = nullptr;
};

#endif //NPS_C_TRANSLATOR_KEYVALUEPAIR_H
