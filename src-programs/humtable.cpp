//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Mar  8 17:05:00 PST 2011
// Last Modified: Sun Mar 27 13:36:19 PDT 2011 added label hyperlinks
// Last Modified: Sun Mar 27 17:34:54 PDT 2011 added ref record tooltips
// Filename:      ...sig/examples/all/humtable.cpp
// Web Address:   http://sig.sapp.org/examples/museinfo/humdrum/humtable.cpp
// Syntax:        C++; museinfo
//
// Description:   Print Humdrum file as an HTML table.
//

#include "humdrum.h"
#include "PerlRegularExpression.h"

#include <stdlib.h>
#include <string.h>

#ifndef OLDCPP
   #include <iostream>
#else
   #include <iostream.h>
#endif


#define C0C0 "cellpadding=\"0\" cellspacing=\"0\""
#define TDSEP 10
#define TOKENWIDTH 70

// function declarations
void     checkOptions         (Options& opts, int argc, char* argv[]);
void     example              (void);
void     usage                (const char* command);
void     createTable          (ostream& out, HumdrumFile& infile);
void     printGlobalComment   (ostream& out, HumdrumFile& infile, int line);
void     printReferenceRecord (ostream& out, HumdrumFile& infile, int line);
void     printFields          (ostream& out, HumdrumFile& infile, int line);
int      getSubTracks         (HumdrumFile& infile, int line, int track);
void     addLabelHyperlinks   (Array<char>& strang);
void     addLabelHyperlinkName(Array<char>& strang);
ostream& printHtmlPageHeader  (ostream& out);
ostream& printHtmlPageFooter  (ostream& out);
ostream& printHtmlPageFooter  (ostream& out);
ostream& printCss             (ostream& out);

// global variables
Options      options;            // database for command-line arguments
int          fullQ = 1;          // used with --page option
int          classQ = 1;         // used with -C option
int          cssQ   = 0;         // used with --css option
int          textareaQ = 0;      // used with --textarea option


///////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[]) {
   // process the command-line options
   checkOptions(options, argc, argv);
   HumdrumFileSet infiles;
   infiles.read(options);

   if (fullQ) {
      printHtmlPageHeader(cout);
   } else if (cssQ) {
      printCss(cout);
      exit(0);
   }

   for (int i=0; i<infiles.getCount(); i++) {
      createTable(cout, infiles[i]);
   }

   if (fullQ) {
      printHtmlPageFooter(cout);
   }

   return 0;
}


///////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// printCss -- print an example Cascading Style Sheet.
//

ostream& printCss(ostream& out) {
   out << ".humhi:hover {background:#d0d0ff}\n";

   out << "\n/* add a spacing after each cell so that the never touch */\n";
   out << "td {padding-right:10px; }\n";

   out << "\n/* Humdrum data tokens */\n";
   out << ".humdat { color:black; }\n";

   out << "\n/* Humdrum barlines*/\n";
   out << ".humbar { background-color:#eeeeee; }\n";
 
   out << "\n/* Humdrum interpretations */\n";
   out << "\n/* Exclusive interpretations */\n";
   out << ".humexinterp { color:magenta; }\n";
   out << "\n/* Spine manipulators */\n";
   out << ".hummanip { color:#f62217; }\n";
   out << "\n/* other interpretations*/\n";
   out << ".huminterp { color:#9f000f; }\n";

   out << "\n/* Humdrum Comments */\n";
   out << "\n/* Reference records*/\n";
   out << ".humref { color:green; }\n";
   out << "\n/* Global comments*/\n";
   out << ".humgcom { color:blue; }\n";
   out << "\n/* Local comments*/\n";
   out << ".humlcom { color:#7d1b7e; }\n";

   return out;
}



//////////////////////////////
//
// printHtmlPageFooter -- 
//

ostream& printHtmlPageFooter(ostream& out) {
   out << "</body></html>\n";
   return out;
}



//////////////////////////////
//
// printHtmlPageHeader --
//

ostream& printHtmlPageHeader(ostream& out) {
   out << "<html><head><title>full page</title>\n";

   if (cssQ) {
      out << "<style type=\"text/css\">\n";
      out << "a {text-decoration:none}\n";
      printCss(out);
      out << "</style>\n";
   }
   out << "</head><body>\n";
   return out;
}



//////////////////////////////
//
// createTable --
//

