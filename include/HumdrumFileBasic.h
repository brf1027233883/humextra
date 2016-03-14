//
// Copyright 1998-2016 by Craig Stuart Sapp, All Rights Reserved.
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon May 18 13:42:11 PDT 1998
// Last Modified: Wed Nov 29 10:19:28 PST 2000
// Last Modified: Fri Jun 12 22:58:34 PDT 2009 Renamed SigCollection class
// Last Modified: Mon Jun 22 15:03:44 PDT 2009 Added Humdrum/Http URIs
// Last Modified: Mon Feb 14 08:11:01 PST 2011 Added VTS functions
// Last Modified: Tue Apr 24 16:54:50 PDT 2012 Added readFromJrpURI()
// Last Modified: Tue Dec 11 17:23:04 PST 2012 Added fileName, segmentLevel
// Last Modified: Sat Apr 27 13:36:16 PDT 2013 Added changeField()
// Last Modified: Sun Mar 13 17:08:48 PDT 2016 Switched to STL
// Filename:      ...sig/include/sigInfo/HumdrumFileBasic.h
// Web Address:   http://museinfo.sapp.org/include/sigInfo/HumdrumFileBasic.h
// Syntax:        C++ 
//
// Description:   Basic parsing functions for handling a Humdrum data file.
//                Special musical parsing of the Humdrum file is done in
//                the inherited class HumdrumFile.
//

#ifndef _HUMDRUMFILEBASIC_H_INCLUDED
#define _HUMDRUMFILEBASIC_H_INCLUDED

#include "HumdrumRecord.h"

#include <iostream>
#include <vector>

using namespace std;

// the following define is for compiling/not compiling the automatic
// downloading of data from the web inside of the read() functions.
// Comment out the define if your OS doesn't understand the primarily
// GCC/linux based method of downloading with a socket.
#define USING_URI

#ifdef USING_URI
   #include <sys/types.h>   /* socket, connect */
   #include <sys/socket.h>  /* socket, connect */
   #include <netinet/in.h>  /* htons           */
   #include <netdb.h>       /* gethostbyname   */
   #include <unistd.h>      /* read, write     */
   #include <string.h>      /* memcpy          */
   #include <sstream>
#endif


///////////////////////////////////////////////////////////////////////////

class HumdrumFileAddress {
   public:
      HumdrumFileAddress   (void);
      HumdrumFileAddress   (int aLine);
      HumdrumFileAddress   (int aLine, int aField);
      HumdrumFileAddress   (int aLine, int aField, int aSubfield);
      HumdrumFileAddress   (const HumdrumFileAddress& anAddress);
      ~HumdrumFileAddress  ();

      int&               line        (void);
      int&               getLine     (void);
      int&               row         (void);
      int&               getRow      (void);
      int&               i           (void);

      int&               field       (void);
      int&               getField    (void);
      int&               column      (void);
      int&               getColumn   (void);
      int&               col         (void);
      int&               getCol      (void);
      int&               j           (void);

      int&               subfield    (void);
      int&               getSubfield (void);
      int&               subtoken    (void);
      int&               getSubtoken (void);
      int&               k           (void);

      void               setLineField(int aLine, int aField);

      void               zero        (void);
      HumdrumFileAddress operator=   (const HumdrumFileAddress& anAddress);

   protected:
      int address[4];   // segment, line, field, subfield
};

///////////////////////////////////////////////////////////////////////////

class HumdrumFileBasic {
   public:
                             HumdrumFileBasic (void);
                             HumdrumFileBasic (const HumdrumFileBasic& 
                                                 aHumdrumFileBasic);
                             HumdrumFileBasic (const char* filename);
                             HumdrumFileBasic (const string& filename);
                            ~HumdrumFileBasic ();

