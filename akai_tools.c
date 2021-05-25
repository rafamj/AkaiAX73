/* akai_tools.c */
#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <math.h>
#include <string.h>
#include "akai_tools.h"

#define OUTPUT_RATE 44100
#define OUTPUT_CHANNELS 2
#define AMPLITUDE (1.0 * 0x7F00)
#define MAX_PARAMS 52 //number of parameters in each AKAI bank
#define ONE_FREQ 2400
#define ZERO_FREQ 1200

static double readSample(SNDFILE *infile,int channels, int samplerate,int *error); 
static void searchStart(SNDFILE *infile,int channels, int samplerate);
static double readNextCycle(SNDFILE *infile,int channels, int samplerate);
static int getValue(double f);
static int getData(char *b);
static void storeData(struct bank_t *b, int n, int d);
static int readData(struct bank_t *b, int n);
static int readNextValue(SNDFILE *infile,int channels, int samplerate);
static void readPilot(SNDFILE *infile,int channels, int samplerate);
static int process_data (SNDFILE *infile,int channels, int samplerate, struct bank_t *banks);
static void writeNextCycle(SNDFILE *outfile,int channels, int samplerate,double freq);
static void writeValue(SNDFILE *outfile,int channels, int samplerate,int value);
static void writePilot(SNDFILE *outfile,int channels, int samplerate);
static void writeEnd(SNDFILE *outfile,int channels, int samplerate);
static void writeData(SNDFILE *outfile,int channels, int samplerate,int data);


static double readSample(SNDFILE *infile,int channels, int samplerate,int *error) {
int readcount;
int chan;
double value=0.0;
double data[255];
  
  readcount = sf_read_double(infile, data,channels);
  if(readcount==channels) {
      for (chan = 0 ; chan < channels ; chan ++) {
        value +=data[chan];
      }
     *error = 0;
    return value;
  } else {
    *error = 1;
    return 0.0;
  } 
}

static void searchStart(SNDFILE *infile,int channels, int samplerate) {
  int i=0;
  int error=0;
  double d,antd;
  
  d=readSample(infile,channels,samplerate,&error);
  while(!error) {
    antd=d;
    d=readSample(infile,channels,samplerate,&error);
    if(((d>0.0) && (d*antd==-1))||i>100) {
      return;
    }
    i++;
  }
}


static double readNextCycle(SNDFILE *infile,int channels, int samplerate) {
  static double d=0.0;
  double antd=0.0;
  static double samplesPerCycle;
  static int polarity=1;
  int posSample=0,negSample=0;
  double freq;
  int error=0;

  while(!error) {
    antd=d;
    d=polarity*readSample(infile,channels,samplerate,&error);
    if(d>=0) {
      posSample++;
    } else {
      negSample++;
    }
//printf("read %g\n",d);
      if(antd*d<=0.0) {
        if(d<0.0) {
            samplesPerCycle +=fabs(antd/(d-antd));  // antd<=0
            freq=(double)samplerate/samplesPerCycle;
//printf("resto %g ",fabs((double)antd/(d-antd)));
            samplesPerCycle=fabs(d/(d-antd)); // initial value for next cycle
//printf("samplesPerCycle %g  freq %g\n",samplesPerCycle,freq);
            if(abs(posSample-negSample)>2) {
               while(d<0.0) { // read an extra half cycle
                 d=readSample(infile,channels,samplerate,&error);
//printf("extra read %g\n",d);
               }
               polarity=-1;
               d = -d;
               return 1200.0;
            }
            return freq;   
        } 
      } else {
        samplesPerCycle +=1.0;
      }
  }
  return 0.0;
}

int read_akai_file(char *fileName,struct bank_t *banks){
  SNDFILE      *infile;
  SF_INFO      sfinfo ;


  setbuf(stdout, NULL);  
  if (! (infile = sf_open(fileName,SFM_READ, &sfinfo))) {
        printf ("Not able to open input file %s.\n", fileName) ;
        sf_perror (NULL) ;
        return  0 ;
  } 
//printf("format %x rate %d channels %d\n",sfinfo.format,sfinfo.samplerate,sfinfo.channels);
  return process_data (infile,sfinfo.channels, sfinfo.samplerate,banks);
/*
    switch(sfinfo.format & 0xff) {
      case SF_FORMAT_PCM_16:  return process_data (infile,sfinfo.channels, sfinfo.samplerate,banks);
      default: printf("Error. Format no supported\n");
    }
*/
  sf_close (infile) ;

  return 0 ;
}

