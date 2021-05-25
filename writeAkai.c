/* writeAkai.c */
#include <stdio.h>
#include "akai_tools.h"
#include "akai_aux.h"

int main(int argc, char **argv) {
char buffer[200];
char *line;
int i,b,r=-1;
struct bank_t bank[100];

  if(argc!=2) return -1;
  i=0;
  b=0;
  while(!feof(stdin)) {
    line=fgets(buffer,200,stdin);
    if(line) {
      r=readBank(&bank[b],line);
    }
      if(r!=-1) b=r;
  }
  write_akai_file(argv[1],bank,100);
  return 0;
}

