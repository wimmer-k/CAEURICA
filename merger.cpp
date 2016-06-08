#include <sys/stat.h>
#include "TSsort.h"
#include <iostream>
#include <algorithm> 
#include <signal.h>
#include <sys/time.h>
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "CommandLineInterface.h"
#include "Merge.h"

using namespace std;
bool signal_received = false;
void signalhandler(int sig){
  if (sig == SIGINT){
    signal_received = true;
  }
}

double get_time(){
    struct timeval t;
    gettimeofday(&t, NULL);
    double d = t.tv_sec + (double) t.tv_usec/1000000;
    return d;
}
int main(int argc, char *argv[]){
  double time_start = get_time();
  signal(SIGINT,signalhandler);
  char *filename =NULL;
  char *outfilename =NULL;
  int maxhits =0;
  int wtree = true;
  int nboards = 2;
  int memdepth =1000;
  //Read in the command line arguments
  CommandLineInterface* interface = new CommandLineInterface();
  interface->Add("-lb", "last buffer to be read", &maxhits);
  interface->Add("-i", "input file base", &filename);
  interface->Add("-o", "output file", &outfilename);
  interface->Add("-m", "memdepth", &memdepth);
  interface->Add("-t", "write tree", &wtree);
  interface->Add("-n", "number of cards", &nboards);
  interface->CheckFlags(argc, argv);

  if(filename==NULL){
    cout << "no inputfile " << endl;
    return 99;
  }
  if(outfilename==NULL){
    cout << "no inputfile " << endl;
    return 99;
  }


  FILE *outFile;
  TFile *rootoutFile;
  if(!wtree){
    cout << "writing .nig file" << endl;
    outFile=fopen(outfilename,"w");
  }
  else{
    cout << "writing .root file" << endl;
    rootoutFile = new TFile(outfilename,"RECREATE");
  }
  Merge merge(filename,nboards,memdepth);
  if(!wtree)
    merge.SetFile(outFile);
  else
    merge.SetRootFile(rootoutFile);
  

  int total = merge.ReadHeaders();
  int ctr=0;
  cout <<"ftimesread[board]"<<"\t"<<"board"<< "\t" << "ch" << "\t" << "TS" <<"\t" << "en" << endl;
  merge.ReadFirst();
  while(!merge.Finished()){
    if(ctr%100000 == 0){
      //cal->PrintCtrs();
      double time_end = get_time();
      cout << setw(5) << setiosflags(ios::fixed) << setprecision(1) << (100.*ctr)/total<<" % done\t" << 
  	(Float_t)ctr/(time_end - time_start) << " events/s " <<
  	(total-ctr)*(time_end - time_start)/(Float_t)ctr << "s to go \r" << flush;
    }
    if(signal_received){
      break;
    }
    merge.Read();
    ctr++;
    if(maxhits>0 && ctr == maxhits)
      break;
  }
  // while(merge.ReadFromFile(0)){}
  // while(merge.ReadFromFile(1)){}
  cout<<"---------------------------------------"<<endl;
  merge.ReadFooters();
  
  cout << "flushing " << endl; 
  cout.flush();
  merge.Flush();

  if(!wtree)
    fclose(outFile);
  else
    rootoutFile->Close();
  double time_end = get_time();
  cout << "Program Run time " << time_end - time_start << " s." << endl;
  cout << "Calculated " << ctr << " events, " << ctr/(time_end - time_start) << " events/s." << endl;  
  return 77;
  
}