static int getValue(double f) {
  if(f==0.0) return -1;
  return f>(ONE_FREQ+ZERO_FREQ)/2;
}


static int getData(char *b) {
char d=0;
int i=0;
int mask=1;
   
  while(b[i]) {
    d= d | (mask * (b[i]-'0'));
//printf("i %d b[i] %d mask %d d %d\n",i,b[i],mask,d);
    mask=mask<<1;
    i++;
  }
 d>>=1;
 d=d & 0x7f;
 return d;
}

static void storeData(struct bank_t *b, int n, int d) {

  switch(n) {
    case 0: b->VCOaOCT=d;break;
    case 1: b->VCOaWF=d;break;
    case 2: b->VCOaPW=d;break;
    case 3: b->VCOaPWMS=d;break;
    case 4: b->VCOaEG=d;break;
    case 5: b->NOISEb=d;break;
    case 6: b->SAMPLERb=d;break;
    case 7: b->a_bBAL=d;break;
    case 8: b->VCF_FREQ=d;break;
    case 9: b->VCF_RESO=d;break;
    case 10: b->VCF_OWFM=d;break;
    case 11: b->VCF_EG=d-50;break;
    case 12: b->VCF_KEYF=d;break;
    case 13: b->VCF_VELO=d-50;break;
    case 14: b->HPF=d;break;
    case 15: b->EG_SEL=d;break;
    case 16: b->EGA_A=d;break;
    case 17: b->EGA_D=d;break;
    case 18: b->EGA_S=d;break;
    case 19: b->EGA_R=d;break;
    case 20: b->EGOF_A=d;break;
    case 21: b->EGOF_D=d;break;
    case 22: b->EGOF_S=d;break;
    case 23: b->EGOF_R=d;break;
    case 24: b->VCA_LEV=d;break;
    case 25: b->VCA_VELO=d-50;break;
    case 26: b->LFO_SEL=d;break;
    case 27: b->LFO_WF=d;break;
    case 28: b->LFO_FREQ=d;break;
    case 29: b->LFO_DEP=d;break;
    case 30: b->LFO_DEL=d;break;
    case 31: b->CHORUS=d;break;
    case 32: b->ASSIGN=d;break;
    case 33: b->SOL_PORT=d;break;
    case 34: b->DETUNE=d;break;
    case 35: b->WH_BND_O=d;break;
    case 36: b->WH_BND_F=d;break;
    case 37: b->WH_MOD=d;break;
    case 38: b->MIDI_SP=d;break;
    case 39: b->MIDI_SPP=d;break;
    case 40: b->LABEL[0]=d;break;
    case 41: b->LABEL[1]=d;break;
    case 42: b->LABEL[2]=d;break;
    case 43: b->LABEL[3]=d;break;
    case 44: b->LABEL[4]=d;break;
    case 45: b->LABEL[5]=d;break;
    case 46: b->LABEL[6]=d;break;
    case 47: b->LABEL[7]=d;break;
    case 48: b->LABEL[8]=d;break;
    case 49: b->LABEL[9]=d;break;
    case 50: b->LABEL[10]=d;break;
    case 51: b->LABEL[11]=d;break;
  }
}

