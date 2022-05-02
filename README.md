# mysigproc
sigroc filterbank file reader/writer


# Example
```
// example.c

#include <stdio.h>
#include <stdlib.h>

#include "mysigproc_utils.h"

int main(int argc, char* argv[])
{
  char fname[256] = "tmp.fil";
  int err=0;

  fil_t* filbank = create_fil(fname, &err,
      SIGPROC_CREATE_FIL_WRITE);


  sprintf(filbank->header->source_name, "%s", "J0000+0000");

  filbank->header->tstart = 59699;
  filbank->header->machine_id = 1;
  filbank->header->telescope_id = 1;
  filbank->header->src_raj = 0;
  filbank->header->src_dej = 0;
  filbank->header->az_start = 0;
  filbank->header->za_start = 0;
  filbank->header->foff = 0.5;
  filbank->header->fch1 = 1400;
  filbank->header->nchans = 100;
  filbank->header->nifs = 1;
  filbank->header->nbits = 8;
  filbank->header->tsamp = 2e-6;
  filbank->header->nbeams = 1;
  filbank->header->ibeam = 0;

  write_header(*filbank);
  int i = destroy_fil(filbank);
}
```

# To compile
gcc -O3 mysigproc_utils.c example.c -o example
