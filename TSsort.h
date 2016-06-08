#ifndef __TSSORT_H 
#define __TSSORT_H
#include <iostream>
#include <list>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "daq.h"

using namespace std;
struct item{
  long long TS;
  buffer_type buf;
};

class TSsort{
 public:
  TSsort();
  TSsort(FILE *out);
  TSsort(TFile *out);
  void SetFile(FILE *out);
  void SetRootFile(TFile *out);
  bool Add(buffer_type addme);
  bool FullList(item* addme);
  bool Insert(item* insertme);
  void WriteFile(item* writeme);
  void PrintList();
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

  long long flastts;

  TTree *ftr;
  int fboard;
  int fch;
  int fen;
  long long fts;
  TH1F* fhtdiff;
};

class TSComparer {
public:
  bool operator() ( item *lhs, item *rhs) {
    return (*rhs).TS < (*lhs).TS;
  }
};

#endif
