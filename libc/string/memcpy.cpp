#include <string.h>

void *memcpy(void *str1, const void *str2, size_t n)
{
    char* c_str1 = (char*)str1;
    const char* c_str2 = (const char*)str2;

    for(size_t i = 0; i < n; i++)
    {
        c_str1[i] = c_str2[i];
    }

    return str1;
}