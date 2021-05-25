/* akai_aux.c */
#include <stdio.h>
#include "akai_tools.h"
#include "akai_aux.h"

static int getParamNumber(char *par);
static int getParamValue(char *par);


void printBank(struct bank_t *b, int n) {
int i;

  printf("BANK %d\n",n);
  printf("E00 VCO Octave %d\n",b->VCOaOCT);
  printf("E01 VCO Wave form %d\n",b->VCOaWF);
  printf("E02 VCO Pulse Width %d\n",b->VCOaPW);
  printf("E03 VCO PWMS %d\n",b->VCOaPWMS);
  printf("E04 VCO EG %d\n",b->VCOaEG);
  printf("E05 NOISE %d\n",b->NOISEb);
  printf("E06 SAMPLER %d\n",b->SAMPLERb);
  printf("E07 a - b BALANCE %d\n",b->a_bBAL);
  printf("E10 VCF cut-off FREQ %d\n",b->VCF_FREQ);
  printf("E11 VCF reso %d\n",b->VCF_RESO);
  printf("E12 VCF OWFM %d\n",b->VCF_OWFM);
  printf("E13 VCF EG %d\n",b->VCF_EG);
  printf("E14 VCF KEYF %d\n",b->VCF_KEYF);
  printf("E15 VCF VELO %d\n",b->VCF_VELO);
  printf("E16 VCF HIGH PASS FILTER %d\n",b->HPF);
 printf("E20 EG SELECT %d\n",b->EG_SEL);
  if(b->EG_SEL==0) {
    printf("E21 EGA ATTACK %d\n",b->EGA_A);
    printf("E22 EGA DECAY %d\n",b->EGA_D);
    printf("E23 EGA SUSTAIN %d\n",b->EGA_S);
    printf("E24 EGA RELEASE %d\n",b->EGA_R);
    printf("E25 EGOF ATTACK %d\n",b->EGOF_A);
    printf("E26 EGOF DECAY %d\n",b->EGOF_D);
    printf("E27 EGOF SUSTAIN %d\n",b->EGOF_S);
    printf("E28 EGOF RELEASE %d\n",b->EGOF_R);
  } else {
    printf("E21 EGAF ATTACK %d\n",b->EGAF_A);
    printf("E22 EGAF DECAY %d\n",b->EGAF_D);
    printf("E23 EGAF SUSTAIN %d\n",b->EGAF_S);
    printf("E24 EGAF RELEASE %d\n",b->EGAF_R);
    printf("E25 EGO ATTACK %d\n",b->EGO_A);
    printf("E26 EGO DECAY %d\n",b->EGO_D);
    printf("E27 EGO SUSTAIN %d\n",b->EGO_S);
    printf("E28 EGO RELEASE %d\n",b->EGO_R);
  }
  printf("E30 VGA LEVEL %d\n",b->VCA_LEV);
  printf("E31 VGA VELOCITY %d\n",b->VCA_VELO);
  printf("E40 LFO SEL %d\n",b->LFO_SEL);
  printf("E41 LFO WAVE FORM %d\n",b->LFO_WF);
  printf("E42 LFO FREQ %d\n",b->LFO_FREQ);
  printf("E43 LFO DEPTH %d\n",b->LFO_DEP);
  printf("E44 LFO DELAY %d\n",b->LFO_DEL);
  printf("E45 CHORUS %d\n",b->CHORUS);
  printf("E50 ASSIGN %d\n",b->ASSIGN);
  printf("E51 PORTAMENTO %d\n",b->SOL_PORT);
  printf("E52 DETUNE %d\n",b->DETUNE);
  printf("E60 PITCH BEND WHEEL RANGE VCO %d\n",b->WH_BND_O);
  printf("E61 PITCH BEND WHEEL RANGE VCF %d\n",b->WH_BND_F);
  printf("E62 MODULATION WHEEL LEVEL %d\n",b->WH_MOD);
  printf("E70 MIDI SPLIT %d\n",b->MIDI_SP);
  printf("E71 MIDI SPLIT POINT %d\n",b->MIDI_SPP);
  printf("LABEL ");
  for(i=0;i<12;i++) {
    printf("%c",b->LABEL[i]);
  }
  printf("\n");
  
}

static int getParamNumber(char *par) {
 return (par[1]-'0') *10 + par[2]-'0';
}

static int getParamValue(char *par){
  int p,i,r,m;
  i=0;
  while(par[i]) {
    if(par[i]==' ') p=i;
    i++;
  }
  r=0;
  m=1;
  while(i>p) {
    if(par[i]>='0' && par[i]<='9') {
      r=r+(par[i]-'0')*m;
      m=m*10; 
    }
    i--;
  }
  if(par[p+1]=='-') r *= -1;
  return r;
}

int readBank(struct bank_t *b, char *par) {
int i,value;
//printf("par %s",par);
  if(par[0]=='B') {
//    printf("BANK %d\n",getParamValue(par));
    return getParamValue(par);
  }
  if(par[0]=='L') {
    for(i=0;i<12;i++) {
      b->LABEL[i]=par[i+6];
    }
    return -1; 
  }
//  printf("parametro %d valor %d\n",getParamNumber(par),getParamValue(par));
  value=getParamValue(par);
  switch(getParamNumber(par)) {
    case 0: b->VCOaOCT=value;break;
    case 1: b->VCOaWF=value;break;
    case 2: b->VCOaPW=value;break;
    case 3: b->VCOaPWMS=value;break;
    case 4: b->VCOaEG=value;break;
    case 5: b->NOISEb=value;break;
    case 6: b->SAMPLERb=value;break;
    case 7: b->a_bBAL=value;break;
    case 10: b->VCF_FREQ=value;break;
    case 11: b->VCF_RESO=value;break;
    case 12: b->VCF_OWFM=value;break;
    case 13: b->VCF_EG=value;break;
    case 14: b->VCF_KEYF=value;break;
    case 15: b->VCF_VELO=value;break;
    case 16: b->HPF=value;break;
    case 20: b->EG_SEL=value;break;
    case 21: b->EGA_A=value;break;
    case 22: b->EGA_D=value;break;
    case 23: b->EGA_S=value;break;
    case 24: b->EGA_R=value;break;
    case 25: b->EGOF_A=value;break;
    case 26: b->EGOF_D=value;break;
    case 27: b->EGOF_S=value;break;
    case 28: b->EGOF_R=value;break;
    case 30: b->VCA_LEV=value;break;
    case 31: b->VCA_VELO=value;break;
    case 40: b->LFO_SEL=value;break;
    case 41: b->LFO_WF=value;break;
    case 42: b->LFO_FREQ=value;break;
    case 43: b->LFO_DEP=value;break;
    case 44: b->LFO_DEL=value;break;
    case 45: b->CHORUS=value;break;
    case 50: b->ASSIGN=value;break;
    case 51: b->SOL_PORT=value;break;
    case 52: b->DETUNE=value;break;
    case 60: b->WH_BND_O=value;break;
    case 61: b->WH_BND_F=value;break;
    case 62: b->WH_MOD=value;break;
    case 70: b->MIDI_SP=value;break;
    case 71: b->MIDI_SPP=value;break;
  }
  return -1;
}
