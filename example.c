#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mysigproc_utils.h"

int main(int argc, char* argv[])
{
  // define name of output filterbank
  char fname[256] = "tmp.fil";
  int err=0;

  // Create filbank struct
  fil_t* filbank = create_fil(fname, &err, 
      SIGPROC_CREATE_FIL_WRITE);

  // Convert to sigproc's ra/dec format
  char ra[64] = "15:23:12.43";
  char dec[64] = "-34:21:23.54";

  double ra_sp, dec_sp = 0;
  err = hhmmss_to_sigproc(ra, &ra_sp);
  if (err)
  {
    fprintf(stderr, "Error in hhmmss\n");
    return EXIT_FAILURE;
  }
  err = ddmmss_to_sigproc(dec, &dec_sp);
  if (err)
  {
    fprintf(stderr, "Error in ddmmss\n");
    return EXIT_FAILURE;
  }
  // ra_sp  = 152312.43
  // dec_sp = -342123.54

  // Populate header parameters
  filbank->header->tstart = 59699.54; // MJD
  filbank->header->machine_id = -1;   // Machine ID
  filbank->header->telescope_id = -1; // Telescope ID
  filbank->header->src_raj = ra_sp;   // RA in weird sigproc format
  filbank->header->src_dej = dec_sp;  // declination in sigproc format
  filbank->header->az_start = 40;     // azimuth
  filbank->header->za_start = 35;     // zenith angle
  filbank->header->foff = 0.5;        // channel width, MHz
  filbank->header->fch1 = 1400;       // Frequency of channel 1
  filbank->header->nchans = 100;      // Number of channels in file
  filbank->header->nifs = 1;          // Number of IF (stokes I => IF = 1)
  filbank->header->nbits = 8;         // Bit size
  filbank->header->tsamp = 64e-6;     // Sampling time in seconds
  filbank->header->nbeams = 1;        // Nbeams 
  filbank->header->ibeam = 0;         // Beam number 

  // Define source name, will go in header
  sprintf(filbank->header->source_name, "%s", "J0000+0000");

  // write the header
  write_header(*filbank);

  // write some 8bit data
  // data dimensions must be:
  // fastest ---> slowest
  // freq -> IF -> time
  size_t nsamples = 5000;
  char* some_data; 
  some_data = malloc(filbank->header->nchans * nsamples * sizeof *some_data);

  memset(some_data, filbank->header->nchans * nsamples, sizeof *some_data);

  size_t nwrite = fwrite(some_data, sizeof *some_data,
      filbank->header->nchans * nsamples, filbank->file);


  // Now destroy the fil struct
  // de-allocates memory and closes file
  err = destroy_fil(filbank);

  return EXIT_SUCCESS;
}
