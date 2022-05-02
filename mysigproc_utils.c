/* read_header.c - general (modified) handling routines for SIGPROC headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <assert.h>

#ifndef __MYSIGPROC_DEF_H
#include "mysigproc_def.h"
#endif

void unpack_2bit(unsigned char, float*);

/* read a string from the input which looks like nchars-char[1-nchars] */
void get_string(FILE *inputfile, int *nbytes, char string[]) /* includefile */
{
  int nchar;
  size_t fread_sz;
  strcpy(string,"ERROR");
  fread_sz = fread(&nchar, sizeof(int), 1, inputfile);
  *nbytes=sizeof(int);
  if (feof(inputfile)) exit(0);
  if (nchar>80 || nchar<1) return;
  fread_sz = fread(string, nchar, 1, inputfile);
  string[nchar]='\0';
  *nbytes+=nchar;
}


/* attempt to read in the general header info from a pulsar data file */
int get_header(char *fil_direc, sigproc_header_t* header) /* includefile */
{
  //struct sigproc_header_t header;
  char string[80], message[256];
  header->expecting_rawdatafile=0;
  header->expecting_source_name=0; 
  header->expecting_frequency_table=0;
  int nbytes;
  long int sz;
  size_t fread_sz;
  strcpy(header->file_name,fil_direc);
  FILE * inputfile = fopen(fil_direc,"rb");
  if (inputfile==NULL){
	  fprintf(stderr,"Error opening file: %s\n",fil_direc);
      return -1;
  }
  /* try to read in the first line of the header */
  get_string(inputfile,&nbytes,string);
  if (strcmp(string,"HEADER_START")!=0) {
	/* the data file is not in standard format, rewind and return */
    fprintf(stderr,"Error: %s is not in standard sigproc form, could not read HEADER_START\n",fil_direc);
	rewind(inputfile);
    return -1;
  }
  /* store total number of bytes read so far */
  header->totalbytes=nbytes;
  /* loop over and read remaining header lines until HEADER_END reached */
  while (1) {
    get_string(inputfile,&nbytes,string);
    if (strcmp(string,"HEADER_END")==0) break;
    header->totalbytes+=nbytes;
    if (strcmp(string,"rawdatafile")==0) {
      header->expecting_rawdatafile=1;
    } else if (strcmp(string,"source_name")==0) {
      header->expecting_source_name=1;
    } else if (strcmp(string,"FREQUENCY_START")==0) {
      header->expecting_frequency_table=1;
      header->channel_index=0;
    } else if (strcmp(string,"FREQUENCY_END")==0) {
      header->expecting_frequency_table=0;
    } else if (strcmp(string,"az_start")==0) {
      fread_sz = fread(&header->az_start,sizeof(header->az_start),1,inputfile);
      header->totalbytes+=sizeof(header->az_start);
    } else if (strcmp(string,"za_start")==0) {
      fread_sz = fread(&header->za_start,sizeof(header->za_start),1,inputfile);
      header->totalbytes+=sizeof(header->za_start);
    } else if (strcmp(string,"src_raj")==0) {
      fread_sz = fread(&header->src_raj,sizeof(header->src_raj),1,inputfile);
      header->totalbytes+=sizeof(header->src_raj);
    } else if (strcmp(string,"src_dej")==0) {
      fread_sz = fread(&header->src_dej,sizeof(header->src_dej),1,inputfile);
      header->totalbytes+=sizeof(header->src_dej);
    } else if (strcmp(string,"tstart")==0) {
      fread_sz = fread(&header->tstart,sizeof(header->tstart),1,inputfile);
      header->totalbytes+=sizeof(header->tstart);
    } else if (strcmp(string,"tsamp")==0) {
      fread_sz = fread(&header->tsamp,sizeof(header->tsamp),1,inputfile);
      header->totalbytes+=sizeof(header->tsamp);
    } else if (strcmp(string,"period")==0) {
      fread_sz = fread(&header->period,sizeof(header->period),1,inputfile);
      header->totalbytes+=sizeof(header->period);
    } else if (strcmp(string,"fch1")==0) {
      fread_sz = fread(&header->fch1,sizeof(header->fch1),1,inputfile);
      header->totalbytes+=sizeof(header->fch1);
    } else if (strcmp(string,"fchannel")==0) {
      fread_sz = fread(&header->frequency_table[header->channel_index++],sizeof(double),1,inputfile);
      header->totalbytes+=sizeof(double);
      header->fch1=header->foff=0.0; /* set to 0.0 to signify that a table is in use */
    } else if (strcmp(string,"foff")==0) {
      fread_sz = fread(&header->foff,sizeof(header->foff),1,inputfile);
      header->totalbytes+=sizeof(header->foff);
    } else if (strcmp(string,"nchans")==0) {
      fread_sz = fread(&header->nchans,sizeof(header->nchans),1,inputfile);
      header->totalbytes+=sizeof(header->nchans);
    } else if (strcmp(string,"telescope_id")==0) {
      fread_sz = fread(&header->telescope_id,sizeof(header->telescope_id),1,
          inputfile);
      header->totalbytes+=sizeof(header->telescope_id);
    } else if (strcmp(string,"machine_id")==0) {
      fread_sz = fread(&header->machine_id,sizeof(header->machine_id),1,
          inputfile);
      header->totalbytes+=sizeof(header->machine_id);
    } else if (strcmp(string,"data_type")==0) {
      fread_sz = fread(&header->data_type,sizeof(header->data_type),1,
          inputfile);
      header->totalbytes+=sizeof(header->data_type);
    } else if (strcmp(string,"ibeam")==0) {
      fread_sz = fread(&header->ibeam,sizeof(header->ibeam),1,inputfile);
      header->totalbytes+=sizeof(header->ibeam);
    } else if (strcmp(string,"nbeams")==0) {
      fread_sz = fread(&header->nbeams,sizeof(header->nbeams),1,inputfile);
      header->totalbytes+=sizeof(header->nbeams);
    } else if (strcmp(string,"nbits")==0) {
      fread_sz = fread(&header->nbits,sizeof(header->nbits),1,inputfile);
      header->totalbytes+=sizeof(header->nbits);
    } else if (strcmp(string,"barycentric")==0) {
      fread_sz = fread(&header->barycentric,sizeof(header->barycentric),1,
          inputfile);
      header->totalbytes+=sizeof(header->barycentric);
    } else if (strcmp(string,"pulsarcentric")==0) {
      fread_sz = fread(&header->pulsarcentric,sizeof(header->pulsarcentric),1,
          inputfile);
      header->totalbytes+=sizeof(header->pulsarcentric);
    } else if (strcmp(string,"nbins")==0) {
      fread_sz = fread(&header->nbins,sizeof(header->nbins),1,inputfile);
      header->totalbytes+=sizeof(header->nbins);
    } else if (strcmp(string,"nsamples")==0) {
      /* read this one only for backwards compatibility */
      fread_sz = fread(&header->itmp,sizeof(header->itmp),1,inputfile);
      header->totalbytes+=sizeof(header->itmp);
    } else if (strcmp(string,"nifs")==0) {
      fread_sz = fread(&header->nifs,sizeof(header->nifs),1,inputfile);
      header->totalbytes+=sizeof(header->nifs);
    } else if (strcmp(string,"npuls")==0) {
      fread_sz = fread(&header->npuls,sizeof(header->npuls),1,inputfile);
      header->totalbytes+=sizeof(header->npuls);
    } else if (strcmp(string,"refdm")==0) {
      fread_sz = fread(&header->refdm,sizeof(header->refdm),1,inputfile);
      header->totalbytes+=sizeof(header->refdm);
    } else if (header->expecting_rawdatafile) {
      strcpy(header->rawdatafile,string);
      header->expecting_rawdatafile=0;
    } else if (header->expecting_source_name) {
      strcpy(header->source_name,string);
      header->expecting_source_name=0;
    } else {
      sprintf(message,"read_header - unknown parameter: %s\n",string);
      fprintf(stderr,"ERROR: %s\n",message);
      exit(1);
    }
    if (header->totalbytes != ftell(inputfile)){
        fprintf(stderr,"ERROR: Header bytes does not equal file position\n");
        fprintf(stderr,"String was: '%s'\n",string);
        fprintf(stderr,"       header: %d file: %lu\n",header->totalbytes,ftell(inputfile));
        exit(1);
    }
  } 
  /* add on last header string */
  header->totalbytes+=nbytes;
  fseek(inputfile, 0L, SEEK_END);
  sz = ftell(inputfile);
  header->nsamples = (sz - header->totalbytes)/(header->nchans*header->nbits/8);
  //header->nbytes
  fclose(inputfile);
  /* return total number of bytes read */
  //return totalbytes;
  //return header;
  return 0;
}


