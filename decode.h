#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>
#include "types.h" // Contains user defined types

typedef struct _DecodeInfo
{
     /* Stego Image Info */
    char *dest_image_fname; // To store the dest file name
    FILE *fptr_dest_image;  // To store the address of stego image
    /* Secret File Info */
    char extn_secret_file[5]; // To store the Secret file extension
    long size_secret_file;    // To store the size of the secret data
    int extn_size;
    char magic_string[10];
    /*Output File Info*/
    char output_fname[20];
    FILE *fptr_output;

} DecodeInfo;

/* Encoding function prototype */

/* Read and validate Encode args from argv */
DecodeStatus read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
DecodeStatus do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
DecodeStatus open_decode_files(DecodeInfo *decInfo);

/* Store Magic String */
DecodeStatus decode_magic_string(const char *magic_string,DecodeInfo *decInfo);

/*Decode extension size*/
DecodeStatus decode_secret_file_extn_size( DecodeInfo *decInfo);

/* Decode secret file extenstion */
DecodeStatus decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
DecodeStatus decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
DecodeStatus decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
DecodeStatus decode_byte_from_lsb(char *data,char *image_buffer);

// decode a size from lsb
DecodeStatus decode_size_from_lsb(int *size,char *imageBuffer);



#endif