void createTable(ostream& out, HumdrumFile& infile) {

   out << "<table " << C0C0 << ">\n";

   int i;
   for (i=0; i<infile.getNumLines(); i++) {
      if (infile[i].isGlobalComment()) {
         printGlobalComment(out, infile, i);
         continue;
      }
      if (infile[i].isBibliographic()) {
         printReferenceRecord(out, infile, i);
         continue;
      }
      if (infile[i].isData()) {
         printFields(out, infile, i);
      }
      if (infile[i].isMeasure()) {
         printFields(out, infile, i);
      }
      if (infile[i].isInterpretation()) {
         printFields(out, infile, i);
      }
   }

   out << "</table>\n";

   if (textareaQ) {
      out << "<textarea wrap=off rows=10 cols=70>";
      out << infile;
      out << "</textarea>\n";
   }

}



//////////////////////////////
//
// getSubTracks --
//

int getSubTracks(HumdrumFile& infile, int line, int track) {
   int& i = line;
   int j;
   int output = 0;
   for (j=0; j<infile[i].getFieldCount(); j++) {
      if (infile[i].getPrimaryTrack(j) == track) {
         output++;
      }
   }

   return output;
}



//////////////////////////////
//
// printFields --
//

void printFields(ostream& out, HumdrumFile& infile, int line) {
   out << "<tr valign=\"baseline\"";
   if (classQ) {
      if (infile[line].isBarline()) {
         out << " class=\"humhi humbar\"";
      } else if (infile[line].isData()) {
         out << " class=\"humhi humdat\"";
      } else if (infile[line].isInterpretation()) {
         if (strncmp(infile[line][0], "**", 2) == 0) {
            out << " class=\"humhi humexinterp\"";
         } else if (infile[line].isSpineManipulator()) {
            out << " class=\"humhi hummanip\"";
         } else {
            out << " class=\"humhi huminterp\"";
         }
      } else if (infile[line].isLocalComment()) {
         out << " class=\"humhi humlcom\"";
      }
   }
   out << ">";
   int& i = line;
   int j;
   int track;
   int counter;
   int subtracks;
   PerlRegularExpression pre;
   Array<char> strang;

   for (track=1; track<=infile.getMaxTracks(); track++) {
      subtracks = getSubTracks(infile, line, track);
      out << "<td>";
      if (subtracks > 1) {
         out << "<table " << C0C0 << "><tr valign=top";
         if (infile[i].isMeasure()) {
            out << " bgcolor=#eeeeee ";
         }
         out << ">";
         counter = 0;
         for (j=0; j<infile[i].getFieldCount(); j++) {
            if (infile[i].getPrimaryTrack(j) == track) {
               strang.setSize(strlen(infile[i][j])+1);
               strcpy(strang.getBase(), infile[i][j]); 
               pre.sar(strang, " ", "&nbsp;", "g");

               // deal with HTML entities, just < and > for now:
               pre.sar(strang, ">", "&gt;", "g");
               pre.sar(strang, "<", "&lt;", "g");

               if (pre.search(strang, "^\\*>.*\\[", "")) {
                  addLabelHyperlinks(strang);
               } else if (pre.search(strang, "^\\*>", "")) {
                  addLabelHyperlinkName(strang);
               }
               out << "<td width=" << TOKENWIDTH << ">" << strang << "</td>";
               counter++;
               if (counter < subtracks) {
                  out << "<td width=" << TDSEP << "></td>";
               }
            }
         }
         out << "</tr></table>\n";
      } else {
         for (j=0; j<infile[i].getFieldCount(); j++) {
            if (infile[i].getPrimaryTrack(j) == track) {
               strang.setSize(strlen(infile[i][j])+1);
               strcpy(strang.getBase(), infile[i][j]); 
               pre.sar(strang, " ", "&nbsp;", "g");

               // deal with HTML entities, just < and > for now:
               pre.sar(strang, ">", "&gt;", "g");
               pre.sar(strang, "<", "&lt;", "g");

               if (pre.search(strang, "^\\*>.*\\[", "")) {
                  addLabelHyperlinks(strang);
               } else if (pre.search(strang, "^\\*>", "")) {
                  addLabelHyperlinkName(strang);
               }
               out << strang;
            }
         }
      }
      out << "</td>";
   }
   out << "</tr>\n";
}



//////////////////////////////
//
// addLabelHyperlinkName --
//