static int readData(struct bank_t *b, int n) {

  switch(n) {
    case 0: return b->VCOaOCT;
    case 1: return b->VCOaWF;
    case 2: return b->VCOaPW;
    case 3: return b->VCOaPWMS;
    case 4: return b->VCOaEG;
    case 5: return b->NOISEb;
    case 6: return b->SAMPLERb;
    case 7: return b->a_bBAL;
    case 8: return b->VCF_FREQ;
    case 9: return b->VCF_RESO;
    case 10: return b->VCF_OWFM;
    case 11: return b->VCF_EG+50;
    case 12: return b->VCF_KEYF;
    case 13: return b->VCF_VELO+50;
    case 14: return b->HPF;
    case 15: return b->EG_SEL;
    case 16: return b->EGA_A;
    case 17: return b->EGA_D;
    case 18: return b->EGA_S;
    case 19: return b->EGA_R;
    case 20: return b->EGOF_A;
    case 21: return b->EGOF_D;
    case 22: return b->EGOF_S;
    case 23: return b->EGOF_R;
    case 24: return b->VCA_LEV;
    case 25: return b->VCA_VELO+50;
    case 26: return b->LFO_SEL;
    case 27: return b->LFO_WF;
    case 28: return b->LFO_FREQ;
    case 29: return b->LFO_DEP;
    case 30: return b->LFO_DEL;
    case 31: return b->CHORUS;
    case 32: return b->ASSIGN;
    case 33: return b->SOL_PORT;
    case 34: return b->DETUNE;
    case 35: return b->WH_BND_O;
    case 36: return b->WH_BND_F;
    case 37: return b->WH_MOD;
    case 38: return b->MIDI_SP;
    case 39: return b->MIDI_SPP;
    case 40: return b->LABEL[0];
    case 41: return b->LABEL[1];
    case 42: return b->LABEL[2];
    case 43: return b->LABEL[3];
    case 44: return b->LABEL[4];
    case 45: return b->LABEL[5];
    case 46: return b->LABEL[6];
    case 47: return b->LABEL[7];
    case 48: return b->LABEL[8];
    case 49: return b->LABEL[9];
    case 50: return b->LABEL[10];
    case 51: return b->LABEL[11];
  }
  return 0;
}

static int readNextValue(SNDFILE *infile,int channels, int samplerate) {
int freq;

  freq=readNextCycle(infile, channels, samplerate);
//printf("%1d",getValue(freq));
  return getValue(freq);
}

static void readPilot(SNDFILE *infile,int channels, int samplerate) {
int value=1,antvalue=1;
int ones_received=0;

  
  searchStart(infile, channels, samplerate);
  while(value==1||antvalue!=0) { //wait for 2 0's
    antvalue=value;
    value=readNextValue(infile,channels,samplerate);
    if(value==-1) return;
  }
  while(ones_received<14) {  // wait for many 1's
    value=readNextValue(infile,channels,samplerate);
    if(value==1) {
      ones_received++;
    } else if(value==-1) return;
  }
 while(value!=0) { // waiting for 0 
    value=readNextValue(infile,channels,samplerate);
    if(value==-1) return;
 } 
//printf("ok\n");
}

static int process_data (SNDFILE *infile,int channels, int samplerate, struct bank_t *banks) {
int data;
int value,antvalue;
int bytes_received=0;
int bits_expected=0;
int parameter_number=0;
int ones_received=0;
char buffer[100];
int buffer_index=0;
int bank_number=0;

  if(channels>255) { 
    printf("Error. Too many channels\n");
  } 
  readPilot(infile, channels, samplerate);
  value=0;//the first bit after the header is a 0
  ones_received=0;//we start to receive data
  buffer_index=0;
  bits_expected=9;
  while(value!=-1) {
    if(value==1) {
      ones_received++;
    } else { //value==0
      if(ones_received>11){ //we have received the first 0 after many 1's(usually 15)
            buffer[buffer_index]=0;
            data=getData(buffer);
    //        printf(" %d %s data %d\n",parameter_number,buffer,data);
            storeData(&banks[bank_number],parameter_number,data);
            bytes_received++;
            parameter_number++;
            if(bytes_received%52==0) { 
              parameter_number=0;
              bank_number++;
              if(bank_number==100) {
	         printf("\n");
		 return bank_number;
	      }
              //printBank(&bank[bank_number],bank_number);
            }
          buffer_index=0;
          bits_expected=9;
        }
        //ones_received++;
    }
    
    if(bits_expected) { //here we read the next bits_expected
      //printf("%d",value);
      if(value==0 || antvalue==1) { // we read every other 1
        bits_expected--;
        if(buffer_index<90) {
          buffer[buffer_index++]= '0'+value;
        }
        value=0; // in order to read very other 1
      }
      ones_received=0;
    }
    antvalue=value;
    value=readNextValue(infile, channels, samplerate);
  }
return bank_number;
}

