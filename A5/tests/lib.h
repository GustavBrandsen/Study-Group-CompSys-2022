#ifndef __LIB_H__
#define __LIB_H__

char inp();
void outp(char);
void terminate();
void print_string(const char* p);
void read_string(char* buffer, int max_chars);
unsigned int str_to_uns(const char* str);
int uns_to_str(char* buffer, unsigned int val);

#endif
