#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void memcpy(void*, const void*, size_t);
void memmove(void*, const void*, size_t);
void memset(void*, int, size_t);
int memcmp(const void*, const void*, size_t);

size_t strlen(const char*);
char* strcpy(char*, const char*);
char* strncpy(char*, const char*, size_t);
char* strcat(char*, const char*);
char* strncat(char*, const char*, size_t);
int strcmp(const char*, const char*);
int strncmp(const char*, const char*, size_t);

const void* memchr(const void*, int, size_t);
const char* strchr(const char*, int);
size_t strcspn(const char*, const char*);
const char* strpbrk(const char*, const char*);
const char* strrchr(const char*, int);
size_t strspn(const char*, const char*);
const char* strstr(const char*, const char*);
char* strtok(char*, const char*);

#ifdef __cplusplus
}
#endif