      void                   appendLine       (const char* aLine);
      void                   appendLine       (HumdrumRecord& aRecord);
      void                   appendLine       (HumdrumRecord* aRecord);
      void                   setAllocation    (int allocation);
      void                   changeField      (HumdrumFileAddress& add,
                                               const char* newField);
      void                   clear            (void);
      void                   setFilename      (const char* filename);
      const char*            getFilename      (void);
      const char*            getFileName      (void) { return getFilename(); }
      void                   setSegmentLevel  (int level = 0);
      int                    getSegmentLevel  (void);
      ostream&               printSegmentLabel(ostream& out);
      ostream&               printNonemptySegmentLabel(ostream& out);
      HumdrumFileBasic       extract          (int aField);
      void                   getNonNullAddress(HumdrumFileAddress& add);
      const char*            getDotValue      (int index, int spinei);
      const char*            getLine          (int index);
      const char*            getBibValue      (char* buffer, const char* key);
      int                    getNumLines      (void);
      HumdrumRecord&         getRecord        (int index);
      int                    getSegmentCount  (void);
      int                    getSpineCount    (int index);
      int                    getTracksByExInterp(vector<int>& tracks, 
                                                  const char* exinterp);
		int                    getKernTracks    (vector<int>& tracks);
      int                    getType          (int index);
      void                   makeVts          (string& vtsstring);
      static void            makeVts          (string& vtsstring,
                                               HumdrumFileBasic& infile);
      void                   makeVtsData      (string& vtsstring);
      static void            makeVtsData      (string& vtsstring,
                                               HumdrumFileBasic& infile);
      HumdrumFileBasic&      operator=        (const HumdrumFileBasic& aFile);
      void                   read             (const char* filename);
      void                   read             (const string& filename);
      void                   read             (istream& inStream);
      HumdrumFileBasic       removeNullRecords(void);
      HumdrumRecord&         operator[]       (int index);
      const char*            operator[]       (HumdrumFileAddress& add);
      void                   write            (const char* filename);
      void                   write            (ostream& outstream);

      // analyses that generate internal data
      void                   analyzeSpines    (void);
      void                   analyzeDots      (void);
      int                    getMaxTracks     (void);
      const char*            getTrackExInterp (int track);

   protected:
      string         fileName;       // storage for input file's name
      int            segmentLevel;   // storage for input file's segment level
      vector<HumdrumRecord*>  records;
      int            maxtracks;      // max exclusive interpretation count
      vector<char*>  trackexinterp;
      static char    empty[1];

   private:
      static int intcompare(const void* a, const void* b);

      // spine analysis functions:
      void       privateSpineAnalysis(void);
      int        predictNewSpineCount(HumdrumRecord& aRecord);
      void       makeNewSpineInfo(vector<char*>&spineinfo, 
                    HumdrumRecord& aRecord, int newsize, int& spineid,
                    vector<int>& ex);
      void       simplifySpineString(char* spinestring);
      void       simplifySpineInfo(vector<char*>& info, int index);

      // determining the meaning of dots (null records)
      void       privateDotAnalysis(void);
      void       readjustDotArrays(vector<int>& lastline, vector<int>& lastspine, 
                       HumdrumRecord& record, int newsize);


      // automatic URI downloading of data in read()
      #ifdef USING_URI
         void     readFromHumdrumURI       (const char* humdrumaddress);
         void     readFromJrpURI           (const char* jrpaddress);
         void     readFromHttpURI          (const char* webaddress);
         int      getChunk                 (int socket_id, stringstream& inputdata, 
                                            char* buffer, int bufsize);
         int      getFixedDataSize         (int socket_id, int datalength, 
                                            stringstream& inputdata, char* buffer, 
                                            int bufsize);
         int      open_network_socket      (const char *hostname, 
                                            unsigned short int port);
         void     prepare_address          (struct sockaddr_in *addr, 
                                            const char *hostname,
                                            unsigned short int port);
      #endif
      public:
         static char* getUriToUrlMapping   (char* buffer, int buffsize, 
                                            const char* uri);

         static void extractEmbeddedDataFromPdf(ostream& outputData, 
                                                istream& inputData);
};


ostream& operator<<(ostream& out, HumdrumFileBasic& aHumdrumFileBasic);

#endif /* _HUMDRUMFILEBASIC_H_INCLUDED */



// md5sum: d38361994fc8639572f7b8d92d5bb5ce HumdrumFileBasic.h [20050403]
