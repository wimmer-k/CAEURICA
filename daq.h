#ifndef DAQ_H
#define DAQ_H

#define MAX_NBOARD       8
#define MaxNChannels    16
#define WF_BLOCK_LENGTH 96//4085 //maximum 8170
#define DGTZ_BLOCK_LENGTH 8 //maximum 8170
#define MAX_BLOCK       1000 //default 4

#define WF_WRITELENGTH  500

#define BLOCK_EMPTY     0
#define BLOCK_WRITING   1
#define BLOCK_WAITING   2
#define SHMKEY 75 //default


//For share memory
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#include <time.h>




#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32

    #include <time.h>
    #include <sys/timeb.h>
    #include <conio.h>
    #include <process.h>
    #define getch _getch     /* redefine POSIX 'deprecated' */
    #define kbhit _kbhit     /* redefine POSIX 'deprecated' */

#else
    #include <unistd.h>
    #include <stdint.h>   /* C99 compliant compilers: uint64_t */
    #include <ctype.h>    /* toupper() */
    #include <sys/time.h>
#endif

#include "dpp.h"

typedef struct header_type_t{
   //config info
  int nboard;
  int MAX_BLOCK_t;
  int MaxNChannels_t;
  int WF_BLOCK_LENGTH_t;
  int DGTZ_BLOCK_LENGTH_t;
  uint32_t TrgCnt[MAX_NBOARD][MaxNChannels];
  uint32_t channelMask[MAX_NBOARD];
  uint32_t  preTrg[MAX_NBOARD][MaxNChannels];
  uint32_t  recordLength[MAX_NBOARD];
  int  preGate[MAX_NBOARD][MaxNChannels];
  int shortGate[MAX_NBOARD][MaxNChannels];
  int longGate[MAX_NBOARD][MaxNChannels];
  uint8_t pulsePolarity[MAX_NBOARD][MaxNChannels];

  time_t  start_stop_time;
  uint32_t  runno;
  char comment[500];
  char raw_filename[756];
  int inputFileContent_Length;
  char inputFileContent[500*MAX_NBOARD][500];
} header_type;

typedef struct buffer_type_t{
  unsigned int status,blockno;
  /*
  shmp->buffer[shmp->block_no].blockno=shmp->block_no;
  shmp->buffer[shmp->block_no].dgtzdata[0] = (uint32_t)b;//Board number
  shmp->buffer[shmp->block_no].dgtzdata[1] = (uint32_t)ch;//Channel number
  shmp->buffer[shmp->block_no].dgtzdata[2] = (uint32_t)TrgCnt_e[b][ch];//event Number
  shmp->buffer[shmp->block_no].dgtzdata[3] = (uint32_t)Events_t[ch][ev].TimeTag;//TTT LSB
  shmp->buffer[shmp->block_no].dgtzdata[4] = (uint32_t)Events_t[ch][ev].Energy;

  shmp->buffer[shmp->block_no].dgtzdata[5] = Events_t[ch][ev].Format;
  shmp->buffer[shmp->block_no].dgtzdata[6] = (uint32_t)Events_t[ch][ev].Extras;
  shmp->buffer[shmp->block_no].dgtzdata[7] = Events_t[ch][ev].Extras2; //16bit TTT MSB
  */

  //uint16_t wfdata[WF_BLOCK_LENGTH];
  uint32_t dgtzdata[DGTZ_BLOCK_LENGTH];
  //double time_data[2];
} buffer_type;

typedef struct SHM_DGTZ_S{
  uint32_t pid;
  uint32_t  runno;
  uint32_t  status;
  time_t  start_time;
  time_t  stop_time;
  uint32_t  block_no;

  uint32_t channelMask[MAX_NBOARD];
  char config_file_name[MAX_NBOARD][500];
  char raw_projectName[756];
  char raw_saveDir[756];
  char raw_filename[756];
  FILE *raw_fd;

  buffer_type_t buffer[MAX_BLOCK];
  uint32_t TrgCnt[MAX_NBOARD][MaxNChannels];
  uint32_t com_flag;  
  time_t com_time;
  char com_filename[500];
  char com_comments[500];
  char com_message[500]; //interface for start/stop daq

  uint32_t  preTrg[MAX_NBOARD][MaxNChannels];
  uint32_t  recordLength[MAX_NBOARD];
  int  preGate[MAX_NBOARD][MaxNChannels];
  int shortGate[MAX_NBOARD][MaxNChannels];
  int longGate[MAX_NBOARD][MaxNChannels];
  uint8_t pulsePolarity[MAX_NBOARD][MaxNChannels];

  int ana_status;
  int ana_flag;
  char ana_message[500];

  //new add
  int nboard;

} SHM_DGTZ;





class daq
{
    private:
    SHM_DGTZ* shmp;
    daq(){};
};

#endif // DAQ_H

