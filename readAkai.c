/* readAkai.c */
#include "akai_tools.h"
#include "akai_aux.h"

int main(int argc, char **argv) {
  struct bank_t bank[100];
  int banks_n,i;

  if(argc!=2) return -1;
  banks_n=read_akai_file(argv[1],bank);
  
  for(i=0;i<banks_n;i++) {
    printBank(&bank[i],i);
  }
return 0;
}

