#include "TSsort.h"
#include "stdio.h"
#include "string.h"
TSsort::TSsort(TFile *out){
  frootfile = out;
  fevtnr =0;
  fdiscarded =0;
  fmemdepth = 200;
}
TSsort::TSsort(FILE *out){
  ffile = out;
  fevtnr =0;
  fdiscarded =0;
  fmemdepth = 200;
}
bool TSsort::Add(buffer_type addme){
  fevtnr++;
  item* toadd = new item;
  toadd->buf = addme;
  
  unsigned int tsmsb=addme.dgtzdata[7]>>16;
  long long timestamp=(long long) tsmsb<<31 | (addme.dgtzdata[3]&0x7FFFFFFF);
  
  toadd->TS = timestamp;
  //cout << __PRETTY_FUNCTION__ << endl;
  //cout << "TS = " << timestamp << endl;
  //cout << "items already in memory" << endl;
  //list<item*>::const_iterator it;
  //for(it = flist.begin(); it != flist.end(); ++it) {
  //  cout << (*it)->TS << endl;
  //}
  
  if(flist.empty()){
    //list is empty, add new item
    flist.push_back(toadd);
    return true;
  }
  else if(fevtnr > fmemdepth){
    //to be written
    //cout << "fevtnr > fmemdepth" << endl;
    return FullList(toadd);
  }
  else{
    //insert item into list
    return Insert(toadd);
  }
}
bool TSsort::FullList(item* addme){
  list<item*>::iterator it = flist.begin();

  if((*it)->TS > addme->TS){
    fdiscarded++;
    cout << __PRETTY_FUNCTION__ << " problem" << endl;
    cout << "current timestamp item " << addme->TS << endl;
    cout << "oldest timestamp in list " << (*it)->TS << endl;
    cout << "discarding this item, you should increase memory depth" << endl;
    return false;
  }
  //write out oldest item on the list
  WriteFile(*it);
  //remove it from list
  delete (*it);
  it = flist.erase(it);
  return Insert(addme);
}
bool TSsort::Insert(item* insertme){
  //cout << __PRETTY_FUNCTION__ << endl;
  //cout << "TS = " << insertme->TS << endl;

  list<item*>::iterator it = flist.end();
  it--;
  //cout << "end = " << (*it)->TS << endl;
  while(it != flist.begin() && (*it)->TS >= insertme->TS){
    //cout << "list " << (*it)->TS << endl;
    it--;
  }
  it++;
  flist.insert(it,insertme);
  return true;
}
void TSsort::WriteFile(item* writeme){
  if(ffile){
    fwrite(&(writeme->buf), sizeof(buffer_type), 1, ffile);
  }
}
void TSsort::Flush(){
  list<item*>::iterator it = flist.begin();
  while(it != flist.end()){
    WriteFile(*it);
    delete *it;
    it = flist.erase(it);
  }
}
void TSsort::Status(){
  cout << "Status of HFC object:" << endl;
  cout << "Event memory depth: " << fmemdepth << endl;
  cout << "Events processed:   " << fevtnr << endl;
  if(fdiscarded)
    cout << "Events discarded:   " << fdiscarded << "  (increase mem depth!)"<< endl;
}
