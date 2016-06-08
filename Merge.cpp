#include "Merge.h"
#include "stdio.h"
#include "string.h"

Merge::Merge(char *filename, int nboards, int memdepth){
  finFiles.resize(nboards);
  fyoungestTS.resize(nboards);
  for(unsigned short i=0;i<nboards;i++){
    finFiles[i]=fopen(Form("%s_m%d.nig",filename,i+1),"rb");
    fyoungestTS[i] = LLONG_MAX;
  }
  ffilesize.resize(nboards);
  ftimesread.resize(nboards);
  ffinished.resize(nboards);
  ffinishedsum = 0;
  fnboards = nboards;
  ffile = NULL;
  ffilename = filename;
  //ftssort.SetMemDepth(memdepth);
  
  fyoungestboard = -1;
}
int Merge::ReadHeaders(){
  header_type header;
  size_t ss;
  int totalfilesize = 0;
  for(unsigned short i = 0;i<fnboards;i++){
    if(finFiles[i]!=NULL) 
      ss=fread(&header,sizeof(header_type),1,finFiles[i]);
    if(ss!=1) 
      cout<<"Error reading header!"<<endl;
    cout<<"----------HEADER content---------"<<endl;
    cout<<"Run No: "<<header.runno<<endl;
    struct tm * start_timeinfo;
    start_timeinfo = localtime ( &header.start_stop_time );
    printf("Start time:%s",asctime(start_timeinfo));
    cout<<"Comment: "<<header.comment << endl;
    //write header
    if(ffile)
      fwrite(&header,sizeof(header_type),1,ffile);

    cout<<"----------PROCEED TO FILE CHECKING-------------------"<<endl;

    size_t file_size;
    struct stat st;
    stat(Form("%s_m%d.nig",ffilename,i+1), &st);
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
    ffilesize[i] = nBlock*header.MAX_BLOCK_t;
    totalfilesize += ffilesize[i];
  }
  return totalfilesize;
}
void Merge::ReadFooters(){
  header_type footer;
  size_t ss;
  for(unsigned short i = 0;i<fnboards;i++){
    if(finFiles[i]!=NULL) 
      ss=fread(&footer,sizeof(header_type),1,finFiles[i]);
    if(ss!=1) 
      cout<<"Error reading footer!"<<endl;

    cout<<"----------Footer content---------"<<endl;
    cout<<"End of Run No: "<<footer.runno<<endl;
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
    if(ffile)
      fwrite(&footer,sizeof(header_type),1,ffile);
  }
}
void Merge::SetFile(FILE* out){
  ffile = out;
  //ftssort.SetFile(ffile);
}
void Merge::SetRootFile(TFile* out){
  ffile = NULL;
  frootfile = out;
  ftr = new TTree("tr","merged events");
  fboard = -1;
  fch = -1;
  fen = -1;
  fts = -1;
  ftr->Branch("board",&fboard,"board/I");
  ftr->Branch("ch",&fch,"ch/I");
  ftr->Branch("en",&fen,"en/I");
  ftr->Branch("ts",&fts,"ts/L");
  ftr->BranchRef();
  //ftssort.SetRootFile(out);
}
void Merge::ReadFirst(){
  for(unsigned short i = 0;i<fnboards;i++){
    ReadFromFile(i);
  }
}
void Merge::Read(){
  sort(fitems.begin(), fitems.end(),TSComparer());

  WriteFile(fitems.back());
  fitems.pop_back();

  if(!ffinished[fyoungestboard])
    ReadFromFile(fyoungestboard);
  else{
    fyoungestTS[fyoungestboard] = LLONG_MAX;
    long long youngest = LLONG_MAX;
    for(int i=0;i<fnboards;i++){
      if(fyoungestTS[i]<youngest){
	fyoungestboard = i;
	youngest = fyoungestTS[i];
      }
    }
    ReadFromFile(fyoungestboard);
  }

}
bool Merge::ReadFromFile(int board){
  if(ftimesread[board]>=ffilesize[board]){
    return false;
  }
  buffer_type buf[1];
  size_t s;
  s=fread(buf,sizeof(buffer_type),1,finFiles[board]);
  buf[0].dgtzdata[0] = board;

  unsigned int tsmsb=buf[0].dgtzdata[7]>>16;
  long long longTimestamp=(long long) tsmsb<<31 | (buf[0].dgtzdata[3]&0x7FFFFFFF);
  //cout <<ftimesread[board]<<"\t"<< buf[0].dgtzdata[0] << "\t" << buf[0].dgtzdata[1] << "\t" << longTimestamp <<"\t" << buf[0].dgtzdata[4] << endl;
  
  fyoungestTS[board] = longTimestamp;
  fyoungestboard = board;
  for(int i=0;i<fnboards;i++){
    if(longTimestamp>fyoungestTS[i])
      fyoungestboard = i;
  }
  item* toadd = new item;
  toadd->buf = buf[0];
  toadd->TS = longTimestamp;

  fitems.push_back(toadd);

  ftimesread[board]++;
  if(ftimesread[board]==ffilesize[board]){
    cout << "file " << Form("%s_m%d.nig",ffilename,board+1) << " finished!" << endl;
    ffinished[board] = 1;
    ffinishedsum++;
  }
  return true;
}
void Merge::Flush(){
  cout << __PRETTY_FUNCTION__ << endl;
  for(unsigned short i = 0;i<fnboards;i++){
    fclose(finFiles[i]);
  }
  sort(fitems.begin(), fitems.end(),TSComparer());
  for(int i=fitems.size()-1;i>-1;i--){
    WriteFile(fitems.at(i));
  }
  if(frootfile){
    frootfile->cd();
    ftr->Write();
    //fhtdiff->Write();
  }
}
void Merge::WriteFile(item* writeme){
  if(ffile){
    fwrite(&(writeme->buf), sizeof(buffer_type), 1, ffile);
  }
  else{
    fboard = (writeme->buf).dgtzdata[0];
    fch = (writeme->buf).dgtzdata[1];
    fen = (writeme->buf).dgtzdata[4];
    fts = writeme->TS;
    ftr->Fill();
    //fhtdiff->Fill(writeme->TS - flastts);
  }
  //flastts = writeme->TS;
}

