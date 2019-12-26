﻿// 默认参数get_word.cpp: Default arguments and Pass-by-reference from Hands-on C++
#include<iostream>
#include<ctype.h>
using namespace std;

int get_word(char *, int &, int start = 0);
int main()
{
int word_len;
char *s = " These words will be printed one-per-line ";
int word_idx = get_word(s,word_len); // line 13
while(word_len > 0)
{
cout.write(s+word_idx, word_len);
cout<<"\n";
word_idx = get_word(s,word_len,word_idx+word_len);
}
return 0;
}

int get_word(char *s, int& size, int start)
{
// Skip initial whitespace
int i = start;
for (; isspace(s[i]); ++i);
int start_of_word = i;
// Traverse word
while (s[i] != '\0' && !isspace(s[i]))
++i;
size = i - start_of_word;
return start_of_word;
}