void send_string(char *string, FILE* output) /* includefile */
{
  int len;
  len=strlen(string);
  fwrite(&len, sizeof(int), 1, output);
  fwrite(string, sizeof(char), len, output);
  /*fprintf(stderr,"%s\n",string);*/
}

void send_float(char *name,float floating_point,FILE * output) /* includefile */
{
  send_string(name,output);
  fwrite(&floating_point,sizeof(float),1,output);
  /*fprintf(stderr,"%f\n",floating_point);*/
}

void send_double (char *name, double double_precision,FILE *output) /* includefile */
{
  send_string(name,output);
  fwrite(&double_precision,sizeof(double),1,output);
  /*fprintf(stderr,"%f\n",double_precision);*/
}

void send_int(char *name, int integer,FILE* output) /* includefile */
{
  send_string(name,output);
  fwrite(&integer,sizeof(int),1,output);
  /*fprintf(stderr,"%d\n",integer);*/
}

void send_long(char *name, long integer,FILE* output) /* includefile */
{
  send_string(name,output);
  fwrite(&integer,sizeof(long),1,output);
}

void send_coords(double raj, double dej, double az, double za, FILE* output) /*includefile*/
{
  if ((raj != 0.0) || (raj != -1.0)) send_double("src_raj",raj,output);
  if ((dej != 0.0) || (dej != -1.0)) send_double("src_dej",dej,output);
  if ((az != 0.0)  || (az != -1.0))  send_double("az_start",az,output);
  if ((za != 0.0)  || (za != -1.0))  send_double("za_start",za,output);
}