static void writeNextCycle(SNDFILE *outfile,int channels, int samplerate,double freq) {
  static double angle=0.0;
  int d,c,k;
  short data[255];

//printf("samples %g\n",samplerate/freq );
   k=0;
   while(angle<2*M_PI) {
     d=AMPLITUDE *  sin(angle) ;
    //printf("k %d s0 %g %d\n",k,angle,d);
     for(c=0;c<channels;c++) {
       data[channels*k+c] =  d;
     }
     k=k+1;
     angle +=freq/samplerate * 2  * M_PI;
   }
   angle=angle-2*M_PI;
   if (sf_write_short (outfile, data, channels*k) !=channels*k) {
      printf("error writing file\n");
      sf_close (outfile);
      exit(1);
   }
//printf("fase %g\n",s0);
}

static void writeValue(SNDFILE *outfile,int channels, int samplerate,int value) {
//printf("writing %d\n",value);
  if(value==1) {
    writeNextCycle(outfile,channels,samplerate,ONE_FREQ);
  } else {
    writeNextCycle(outfile,channels,samplerate,ZERO_FREQ);
  } 
}

static void writePilot(SNDFILE *outfile,int channels, int samplerate) {
int i;

  for(i=0;i<4000;i++) {
    writeValue(outfile,channels,samplerate,1);
  }  
  writeValue(outfile,channels,samplerate,0);
  writeValue(outfile,channels,samplerate,0);
  for(i=0;i<3;i++) {
    writeValue(outfile,channels,samplerate,1);
    writeValue(outfile,channels,samplerate,1);
    writeValue(outfile,channels,samplerate,0);
  }
  for(i=0;i<17;i++) {
    writeValue(outfile,channels,samplerate,1);
  }
}

static void writeEnd(SNDFILE *outfile,int channels, int samplerate) {
int i;

  for(i=0;i<30;i++) {
    writeData(outfile,channels,samplerate,85);
  }  
  for(i=0;i<2000;i++) {
    writeValue(outfile,channels,samplerate,0);
  }
}

static void writeData(SNDFILE *outfile,int channels, int samplerate,int data) {
int mask=1;
int i;

  writeValue(outfile,channels,samplerate,0);
  for(i=0;i<7;i++){
    if(data & mask) {
      writeValue(outfile,channels,samplerate,1);
      writeValue(outfile,channels,samplerate,1);
    } else {
      writeValue(outfile,channels,samplerate,0);
    }
    mask = mask<<1;
  }
  writeValue(outfile,channels,samplerate,0);
  for(i=0;i<15;i++) {
    writeValue(outfile,channels,samplerate,1);
  }  
}

void write_akai_file(char *fileName,struct bank_t *banks,int banks_number){
  SNDFILE      *outfile;
  SF_INFO      sfinfo ;
int i,d,p;

  memset (&sfinfo, 0, sizeof (sfinfo)) ;
  sfinfo.samplerate       = OUTPUT_RATE ;
  sfinfo.channels         = OUTPUT_CHANNELS ;
  sfinfo.format           = (SF_FORMAT_WAV | SF_FORMAT_PCM_16) ;



  if (! (outfile = sf_open(fileName,SFM_WRITE, &sfinfo))) {
        printf ("Not able to open output file %s.\n", fileName) ;
        sf_perror (NULL) ;
        return ;
  } 
  writePilot(outfile,sfinfo.channels,sfinfo.samplerate);
  for(i=0;i<banks_number;i++) {
    for(p=0;p<MAX_PARAMS;p++) {
      d=readData(&banks[i],p);
      writeData(outfile,sfinfo.channels,sfinfo.samplerate,d);
    }
  }
  writeEnd(outfile,sfinfo.channels,sfinfo.samplerate);
  sf_close (outfile) ;
}
