#ifndef MEM_H
#define MEM_H
#include <stdlib.h>
void* malloc(unsigned nSize);
void free(void* pBlock);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memcpy2(void *dest, const void *src, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);
char *strcpy(char *dest, const char* src);
char *strncpy(char *dest, const char *src, size_t n);
size_t strlen(const char *s);
int rand();

#define PI1

#ifdef PI1
#define GPIO_BASE_ADDR 0x20200000
#define MAIL_ADDR 0x2000B880
#define UART_BASE_ADDR 0x20201000

#endif

#ifndef PI1
#define GPIO_BASE_ADDR 0x3F200000
#define MAIL_ADDR 0x3F00B880
#define UART_BASE_ADDR 0x3F201000
#endif
#endif
