// $Id: hashdispatch.cpp,v 1.4 2016-11-14 17:07:15-08 - - $

#include <iostream>
#include <stdio.h>
#include <unordered_map>
using namespace std;

void add (void) { printf ("%s\n", __PRETTY_FUNCTION__); }
void sub (void) { printf ("%s\n", __PRETTY_FUNCTION__); }
void mul (void) { printf ("%s\n", __PRETTY_FUNCTION__); }
void div (void) { printf ("%s\n", __PRETTY_FUNCTION__); }

using fn = void (*) ();
unordered_map<char,fn> dispatch = {
   {'+', add},
   {'-', sub},
   {'*', mul},
   {'/', div},
};

int main() {
   char ops[] = "+-*/";
   for (int i = 0; ops[i]; ++i) {
      fn f = dispatch[int(ops[i])];
      cout << reinterpret_cast<void*> (f) << " ";
      f();
   }
}