void addLabelHyperlinkName(Array<char>& strang) {
   PerlRegularExpression pre;
   if (!pre.search(strang, "^\\*>([^\\[]+)$", "")) {
      return;
   }

   char buffer[1024] = {0};
   strcpy(buffer, "<a name=\"");
   strcat(buffer, pre.getSubmatch(1));
   strcat(buffer, "\"></a>");
   strcat(buffer, strang.getBase());
   strang.setSize(strlen(buffer)+1);
   strcpy(strang.getBase(), buffer);
}



//////////////////////////////
//
// addLabelHyperlinks --
//

void addLabelHyperlinks(Array<char>& strang) {
   PerlRegularExpression pre;
   if (!pre.search(strang, "^\\*>(.*)\\[(.*)\\]", "")) {
      return;
   }

   char buffer[10123] = {0};
   strcpy(buffer, "*>");
   strcat(buffer, pre.getSubmatch(1));
   strcat(buffer, "[");

   PerlRegularExpression pre2;
   Array<Array<char> > tokens;
   pre2.getTokens(tokens, ",", pre.getSubmatch(2));
   
   int i;
   for (i=0; i<tokens.getSize(); i++) {
      strcat(buffer, "<a href=\"#");
      strcat(buffer, tokens[i].getBase());
      strcat(buffer, "\">");
      strcat(buffer, tokens[i].getBase());
      strcat(buffer, "</a>");
      if (i < tokens.getSize() - 1) {
         strcat(buffer, ",");
      }
   }
   strcat(buffer, "]");
   strang.setSize(strlen(buffer) + 1);
   strcpy(strang.getBase(), buffer);
}


//////////////////////////////
//
// printReferenceRecord --
//

void printReferenceRecord(ostream& out, HumdrumFile& infile, int line) {
   int& i = line;

   PerlRegularExpression pre;
   if (!pre.search(infile[line][0], "^!!!([^:]+):(.*)$", "")) {
      out << "<tr valign=\"baseline\"";
      if (classQ) {
         out << " class=\"humhi humref\"";
      }
      out << "><td colspan=" << infile.getMaxTracks() << ">";
      out << "<font color=green>" << infile[i] << "</font></td></tr>";
      return;
   } 

   Array<char> description;
   char buffer[128] = {0};
   
   infile[line].getBibliographicMeaning(description, pre.getSubmatch(1));
   PerlRegularExpression pre2;
   pre2.sar(description, "\"", "", "g");

   out << "<tr valign=baseline><td colspan=" << infile.getMaxTracks() << ">";
   out << "<span title=\"";
   out << infile[line].getBibKey(buffer);
   out << ": ";
   out << description;
   out << "\">";

   out << "!!!";
   out << pre.getSubmatch(1);
   out << ":";
   out << pre.getSubmatch(2);

   out << "</span>";
   out << "</td></tr>";

}



//////////////////////////////
//
// printGlobalComment --
//

void printGlobalComment(ostream& out, HumdrumFile& infile, int line) {
   int& i = line;
   out << "<tr valign=\"baseline\"";
   if (classQ) {
      out << " class=\"humhi humgcom\"";
   }
   out << "><td colspan=" << infile.getMaxTracks() << ">";
   out << "<font color=blue>" << infile[i] << "</font></td></tr>";
}



//////////////////////////////
//
// checkOptions -- validate and process command-line options.
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("p|page|full=b", "print a full page instead of just table");
   opts.define("C|no-class=b", "don't label elements with classes");
   opts.define("css=b", "Print an example CSS ");
   opts.define("textarea|ta|t=b", "print data in a textarea after main table");

   opts.define("author=b",          "author of program");
   opts.define("version=b",         "compilation info");
   opts.define("example=b",         "example usages");
   opts.define("h|help=b",          "short description");
   opts.process(argc, argv);
   
   // handle basic options:
   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, March 2011" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << argv[0] << ", version: March 2011" << endl;
      cout << "compiled: " << __DATE__ << endl;
      cout << MUSEINFO_VERSION << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   fullQ     =  opts.getBoolean("page");
   classQ    = !opts.getBoolean("no-class");
   cssQ      =  opts.getBoolean("css");
   textareaQ =  opts.getBoolean("textarea");
}



//////////////////////////////
//
// example -- example usage of the humtable program
//

void example(void) {
   cout <<
   "                                                                         \n"
   << endl;
}





//////////////////////////////
//
// usage -- gives the usage statement for the humtable program
//

void usage(const char* command) {
   cout <<
   "                                                                         \n"
   << endl;
}



// md5sum: 04be8d9cd277dd97fbb01f0a1790770e humtable.cpp [20130916]
