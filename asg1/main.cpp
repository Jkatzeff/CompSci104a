#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <libgen.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

using namespace std;


#include "string_set.h"
#include "auxlib.h"

const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;

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
   strcpy (inputname, filename);
   for (;;) {
      char buffer[LINESIZE];
      char* fgets_rc = fgets (buffer, LINESIZE, pipe);
      if (fgets_rc == NULL) break;
      chomp (buffer, '\n');
      printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
      // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, inputname);
      if (sscanf_rc == 2) {
         printf ("DIRECTIVE: line %d file \"%s\"\n", linenr, inputname);
         continue;
      }
      char* savepos = NULL;
      char* bufptr = buffer;
      for (int tokenct = 1;; ++tokenct) {
         char* token = strtok_r (bufptr, " \t\n", &savepos);
         bufptr = NULL;
         if (token == NULL) break;
         string_set::intern(token);
      }
      ++linenr;
   }
}


int main (int argc, char** argv) {
   const char* execname = basename (argv[0]);
   int exit_status = EXIT_SUCCESS;
   int yy_flex_debug;
   int yydebug;
   printf("%s", argv[argc-1]);
   for (int argi = 1; argi < argc; ++argi) {
      char* filename = argv[argc-1];
      string fname = string(filename);
      string command = CPP + " " + filename;
      size_t dot_pos = fname.rfind('.', strlen(filename));
      if(dot_pos == string::npos || fname.substr(dot_pos, strlen(filename)-dot_pos) != ".oc"){
      	fprintf(stderr, "%s is an invalid file type.", filename);
      	return -1;
      } else {
      	for(;;){
      		int opt = getopt(argc, argv, "@:lyD");
      		if (opt == EOF) break;
      		switch (opt) {
      			case '@': set_debugflags(optarg); break;
      			case 'D': command = CPP + " -D" + optarg + " " + filename; break;
      			case 'l': yy_flex_debug = 1; break;
      			case 'y': yydebug = 1; break;
      			default: fprintf(stderr, "bad option (%c)\n", optopt); break;
      			}
      		}
      	}
      
      printf ("command=\"%s\"\n", command.c_str());
      FILE* pipe = popen (command.c_str(), "r");
      if (pipe == NULL) {
         exit_status = EXIT_FAILURE;
         fprintf (stderr, "%s: %s: %s\n",
                  execname, command.c_str(), strerror (errno));
      }else {
         cpplines (pipe, filename);
         int pclose_rc = pclose (pipe);
         eprint_status (command.c_str(), pclose_rc);
         if (pclose_rc != 0) exit_status = EXIT_FAILURE;
      }
      FILE* outfile;
      string write_to = string(filename).substr(0, strlen(filename)-3);
      write_to += ".str";
      outfile = fopen(write_to.c_str(), "w");
      string_set::dump(outfile); //change to output file
      fclose(outfile);
   }
   return exit_status;
}