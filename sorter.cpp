#include <sys/stat.h>
#include "TSsort.h"
#include <iostream>
#include <algorithm> 
#include <signal.h>
#include <sys/time.h>
#include"TFile.h"
#include"TTree.h"
#include"TROOT.h"
#include"TH1.h"
#include"TH2.h"
#include"CommandLineInterface.h"

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
  char *filename;
  char *outfilename;
  int maxblocks =0;
  int memdepth =1000;
  bool wtree = true;
  //Read in the command line arguments
  CommandLineInterface* interface = new CommandLineInterface();
  interface->Add("-lb", "last buffer to be read", &maxblocks);
  interface->Add("-i", "input file", &filename);
  interface->Add("-o", "output file", &outfilename);
  interface->Add("-m", "memdepth", &memdepth);
  interface->Add("-t", "write tree", &wtree);
  interface->CheckFlags(argc, argv);



  FILE *inFile;
  inFile=fopen(filename,"rb");
  FILE *outFile;
  if(!wtree)
    outFile=fopen(outfilename,"w");

  //read header if available here
  header_type header;
  size_t ss;
  if(inFile!=NULL) 
    ss=fread(&header,sizeof(header_type),1,inFile);
  if(ss!=1) 
    cout<<"Error reading header!"<<endl;

  cout<<"----------HEADER content---------"<<endl;
  cout<<"Run No: "<<header.runno<<endl;
  struct tm * start_timeinfo;
  start_timeinfo = localtime ( &header.start_stop_time );
  printf("Start time:%s",asctime(start_timeinfo));
  cout<<"Comment: "<<header.comment;
  //write header
  if(!wtree)
    fwrite(&header,sizeof(header_type),1,outFile);
  cout<<"----------PROCEED TO FILE CHECKING-------------------"<<endl;

  //Calculating size
  //GET SIZE
  size_t file_size;
  struct stat st;
  stat(filename, &st);
  file_size = st.st_size;
  cout<<"FILE SIZE="<<file_size<<endl;
  int nBlock;
  if((file_size-2*sizeof(header_type))%(sizeof(buffer_type)*header.MAX_BLOCK_t)==0){
    cout<<"FILE is OK!"<<endl;
  }
  else{
    cout<<"Bad file!!"<<endl;
  }
  nBlock=(file_size-2*sizeof(header_type))/(sizeof(buffer_type)*header.MAX_BLOCK_t);
  cout<<"NO of BLOCK="<<nBlock<<endl;
  cout<<"NO of Event="<<nBlock*header.MAX_BLOCK_t<<endl;
  cout<<"---------------------------------------"<<endl;
 
  buffer_type buf[1];
  size_t s;
  
  if(maxblocks>0)
    nBlock = maxblocks;

  TSsort tssort(outFile);
  tssort.SetMemDepth(memdepth);
  long long prevTimestamp =0;
  for(int j=0;j<nBlock*header.MAX_BLOCK_t;j++){
  //for(int j=0;j<nBlock;j++){
    //cout << "reading j = " << j << endl; 
    s=fread(buf,sizeof(buffer_type),1,inFile);
    unsigned int tsmsb=buf[0].dgtzdata[7]>>16;
    long long longTimestamp=(long long) tsmsb<<31 | (buf[0].dgtzdata[3]&0x7FFFFFFF);
    //cout <<j<<"\t"<< buf[0].dgtzdata[0] << "\t" << buf[0].dgtzdata[1] << "\t" << longTimestamp <<"\t" << buf[0].dgtzdata[4] << endl;

    // if(longTimestamp-prevTimestamp<0){
    //   cout  << "TS jump " <<j<<"\t"<< longTimestamp <<"\t"<< prevTimestamp <<"\t" <<longTimestamp-prevTimestamp << endl;
    // }
      
    if(!tssort.Add(buf[0])){
      cout << "error adding buffer with timestamp " << longTimestamp << endl;
    }
    //cout <<"----------------------------------"<<endl;
      
    prevTimestamp=longTimestamp;
  }
  header_type footer;

  s=fread(&footer,sizeof(header_type),1,inFile);
  if(!wtree)
    fwrite(&footer,sizeof(header_type),1,outFile);
  if(s<1){
    cout<<"Can't read footer!"<<endl;
  }
  else{
    //For footer
    cout<<"---------------------------"<<endl;
    cout<<"End of run No "<<footer.runno<<endl;

    struct tm * stop_timeinfo;
    stop_timeinfo = localtime ( &footer.start_stop_time );
    printf("Aquisition stop at %s",asctime(stop_timeinfo));
    cout<<"SCALER INFO from footer:"<<endl;
    cout<<"End of run Comment: "<<footer.comment;

    for (int b=0;b<footer.nboard;b++){
      printf("Board No %i\n",b);
      for (int ch=0;ch<footer.MaxNChannels_t;ch++){
	if (!(footer.channelMask[b] & (1<<ch))) continue;
	printf("          Channel %i received %i events\n",ch,footer.TrgCnt[b][ch]);
      }
    }
  }
  cout << "flushing " << endl; 
  cout.flush();
  tssort.Flush();
  tssort.Status();

  fclose(inFile);
  fclose(outFile);
  return 77;
}
