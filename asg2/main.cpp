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
constexpr size_t LINESIZE = 1024;
const char* filename;
FILE* token_file;

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

// Run cpp against the lines of the file.
void cpplines (FILE* pipe, const char* filename) {
   int linenr = 1;
   char inputname[LINESIZE];
   string tokOut=basename(const_cast<char*>(filename));
   size_t last= tokOut.find_last_of('.'); 
   tokOut.erase(last+1,2);
   tokOut.append("tok");
   token_file=fopen(tokOut.c_str(), "w");
   if(token_file==NULL)
   {
      fprintf(stderr, "Invalid file open");
   }

   strcpy (inputname, filename);
   int tokenNum;
   int oldFilenr=-1;
   for (;;) {
      int tokenNum=yylex();
      if(oldFilenr!=lexer::lloc.filenr){
         fprintf(token_file, "\n\n# %d \"%s\"\n\n",lexer::lloc.filenr-7,
            (*lexer::filename(lexer::lloc.filenr)).c_str());
         oldFilenr=lexer::lloc.filenr;
      }
      if(tokenNum==YYEOF){
         break;
      }
      char buffer[LINESIZE];
      chomp (buffer, '\n');
      fprintf (token_file,"%d %d.%d %d %s (%s) \n", 
         lexer::lloc.filenr-7, lexer::lloc.linenr, 
         lexer::lloc.offset, tokenNum,
         parser::get_tname(tokenNum),yytext);
   }
}

void cpp_pclose() {
   int pclose_rc = pclose (yyin);
   eprint_status (cpp_command.c_str(), pclose_rc);
   if (pclose_rc != 0) exec::exit_status = EXIT_FAILURE;
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
      exit (exec::exit_status);
   }
   else
   {
      if(yy_flex_debug){
         fprintf(stderr, "-- popen (%s), fileno(yyin) = %d\n",
            cpp_command.c_str(), fileno (yyin));
      }
      lexer::newfilename(cpp_command.c_str());
      cpplines(yyin,filename);
      cpp_pclose();
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
      exit (exec::exit_status);
   }
   filename = optind == argc ? "-" : argv[optind];
   const char* extension = strrchr(filename,'.');
   if(extension==NULL || (strcmp(extension,".oc")!=0))
   {
      exec::exit_status=EXIT_FAILURE;
      fprintf(stderr, "Bad file, please use an oc file!\n\n ");
   }
   cpp_popen (filename,d_flag);
}
int main(int argc, char** argv)
{
   exec::execname=basename(argv[0]);
   scan_opts(argc,argv);
   if(optind>=argc)
   {
      fprintf(stderr, "Bad usage!");
   }
   string out=basename(const_cast<char*>(filename));
   size_t last=out.find_last_of('.');
   out.erase(last+1,2);
   out.append("str");
   FILE *output=fopen(out.c_str(),"w");
   string_set::dump(output);
   fclose(output);
   fclose(token_file);
   return exec::exit_status;
}
