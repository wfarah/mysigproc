#ifndef __MYSIGPROC_DEF_H
#define __MYSIGPROC_DEF_H

#define SIGPROC_ORDER_TF 1001
#define SIGPROC_ORDER_FT 1002

#define SIGPROC_CREATE_FIL_WRITE 1
#define SIGPROC_CREATE_FIL_READ 2

typedef struct sigproc_header_t {
    char rawdatafile[80], source_name[80], file_name[256];
    int machine_id, telescope_id, data_type, nchans, nbits, nifs, scan_number,
        barycentric,pulsarcentric; /* these two added Aug 20, 2004 DRL */
    double tstart,mjdobs,tsamp,fch1,foff,refdm,az_start,za_start,src_raj,src_dej;
    double gal_l,gal_b,header_tobs,raw_fch1,raw_foff;
    int nbeams, ibeam;
    /* added 20 December 2000    JMC */
    double srcl,srcb;
    double ast0, lst0;
    long wapp_scan_number;
    char project[8];
    char culprits[24];
    double analog_power[2];

    /* added frequency table for use with non-contiguous data */
    double frequency_table[4096]; /* note limited number of channels */
    long int npuls; /* added for binary pulse profile format */

    double period;
    int nbins;
    int itmp,nbytes,totalbytes,expecting_rawdatafile,expecting_source_name;
    int expecting_frequency_table,channel_index;
    long int nsamples;
} sigproc_header_t;


typedef struct fil_t {
    sigproc_header_t* header;
    FILE * file;
} fil_t;

#endif
