//
// Created by Alexander on 18-Apr-17.
//

#ifndef NPS_C_TRANSLATOR_KEYVALUEPAIR_H
#define NPS_C_TRANSLATOR_KEYVALUEPAIR_H

template <typename K, typename V>
struct KeyValuePair
{
    K* key;
    V* value;
};

#endif //NPS_C_TRANSLATOR_KEYVALUEPAIR_H
