#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <stdlib.h>


/* Function Definitions */

DecodeStatus read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char *ext;
    //check source image have .bmp extension
    ext = strstr(argv[2],".bmp");
    if(ext!=NULL && strcmp(ext,".bmp")==0)
    {
        decInfo->dest_image_fname = argv[2];
    }
    else
    {
    printf("Error:destination file should contain .bmp extention\n");
    return d_failure;
    }
    //checking output file
    if(argv[3]!=NULL)
    {
        //find extension
        char *dot = strrchr(argv[3],'.');
        if(dot!=NULL)
        {
            int len = dot - argv[3];  //length before extension
            strncpy(decInfo->output_fname,argv[3],len);
            decInfo->output_fname[len] = '\0';
        }
        else
        {
             //store into the structure
             strcpy(decInfo->output_fname,argv[3]);
        }
    }
    else
    {
        
        strcpy(decInfo->output_fname,"output"); 
    }
     return d_success;
}

DecodeStatus open_decode_files(DecodeInfo *decInfo)
{
    // destination Image file
    decInfo->fptr_dest_image = fopen(decInfo->dest_image_fname, "rb");
    // Do Error handling
    if (decInfo->fptr_dest_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->dest_image_fname);
        return d_failure;
    }
    // No failure return d_success
    return d_success;
}
DecodeStatus decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char imageBuffer[8];
    int i;
    //run loop upto magic string size
    for(i=0;magic_string[i];i++)
    {
        fread(imageBuffer,8,1,decInfo->fptr_dest_image);
        //Decode one character from the LSB
       decode_byte_from_lsb(&decInfo->magic_string[i],imageBuffer);
    }
    decInfo->magic_string[i]='\0';
    //Comapre with original magic string
    if(strcmp(MAGIC_STRING,decInfo->magic_string)!=0)
    {
        printf("Error:Magic string mismatched\n");
        return d_failure;
    }
    return d_success;
}
DecodeStatus decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char imageBuffer[32];
    //Read 32 bytes from destination image
    fread(imageBuffer,32,1,decInfo->fptr_dest_image);
    //Decode size from LSB
    decode_size_from_lsb(&decInfo->extn_size,imageBuffer);
    return d_success;
}

DecodeStatus decode_secret_file_extn(DecodeInfo *decInfo)
{
    //Declare image buffer
    char imageBuffer[8];
     
    //run loop upto file extn size
    for(int i=0;i<decInfo->extn_size;i++)
    {
        //Read 8 bytes from destination image
        fread(imageBuffer,8,1,decInfo->fptr_dest_image);
        // Decode_byte_to_lsb
        decode_byte_from_lsb(&decInfo->extn_secret_file[i],imageBuffer);
    }
    decInfo->extn_secret_file[decInfo->extn_size] = '\0';
    //check allowed extensions
    strcat(decInfo->output_fname,decInfo->extn_secret_file);
    decInfo->fptr_output = fopen(decInfo->output_fname,"wb");
    if(decInfo->fptr_output == NULL)
    {
        fprintf(stderr,"Error:Unable to open output file\n");
        return d_failure;
    }
    
    return d_success;
    
}

DecodeStatus decode_secret_file_size(DecodeInfo *decInfo)
{
    char imageBuffer[32];
    //Read 32 bytes from src image store into the imageBuffer
    fread(imageBuffer,32,1,decInfo->fptr_dest_image);
    //call the function decode_size_to_lsb
    decode_size_from_lsb((int*)&decInfo->size_secret_file,imageBuffer);
     return d_success;
}

DecodeStatus decode_secret_file_data(DecodeInfo *decInfo)
{
    //Declare image buffer
    char imageBuffer[8];
    char ch;
    //run loop upto file extn size
    for(int i=0;i<decInfo->size_secret_file;i++)
    {
        //Read 8 bytes from src image store into the imageBuffer
        fread(imageBuffer,8,1,decInfo->fptr_dest_image);
        //call the function eencode_byte_to_lsb
       decode_byte_from_lsb(&ch,imageBuffer);
       fwrite(&ch,1,1,decInfo->fptr_output);
        
    }
   return d_success;
    
}
DecodeStatus decode_byte_from_lsb(char *data, char *image_buffer)
{
    *data = 0;
    //Extract LSB bits from 8 bytes
    for(int i=0;i<8;i++)
    {
        *data = *data | ((image_buffer[i]&1)<<(7-i));
    }
    return e_success;
}

DecodeStatus decode_size_from_lsb(int *size, char *imageBuffer)
{
    *size = 0;
    //Extract 32 bits from LSB of image buffer
    for(int i=0;i<32;i++)
    {
        *size = *size|((imageBuffer[i] & 1) << (31-i));
    }
    return e_success;
}

DecodeStatus do_decoding(DecodeInfo *decInfo)
{
    //call open_files function
    if(open_decode_files(decInfo)==d_success)
    {
        printf("Files opened successfully\n");
    }
    else 
     return d_failure;
    //skip 54 bytes BMP header
    fseek(decInfo->fptr_dest_image,54,SEEK_SET);
    //decode magic string
    if(decode_magic_string(MAGIC_STRING,decInfo)==d_success)
    {
        printf("Magic string is decoded successfully\n");
    }
    else
    {
        printf("Error:Failed to decode magic string\n");
        return d_failure;
    }
    if(decode_secret_file_extn_size(decInfo)==d_success)
    {
        printf("Secret file extension size decoded successfully\n");
    }
    else
    {
        printf("Error:Failed to decode secret file extension size\n");
        return d_failure;
    }
    //decode secret file extension
    if(decode_secret_file_extn(decInfo)==d_success)
    {
        printf("Secret file extension decoded successfully\n");
    }
    else 
    {
        printf("Error:Failed to decode secret file extension\n");
        return d_failure;
    }
    if(decode_secret_file_size(decInfo)==d_success)
    {
        printf("Secret file size decoded successfully\n");
    }
    else 
    {
        printf("Error:Failed to decode secret file size\n");
        return d_failure;
    }
    //Encode secret file data
    if(decode_secret_file_data(decInfo)==d_success)
    {
        printf("Secret file data decoded successfully\n");
    }
    else 
    {
        printf("Error:Failed to decode secret file data\n");
        return e_failure;
    }
   
    fclose(decInfo->fptr_dest_image);
    //Decoding success
    return d_success;
    
     
}
