//Jacob Katzeff :: jkatzeff@ucsc.edu :: 1426015
//
//Chris Hsiao :: chhsiao@ucsc.edu :: 1398305
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <cstdlib>

using namespace std;

#include "string_set.h"
#include "auxlib.h"
#include "lyutils.h"
#include "astree.h"
const string cpp_name = "/usr/bin/cpp";
string cpp_command;
int d_flag=0;
char* d_options=nullptr;
const char* filename;
FILE* token_file;

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

void createTokFile(const char* filename){
   string tokOut=basename(const_cast<char*>(filename));
   size_t last= tokOut.find_last_of('.'); 
   tokOut.erase(last+1,2);
   tokOut.append("tok");
   token_file=fopen(tokOut.c_str(), "w");
   if(token_file==NULL)
   {
      fprintf(stderr, "Invalid file open (tok)");
      exit(EXIT_FAILURE);
   }
}
void makeStringSet(const char* filename){
   string out=basename(const_cast<char*>(filename));
   size_t last=out.find_last_of('.');
   out.erase(last+1,2);
   out.append("str");
   FILE* output=fopen(out.c_str(),"w");
   if(output==NULL)
   {
      fprintf(stderr, "Invalid file open (str)");
      exit(EXIT_FAILURE);
   }
   string_set::dump(output);
   fclose(output);
}
void makeASTree(const char* filename){
   string AS=basename(const_cast<char*>(filename));
   size_t last= AS.find_last_of('.'); 
   AS.erase(last+1,2);
   AS.append("ast");
   FILE* ASTout = fopen(AS.c_str(),"w");
   if(ASTout==NULL)
   {
      fprintf(stderr, "Invalid file open (ast)");
      exit(EXIT_FAILURE);
   }
   astree::print(ASTout,yyparse_astree,0);
   fclose(ASTout);
}

void cpp_pclose() {
   int pclose_rc = pclose (yyin);
   eprint_status (cpp_command.c_str(), pclose_rc);
   if (pclose_rc != 0) exit(EXIT_FAILURE);
}

void cpp_popen (const char* filename,int d_flag) {
   if(d_flag)
   {
      cpp_command=cpp_name + " " + " -D" + d_options + " " + filename;
   }
   else
   {
      cpp_command = cpp_name + " " + filename;
   }
   yyin = popen(cpp_command.c_str(), "r");
   if(yyin == NULL)
   {
      syserrprintf (cpp_command.c_str());
   }
   else
   {
      if(yy_flex_debug){
         fprintf(stderr, "-- popen (%s), fileno(yyin) = %d\n",
            cpp_command.c_str(), fileno (yyin));
      }
      lexer::newfilename(cpp_command.c_str());
      //cpplines(yyin,filename);
   }
}


void scan_opts (int argc, char** argv) {
   yy_flex_debug = 0;
   yydebug = 0;
   d_flag=0;
   for(;;) {
      int opt = getopt (argc, argv, "D:@:ly");
      if (opt == EOF) break;
      switch (opt) {
         case '@': set_debugflags (optarg);   break;
         case 'l': yy_flex_debug = 1;         break;
         case 'y': yydebug = 1;               break;
         case 'D': d_flag=1; d_options=optarg;break;
         default:  errprintf ("bad option (%c)\n", optopt); break;
      }
   }
   if (optind > argc) {
      errprintf ("Usage: %s [-ly] [filename]\n",
                 exec::execname.c_str());
      exit (EXIT_FAILURE);
   }
   filename = optind == argc ? "-" : argv[optind];
   const char* extension = strrchr(filename,'.');
   if(extension==NULL || (strcmp(extension,".oc")!=0))
   {
      fprintf(stderr, "Bad file, please use an oc file!\n\n ");
      exit (EXIT_FAILURE);
   }
}
int main(int argc, char** argv)
{
   exec::execname=basename(argv[0]);
   scan_opts(argc,argv);
   cpp_popen (filename,d_flag);
   createTokFile(filename);
   yyparse();
   cpp_pclose();
   fclose(token_file);
   if(exec::exit_status==EXIT_FAILURE){
      return EXIT_FAILURE;
   }
   makeStringSet(filename);
   makeASTree(filename);
   return EXIT_SUCCESS;
}
