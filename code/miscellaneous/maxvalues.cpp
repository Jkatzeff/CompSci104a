// $Id: maxvalues.cpp,v 1.3 2016-09-21 17:09:57-07 - - $

#include <iostream>
#include <locale>
#include <string>
using namespace std;

struct comma_numpunct: public numpunct<char> {
   string digits;
   comma_numpunct (const string& dig = "\3"): digits(dig) {}
   virtual char do_thousands_sep() const { return ','; }
   virtual string do_grouping() const { return digits; }
};

template <typename unsigned_t>
void print (const string& type) {
   unsigned_t max = -1LL;
   unsigned long long maxx = max;
   cout << endl << type << ":" << endl;
   cout << "0" << oct << maxx << endl;
   cout << dec << maxx << endl;
   cout << "0x" << uppercase << hex << maxx << endl;
}

#define PRINT(X) print<X> (#X);

int main() {
   cout.imbue (locale (locale(), new comma_numpunct()));
   PRINT (unsigned char);
   PRINT (unsigned short);
   PRINT (unsigned int);
   PRINT (unsigned long);
   PRINT (unsigned long long);
   PRINT (size_t);
   return 0;
}

//TEST// ./maxvalues >maxvalues.lis 2>&1
//TEST// mkpspdf maxvalues.ps maxvalues.cpp* maxvalues.lis

