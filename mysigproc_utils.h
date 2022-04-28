#include <stdint.h>
#ifndef __MYSIGPROC_DEF_H
#include "mysigproc_def.h"
#endif

#ifdef __cpluplus
extern "C" {
#endif
int write_header(fil_t);
int read_block(uint64_t, uint64_t, fil_t*, const int, float *);
int read_block_rescale(uint64_t, uint64_t, fil_t*, const int, float *, float, float);
int read_block_rescale_inv_chan(uint64_t, uint64_t, fil_t*, const int, float *, float, float);
int hhmmss_to_sigproc (char * hhmmss, double * sigproc);
int ddmmss_to_sigproc (char * ddmmss, double * sigproc);
fil_t* create_fil(char*, int*, const int);
int destroy_fil(fil_t*);
#ifdef __cplusplus
}
#endif
