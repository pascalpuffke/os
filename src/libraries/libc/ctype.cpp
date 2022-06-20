#include <libc/ctype.h>

int isalpha(int c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
int iscntrl(int c) { return (c >= 0 && c <= 31) || c == 127; }
int isdigit(int c) { return c >= '0' && c <= '9'; }
int isgraph(int c) { return c >= '!' && c <= '~'; }
int islower(int c) { return c >= 'a' && c <= 'z'; }
int isprint(int c) { return c >= ' ' && c <= '~'; }
int isalnum(int c) { return isalpha(c) || isdigit(c); }
int ispunct(int c) { return isgraph(c) && !isalnum(c); }
int isspace(int c) { return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r'; }
int isupper(int c) { return c >= 'A' && c <= 'Z'; }
int isxdigit(int c) { return isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
int tolower(int c) { return isupper(c) ? c + 'a' - 'A' : c; }
int toupper(int c) { return islower(c) ? c + 'A' - 'a' : c; }
