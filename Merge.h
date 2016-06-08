#ifndef __MERGE_H 
#define __MERGE_H
#include <iostream>
#include <vector>
#include <limits.h>
#include "TFile.h"
#include "daq.h"
#include "TSsort.h"
#include <algorithm>

using namespace std;

class Merge{
 public:
  Merge(char *filename, int nboards, int memdepth);
  int ReadHeaders();
  void ReadFooters();
  void SetFile(FILE *out);
  void SetRootFile(TFile *out);
  void ReadFirst();
  void Read();
  bool Finished(){return fnboards == ffinishedsum;};
  bool ReadFromFile(int board);
  void WriteFile(item* writeme);
  void Flush();
 private:
  char * ffilename;
  int fnboards;
  vector<FILE*> finFiles;
  vector<int> ffilesize;
  vector<int> ftimesread;
  vector<int> ffinished;
  int ffinishedsum;
  FILE *ffile;
  TFile *frootfile;

  vector<long long> fyoungestTS;
  int fyoungestboard;
  vector<item*> fitems;

  TTree *ftr;
  int fboard;
  int fch;
  int fen;
  long long fts;
};
#endif