//int write_header(Header_t header, FILE *output)
int write_header(fil_t fil)
{

  send_string("HEADER_START",fil.file);
  send_string("rawdatafile",fil.file);
  send_string("",fil.file);
  if (strcmp(fil.header->source_name,""))
  {
    send_string("source_name",fil.file);
    send_string(fil.header->source_name,fil.file);
  }
  send_int("machine_id",fil.header->machine_id,fil.file);
  send_int("telescope_id",fil.header->telescope_id,fil.file);
  send_coords(fil.header->src_raj,fil.header->src_dej,fil.header->az_start,fil.header->za_start,fil.file);

  /* filterbank data */
  send_int("data_type",1,fil.file);
  send_double("fch1",fil.header->fch1,fil.file);
  send_double("foff",fil.header->foff,fil.file);
  send_int("nchans",fil.header->nchans,fil.file);

  /* beam info */
  send_int("nbeams",fil.header->nbeams,fil.file);
  send_int("ibeam",fil.header->ibeam,fil.file);
  /* number of bits per sample */
  send_int("nbits",fil.header->nbits,fil.file);
  /* start time and sample interval */
  send_double("tstart",fil.header->tstart,fil.file);
  send_double("tsamp",fil.header->tsamp,fil.file);

  send_int("nifs",fil.header->nifs,fil.file);
  send_string("HEADER_END",fil.file);

  return EXIT_SUCCESS;

}

/*
int read_block_TF(uint64_t sample, uint64_t nsamples, fil_t* filterbank, 
    const int order, float * buff)
{
  fseek(filterbank->file,filterbank->header->totalbytes+
      sample*filterbank->header->nchans,SEEK_SET);
  int i,chan;
  switch(filterbank->header->nbits)
  {
    case 8:
      for(i=0;i<nsamples;i++)
      {
        for(chan=0;chan<filterbank->header->nchans;chan++)
        {
          buff[chan*nsamples+i] = (float) fgetc(filterbank->file);
        }
      }
      break;

    default:
      fprintf(stderr,"ERROR: read_block: %i nbits is (currently) not supported",
          filterbank->header->nbits);
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
*/

