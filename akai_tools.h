/* akai_tools.h*/
struct bank_t{
  int VCOaOCT;
  int VCOaWF;
  int VCOaPW;
  int VCOaPWMS;
  int VCOaEG;
  int NOISEb;
  int SAMPLERb;
  int a_bBAL;
  int VCF_FREQ;
  int VCF_RESO;
  int VCF_OWFM;
  int VCF_EG;
  int VCF_KEYF;
  int VCF_VELO;
  int HPF;
  int EG_SEL;
  union {
    struct {
      int EGA_A;
      int EGA_D;
      int EGA_S;
      int EGA_R;
     int EGOF_A;
     int EGOF_D;
     int EGOF_S;
     int EGOF_R;
   };
   struct {
     int EGAF_A;
     int EGAF_D;
     int EGAF_S;
     int EGAF_R;
     int EGO_A;
     int EGO_D;
     int EGO_S;
     int EGO_R;
    };
  };
  int VCA_LEV;
  int VCA_VELO;
  int LFO_SEL;
  int LFO_WF;
  int LFO_FREQ;
  int LFO_DEP;
  int LFO_DEL;
  int CHORUS;
  int ASSIGN;
  int SOL_PORT;
  int DETUNE;
  int WH_BND_O;
  int WH_BND_F;
  int WH_MOD;
  int MIDI_SP;
  int MIDI_SPP;
  char LABEL[12]; 
};
extern int read_akai_file(char *fileName,struct bank_t *banks);
extern void write_akai_file(char *fileName,struct bank_t *banks,int banks_number);

