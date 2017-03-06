#ifndef __STRING_HELPERS_H_INCLUDED__
#define __STRING_HELPERS_H_INCLUDED__

char* copy_string(const char *s);
bool compare_strings(const char *s1, const char *s2);
const char* trim_start(const char *s, int &length);

#endif  // __STRING_HELPERS_H_INCLUDED__