int read_block(uint64_t start_sample, uint64_t nsamples, fil_t* filterbank, 
    const int order, float * buff)
{
  size_t fread_sz;
  if (start_sample+nsamples > filterbank->header->nsamples)
  {
    fprintf(stderr,
        "ERROR: sigproc: read_block: cannot read past %"PRIu64" samples\n",
        filterbank->header->nsamples);
    return EXIT_FAILURE;
  }

  fseek(filterbank->file,filterbank->header->totalbytes+
      start_sample*filterbank->header->nchans,SEEK_SET);

  int i,chan;
  switch(filterbank->header->nbits)
  {
    case 8:
      if (order == SIGPROC_ORDER_FT)
      {
        for(i=0;i<nsamples;i++)
        {
          for(chan=0;chan<filterbank->header->nchans;chan++)
          {
            buff[i*filterbank->header->nchans+chan] = (float) fgetc(filterbank->file);
          }
        }
      }
      else if (order == SIGPROC_ORDER_TF)
      {
        for(i=0;i<nsamples;i++)
        {
          for(chan=0;chan<filterbank->header->nchans;chan++)
          {
            buff[chan*nsamples+i] = (float) fgetc(filterbank->file);
          }
        }
      }
      else
      {
        fprintf(stderr,"ERROR: read_block: wrong order (4th argument)");
        return EXIT_FAILURE;
      }
      break;

    case 32:
      if (order == SIGPROC_ORDER_TF)
        fread_sz = fread(buff, 4, 
            filterbank->header->nchans*nsamples, filterbank->file);

      else if (order == SIGPROC_ORDER_FT)
      {
        for(i=0;i<nsamples;i++)
        {
          for(chan=(filterbank->header->nchans -1); chan>=0; chan--)
          {
            fread_sz = fread(&buff[chan*nsamples+i], 4, 1, filterbank->file);
          }
        }
      }
      else
      {
        fprintf(stderr,"ERROR: read_block: wrong order (4th argument)");
        return EXIT_FAILURE;
      }
      break;

    default:
      fprintf(stderr,"ERROR: read_block: %i nbits is (currently) not supported",
          filterbank->header->nbits);
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


int read_block_rescale(uint64_t start_sample, uint64_t nsamples, fil_t* filterbank, 
    const int order, float * buff, float mean, float stdev)
{
  size_t fread_sz;
  if (start_sample+nsamples > filterbank->header->nsamples)
  {
    fprintf(stderr,
        "ERROR: sigproc: read_block: cannot read past %"PRIu64" samples\n",
        filterbank->header->nsamples);
    return EXIT_FAILURE;
  }

  fseek(filterbank->file,filterbank->header->totalbytes+
      start_sample*filterbank->header->nchans*filterbank->header->nbits/8,
      SEEK_SET);

  float tmp_2bit[4];
  unsigned char tmp = 0;
  int bit_count = 8;
  int i,chan;
  switch(filterbank->header->nbits)
  {
    case 8:
      if (order == SIGPROC_ORDER_TF)
      {
        for(i=0;i<nsamples;i++)
        {
          for(chan=0;chan<filterbank->header->nchans;chan++)
          {
            buff[i*filterbank->header->nchans+chan] = (((float) fgetc(filterbank->file)) - mean)/stdev;
          }
        }
      }
      else if (order == SIGPROC_ORDER_FT)
      {
        for(i=0;i<nsamples;i++)
        {
          for(chan=0;chan<filterbank->header->nchans;chan++)
          {
            buff[chan*nsamples+i] = (((float) fgetc(filterbank->file)) - mean)/stdev;
          }
        }
      }
      else
      {
        fprintf(stderr,"ERROR: read_block: wrong order (4th argument)");
        return EXIT_FAILURE;
      }
      break;

    case 2:
      if (order == SIGPROC_ORDER_TF)
      {
        for(i=0;i<nsamples;i++)
        {
          for(chan=0;chan<filterbank->header->nchans;chan++)
          {
            if (bit_count >= 4)
            {
              tmp = fgetc(filterbank->file);
              unpack_2bit(tmp, tmp_2bit);
              bit_count = 0;
            }
            buff[i*filterbank->header->nchans+chan] = (tmp_2bit[bit_count] - mean)/stdev;
            bit_count += 1;
          }
        }
      }
      else if (order == SIGPROC_ORDER_FT)
      {
        for(i=0;i<nsamples;i++)
        {
          for(chan=0;chan<filterbank->header->nchans;chan++)
          {
            if (bit_count >= 4)
            {
              tmp = fgetc(filterbank->file);
              unpack_2bit(tmp, tmp_2bit);
              bit_count = 0;
            }
            buff[chan*nsamples+i] = (tmp_2bit[bit_count] - mean)/stdev;
            bit_count += 1;
          }
        }
      }
      else
      {
        fprintf(stderr,"ERROR: read_block: wrong order (4th argument)");
        return EXIT_FAILURE;
      }
      break;

    case 32:
      if (order == SIGPROC_ORDER_TF)
      {
        fread_sz = fread(buff, 4, filterbank->header->nchans*nsamples, 
            filterbank->file);
        for (i=0; i<filterbank->header->nchans*nsamples; i++)
          buff[i] = (buff[i] - mean)/stdev;
      }
      else if (order == SIGPROC_ORDER_FT)
      {
        for(i=0;i<nsamples;i++)
        {
//          for(chan=0;chan<filterbank->header->nchans;chan++)
          for(chan=(filterbank->header->nchans - 1); chan>=0; chan--)
          {
            fread_sz = fread(&buff[chan*nsamples+i], 4, 1, filterbank->file);
            buff[chan*nsamples+i] = (buff[chan*nsamples+i] - mean)/stdev;
          }
        }
      }
      else
      {
        fprintf(stderr,"ERROR: read_block: wrong order (4th argument)");
        return EXIT_FAILURE;
      }
      break;

    default:
      fprintf(stderr,"ERROR: read_block: %i nbits is (currently) not supported",
          filterbank->header->nbits);
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int read_block_rescale_inv_chan(uint64_t start_sample, uint64_t nsamples, fil_t* filterbank, 
    const int order, float * buff, float mean, float stdev)
{
  size_t fread_sz;
  if (start_sample+nsamples > filterbank->header->nsamples)
  {
    fprintf(stderr,
        "ERROR: sigproc: read_block: cannot read past %"PRIu64" samples\n",
        filterbank->header->nsamples);
    return EXIT_FAILURE;
  }

  fseek(filterbank->file,filterbank->header->totalbytes+
      start_sample*filterbank->header->nchans*filterbank->header->nbits/8,
      SEEK_SET);

  float tmp_2bit[4];
  unsigned char tmp = 0;
  int bit_count = 8;
  int i,chan;
  switch(filterbank->header->nbits)
  {
    case 8:
      if (order == SIGPROC_ORDER_TF)
      {
        for(i=0;i<nsamples;i++)
        {
          for(chan=(filterbank->header->nchans - 1);chan>=0;chan--)
          {
            buff[i*filterbank->header->nchans+chan] = (((float) fgetc(filterbank->file)) - mean)/stdev;
          }
        }
      }
      else if (order == SIGPROC_ORDER_FT)
      {
        for(i=0;i<nsamples;i++)
        {
          for(chan=(filterbank->header->nchans - 1);chan>=0;chan--)
          {
            buff[chan*nsamples+i] = (((float) fgetc(filterbank->file)) - mean)/stdev;
          }
        }
      }
      else
      {
        fprintf(stderr,"ERROR: read_block: wrong order (4th argument)");
        return EXIT_FAILURE;
      }
      break;
    
    case 32:
      if (order == SIGPROC_ORDER_TF)
      {
        for(i=0;i<nsamples;i++)
        {
          for(chan=(filterbank->header->nchans - 1);chan>=0;chan--)
          {
            fread_sz = fread(&buff[i*filterbank->header->nchans+chan], 
                4, 1, filterbank->file);
            buff[i*filterbank->header->nchans+chan] = (buff[i*filterbank->header->nchans+chan] - mean)/stdev;
          }
        }
        //fread(buff, 4, filterbank->header->nchans*nsamples, filterbank->file);
        //for (i=0; i<filterbank->header->nchans*nsamples; i++)
        //  buff[i] = (buff[i] - mean)/stdev;
      }
      else if (order == SIGPROC_ORDER_FT)
      {
        for(i=0;i<nsamples;i++)
        {
          for(chan=(filterbank->header->nchans -1); chan>=0; chan--)
          {
            fread_sz = fread(&buff[chan*nsamples+i], 4, 1, filterbank->file);
            buff[chan*nsamples+i] = (buff[chan*nsamples+i] - mean)/stdev;
          }
        }
      }
      else
      {
        fprintf(stderr,"ERROR: read_block: wrong order (4th argument)");
        return EXIT_FAILURE;
      }
      break;

    case 2:
      if (order == SIGPROC_ORDER_TF)
      {
        for (i=0; i<nsamples; i++)
        {
          for(chan=(filterbank->header->nchans - 1); chan>=0; chan--)
          {
            if (bit_count >= 4)
            {
              fread_sz = fread(&tmp, 1, 1, filterbank->file);
              unpack_2bit(tmp, tmp_2bit);
              bit_count = 0;
            }
            buff[i*filterbank->header->nchans+chan] = (tmp_2bit[bit_count] - mean)/stdev;
            bit_count += 1;
          }
        }
      }
      else if (order == SIGPROC_ORDER_FT)
      {
        for (i=0; i<nsamples; i++)
        {
          for (chan=(filterbank->header->nchans -1); chan>=0; chan--)
          {
            if (bit_count >= 4)
            {
              fread_sz = fread(&tmp, 1, 1, filterbank->file);
              unpack_2bit(tmp, tmp_2bit);
              bit_count = 0;
            }
            buff[chan*nsamples+i] = (tmp_2bit[bit_count] - mean)/stdev;
            bit_count += 1;
          }
        }
      }
      else
      {
        fprintf(stderr,"ERROR: read_block: wrong order (4th argument)");
        return EXIT_FAILURE;
      }
      break;

    default:
      fprintf(stderr,"ERROR: read_block: %i nbits is (currently) not supported",
          filterbank->header->nbits);
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

// quick and dirty way
void unpack_2bit(unsigned char tmp, float* tmp_2bit)
{
  unsigned int i;
  for (i=0; i<4; i++)
    tmp_2bit[i] = (float) ((tmp >> 2*i) & (0x03)) ;
}

// same as mopsr_delays_hhmmss_to_sigproc
int hhmmss_to_sigproc (char * hhmmss, double * sigproc)
{
  int ihour = 0;
  int imin = 0;
  double sec = 0;
  const char *sep = ":";
  char * saveptr;

  char * copy = (char *) malloc (strlen(hhmmss) + 1);
  strcpy (copy, hhmmss);

  char * str = strtok_r(copy, sep, &saveptr);
  if (str != NULL)
  {
    if (sscanf(str, "%d", &ihour) != 1)
      return -1;

    str = strtok_r(NULL, sep, &saveptr);
    if (str != NULL)
    {
      if (sscanf(str, "%d", &imin) != 1)
        return -1;

      str = strtok_r(NULL, sep, &saveptr);
      if (str != NULL)
      {
        if (sscanf(str, "%lf", &sec) != 1)
          return -1;
      }
    }
  }
  free (copy);

  char s = '\0';
  if (ihour < 0)
  {
    ihour *= -1;
    s = '-';
  }

  *sigproc = ((double)ihour*1e4  + (double)imin*1e2 + sec);
  return 0;
}

// Same as mopsr_delays_ddmmss_to_sigproc
int ddmmss_to_sigproc (char * ddmmss, double * sigproc)
{
  int ideg = 0;
  int iamin = 0;
  double asec = 0;
  const char *sep = ":";
  char * saveptr;

  char * copy = (char *) malloc (strlen(ddmmss) + 1);
  strcpy (copy, ddmmss);

  char * str = strtok_r(ddmmss, sep, &saveptr);
  if (str != NULL)
  {
    if (sscanf(str, "%d", &ideg) != 1)
      return -1;

    str = strtok_r(NULL, sep, &saveptr);
    if (str != NULL)
    {
      if (sscanf(str, "%d", &iamin) != 1)
        return -1;

      str = strtok_r(NULL, sep, &saveptr);
      if (str != NULL)
      {
        if (sscanf(str, "%lf", &asec) != 1)
          return -1;
      }
    }
  }

  free (copy);

  if (ideg < 0)
    *sigproc = ((double) ideg*1e4 - (double) iamin*1e2) - asec;
  else
    *sigproc = ((double) ideg*1e4  + (double) iamin*1e2) + asec;

  return 0;
}

int destroy_fil(fil_t* fil)
{
  fclose(fil->file);
  free(fil->header);
  free(fil);
  return EXIT_SUCCESS;
}

fil_t * create_fil(char* filename, int* err, const int read_or_write)
{
  fil_t* fil = malloc(sizeof(fil_t));
  assert(fil != 0);
  fil->header = malloc(sizeof(sigproc_header_t));
  assert(fil->header != 0);

  if (read_or_write == SIGPROC_CREATE_FIL_READ)
  {
    if ( get_header(filename, fil->header) != 0)
    {
      (*err) = -1;
      return fil;
    }
    fil->file = fopen(filename,"rb");
  }
  else if (read_or_write == SIGPROC_CREATE_FIL_WRITE)
  {
    memset(fil->header, 0, sizeof (sigproc_header_t));
    fil->file = fopen(filename,"wb");
  }


  (*err) = 0;
  return fil;
}
