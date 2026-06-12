#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"



/* Function Definitions */
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}
/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    uint size;
    //move to end of file
    fseek(fptr,0,SEEK_END);
    //Get current position(file size)
    size = ftell(fptr);
    //move back to begining
    rewind(fptr);
    return size;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char *ext;
    //check source image have .bmp extension
    ext = strstr(argv[2],".bmp");
    if(ext!=NULL && strcmp(ext,".bmp")==0)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
    printf("Error:Source file should contain .bmp extention\n");
    return e_failure;
    }
    //check secret file .txt extension
     ext = strrchr(argv[3],'.');
    if(ext!=NULL)
    {
        if(strcmp(ext,".txt")==0 || strcmp(ext,".c")==0 || strcmp(ext,".sh")==0)
        {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file,ext);
        }
        else
        {
            printf("Error:Secret file should contain .txt/.c/.sh extention\n");
            return e_failure;
        }
    }
    else
    {
    printf("Error:Secret file has no extension\n");
    return e_failure;
    }
    
    //check output file
    if(argv[4]==NULL)
    {
        encInfo->dest_image_fname = "default.bmp";
    }
    //check destination image have .bmp extension
    else
    {
       ext = strstr(argv[4],".bmp");
    if(ext!=NULL && strcmp(ext,".bmp")==0)
    {
        encInfo->dest_image_fname = argv[4];
    }
    else 
     {
        printf("Error:Destination file should contain .bmp extenstion\n");
        return e_failure;
     }
    }
     return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_dest_image = fopen(encInfo->dest_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_dest_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->dest_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
   //Get image capacity
   encInfo->image_capacity =  get_image_size_for_bmp(encInfo->fptr_src_image);
   //Get secret file size
   encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
   //Check image capacity
   int required_size = (strlen(MAGIC_STRING)+sizeof(int)+strlen(encInfo->extn_secret_file)+sizeof(int)+encInfo->size_secret_file)*8;
   if(encInfo->image_capacity >= required_size)
      return e_success;
   else 
      return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    //rewind the source image position
    rewind(fptr_src_image);
    //declare buffer
    char buffer[54];
    //read data from source image and store into buffer
    fread(buffer,54,1,fptr_src_image);
    //write the data from buffer to destination
    fwrite(buffer,54,1,fptr_dest_image);
    //check the offset of the both
    if(ftell(fptr_src_image)==ftell(fptr_dest_image))
      return e_success;
    else 
      return e_failure;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    //Declare image buffer with 8bytes
    char imageBuffer[8];
    //run loop upto magic string size
    for(int i=0;magic_string[i];i++)
    {
        //Read 8 bytes from src image store into the imageBuffer
        fread(imageBuffer,8,1,encInfo->fptr_src_image);
        //call the function encode_byte_to_lsb
        encode_byte_to_lsb(magic_string[i],imageBuffer);
        //write 8 bytes from imageBuffer into the destination file
        fwrite(imageBuffer,8,1,encInfo->fptr_dest_image);
    }
    //check offset of both file pointer
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image))
      return e_success;
    return e_failure;

}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char imageBuffer[32];
    //Read 32 bytes from src image store into the imageBuffer
    fread(imageBuffer,32,1,encInfo->fptr_src_image);
     //call the function encode_size_to_lsb
    encode_size_to_lsb(size,imageBuffer);
    //write 8 bytes from imageBuffer into the destination file
    fwrite(imageBuffer,32,1,encInfo->fptr_dest_image);
    //check offset of both file pointer
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image))
      return e_success;
    return e_failure;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    //Declare image buffer
    char imageBuffer[8];
    //run loop upto file extn size
    for(int i=0;file_extn[i];i++)
    {
        //Read 8 bytes from src image store into the imageBuffer
        fread(imageBuffer,8,1,encInfo->fptr_src_image);
        //call the function eencode_byte_to_lsb
        encode_byte_to_lsb(file_extn[i],imageBuffer);
        //write 8 bytes from imageBuffer into the destination file
        fwrite(imageBuffer,8,1,encInfo->fptr_dest_image);
    }
    //check offset of both file pointer
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image))
      return e_success;
    return e_failure;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char imageBuffer[32];
    //Read 32 bytes from src image store into the imageBuffer
    fread(imageBuffer,32,1,encInfo->fptr_src_image);
    //call the function encode_size_to_lsb
    encode_size_to_lsb(file_size,imageBuffer);
    //write 8 bytes from imageBuffer into the destination file
    fwrite(imageBuffer,32,1,encInfo->fptr_dest_image);
    //check offset of both file pointer
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image))
      return e_success;
    return e_failure;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //rewind secret file pointer
    rewind(encInfo->fptr_secret);
    //Declare the secret file data
    char secret_file_data[encInfo->size_secret_file];
    //Read the data from secret file up to the size times and store into secret_file_data
    fread(secret_file_data,encInfo->size_secret_file,1,encInfo->fptr_secret);
    //Declare image buffer
    char imageBuffer[8];
    //run loop upto file extn size
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
        //Read 8 bytes from src image store into the imageBuffer
        fread(imageBuffer,8,1,encInfo->fptr_src_image);
        //call the function eencode_byte_to_lsb
        encode_byte_to_lsb(secret_file_data[i],imageBuffer);
        //write 8 bytes from imageBuffer into the destination file
        fwrite(imageBuffer,8,1,encInfo->fptr_dest_image);
    }
    //check offset of both file pointer
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_dest_image))
      return e_success;
    return e_failure;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    //Variable to store one byte from source image
    char ch;
    //read one byte at a time until end of file
    while(fread(&ch,1,1,fptr_src)==1)
    {
        //write the same byte into destination image
        fwrite(&ch,1,1,fptr_dest);
    }
    //check if loop stopped because of end of file reached
    if(feof(fptr_src))
     return e_success;  //Return success if all data copied
    else 
     return e_failure; //Return failure if any error occured
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    //Generate the loop 8 times
    for(int i=0;i<8;i++)
    {
        //Get the bit from an data and clearing the lsb bit of image buffer
        int bit = (data>>(7-i))&1;
        //set the bit into the lsb of image buffer
        image_buffer[i] = (image_buffer[i] & 0xFE) | bit;
    }
    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    //Generate the loop 32 times
    for(int i=0;i<32;i++)
    {
        //Get the bit from an data and clear the bit from lsb of image buffer
        int bit = (size >> (31-i)) & 1;
        //Set the bit lsb if image buffer
        imageBuffer[i] = (imageBuffer[i] & 0xFE) | bit;
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    //call open_files function
    if(open_files(encInfo)==e_success)
    {
        printf("Files opened successfully\n");
    }
    else 
      return e_failure;
    //call check_capacity function
    if(check_capacity(encInfo)==e_success)
    {
        printf("Check image capacity successful\n");
    }
    else 
    {
        printf("Error:Insufficient image capacity\n");
        return e_failure;
    }
    //call the copy_bmp_header function
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_dest_image)==e_success)
    {
        printf("BMP header copied successfully\n");
    }
    else 
    {
       printf("Error:Failed to copy BMP header\n");
         return e_failure;
    }
    //Encode magic string
    
    if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
    {
        printf("Magic string is encoded successfully\n");
    }
    else
    {
        printf("Error:Failed to encode magic string\n");
        return e_failure;
    }
    //Encode secret file extension size
    int extn_size = strlen(encInfo->extn_secret_file);
    if(encode_secret_file_extn_size(extn_size,encInfo)==e_success)
    {
        printf("Secret file extension size encoded successfully\n");
    }
    else
    {
        printf("Error:Failed to encode secret file extension size\n");
        return e_failure;
    }
    //Encode secret file extension
    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
    {
        printf("Secret file extension encoded successfully\n");
    }
    else 
    {
        printf("Error:Failed to encode secret file extension\n");
        return e_failure;
    }
    //Encode secret file size
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
    {
        printf("Secret file size encoded successfully\n");
    }
    else 
    {
        printf("Error:Failed to encode secret file size\n");
        return e_failure;
    }
    //Encode secret file data
    if(encode_secret_file_data(encInfo)==e_success)
    {
        printf("Secret file data encoded successfully\n");
    }
    else 
    {
        printf("Error:Failed to encode secret file data\n");
        return e_failure;
    }
    //Copy remaining image data
    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_dest_image)==e_success)
    {
        printf("Remaining image data copied successfully\n");
    }
    else 
    {
        printf("Error:Failed copy remaining image data\n");
        return e_failure;
    }
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_dest_image);
    fclose(encInfo->fptr_secret);
    //Encoding success
    return e_success;
    
     
}
