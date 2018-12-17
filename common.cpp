/*
 * common.cpp
 *
 *  Created on: 3 Sep 2018
 *      Author: simon
 */

#include "common.h"
#include <string.h>
#include <stdlib.h>

extern "C" void *memset(void *block, int c, size_t size)
{
	char *p = (char *)block;
	for (size_t count = 0; count < size; count++)
		p[count] = c;

	return block;
}

extern "C" void *memcpy(void *to, const void *from, size_t size)
{
	char *pDest = (char *)to;
	char *pSrc = (char *)from;

	for (size_t count = 0; count < size; count++)
		pDest[count] = pSrc[count];

	return to;
}

extern "C" size_t strlen(const char *s)
{
	size_t count = 0;

	while (*s++ != 0)
		count++;

	return count;
}

//https://stackoverflow.com/questions/34873209/implementation-of-strcmp/34873406
extern "C" int strcmp(const char *s1, const char *s2)
{
    while (1)
    {
        int res = ((*s1 == 0) || (*s1 != *s2));
        if  (__builtin_expect((res),0))
        {
            break;
        }
        ++s1;
        ++s2;
    }
    return (*s1 - *s2);
}

//https://www.harishnote.com/2016/06/c-programming-78-strchr-implementation.html
const char *strchr(const char *s, int c)
{
   while(*s != c && *s != '\0') {
      s++;
   }
   if(*s == c) {
      return s;
   }else {
      return NULL;
   }
}

//https://codereview.stackexchange.com/questions/35396/strstr-implementation
char* strstr(char *haystack, const char *needle) {
    if (haystack == NULL || needle == NULL) {
        return NULL;
    }

    for ( ; *haystack; haystack++) {
        // Is the needle at this point in the haystack?
        const char *h, *n;
        for (h = haystack, n = needle; *h && *n && (*h == *n); ++h, ++n) {
            // Match is progressing
        }
        if (*n == '\0') {
            // Found match!
            return haystack;
        }
        // Didn't match here.  Try again further along haystack.
    }
    return NULL;
}
