#ifndef __TSSORT_H 
#define __TSSORT_H
#include <iostream>
#include <list>
#include "TFile.h"
#include "daq.h"

using namespace std;
struct item
{
  long long TS;
  buffer_type buf;
};

class TSsort{
 public:
  TSsort(FILE *out);
  TSsort(TFile *out);
  bool Add(buffer_type addme);
  bool FullList(item* addme);
  bool Insert(item* insertme);
  void WriteFile(item* writeme);
  void Flush();
  void Status();
  void SetMemDepth(int memdepth){fmemdepth = memdepth;}
 private:
  list<item*> flist;
  //list<item*>::iterator flastit;
  int fevtnr;
  int fdiscarded;
  FILE *ffile;
  TFile *frootfile;
  int fmemdepth;
};


#endif
