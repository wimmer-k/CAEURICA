#ifndef __COMMAND_LINE_INTERFACE
#define __COMMAND_LINE_INTERFACE

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


using namespace std;

class CommandLineInterface
{
public:
  CommandLineInterface();
  ~CommandLineInterface(){};

  //main functions to check all flags from command line
  bool CheckFlags(uint,char*[],const bool& Debug = false);

  //functions to add flags
  void Add(const char*);
  void Add(const char*, const char*, bool*);
  void Add(const char*, const char*, char**);
  void Add(const char*, const char*, string*);
  void Add(const char*, const char*, int*);
  void Add(const char*, const char*, size_t*);
  void Add(const char*, const char*, long long*);
  void Add(const char*, const char*, double*, double factor = 1.);
  void Add(const char*, const char*, vector<char*>*);
  void Add(const char*, const char*, vector<string>*);
  void Add(const char*, const char*, vector<int>*);
  void Add(const char*, const char*, vector<long long>*);
  void Add(const char*, const char*, vector<double>*, double factor = 1.);

  friend ostream& operator <<(ostream &,const CommandLineInterface &);

private:
  uint fMaximumFlagLength;
  vector<string> fFlags;
  vector<void*>  fValues;
  uint fMaximumTypeLength;
  vector<string> fTypes;
  uint fMaximumCommentLength;
  vector<string> fComments;
  vector<double> fFactors;
};

#endif
