// $Id: arraydispatch.c,v 1.16 2016-11-14 17:02:46-08 - - $

#include <stdio.h>

void add (void) { printf ("%s\n", __PRETTY_FUNCTION__); }
void sub (void) { printf ("%s\n", __PRETTY_FUNCTION__); }
void mul (void) { printf ("%s\n", __PRETTY_FUNCTION__); }
void div (void) { printf ("%s\n", __PRETTY_FUNCTION__); }

typedef void (*fn) (void);
fn dispatch[256] = {
   ['+'] add,
   ['-'] sub,
   ['*'] mul,
   ['/'] div,
};

int main() {
   char ops[] = "+-*/";
   for (int i = 0; ops[i]; ++i) {
      fn f = dispatch[(int)ops[i]];
      printf ("%p ", f);
      f();
   }
}

