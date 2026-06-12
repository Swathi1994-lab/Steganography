/*Name:D Swathi
Roll NO:25040B_051
Date:15/03/2026
Project Name:Stegonagraphy
Description:Stegonagraphy is a technique used to hide secret data inside a cover file(such as image,audio or video file)

Encoding:Encoding is the process of hiding secret data inside a cover file (usually an image like .bmp).
         The output image (stego image) looks almost same as original file but contains the hidden message.
         
Decoding:Decoding is the process of retrieving hidden secret data trom the stego image.
         The hidden message is successfully extracted from the image.
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    OperationType type = check_operation_type(argv[1]);
    //check operation is encoding
    if(type==e_encode)
    {
    //check minimum arguments for encoding
     if(argc<4)
    {
         printf("Encoding:./a.out -e <source.bmp file> <secret.txt file> [output.bmp file] \n");
         return e_failure;
    }
      printf("-------------Encoding selected-------------\n");
      //declare structure variable
     EncodeInfo encInfo;
     if(read_and_validate_encode_args(argv,&encInfo)!=e_success)
     {
      printf("Error:Argument validation failed\n");
        return e_failure;
      }
      printf("Arguments validated successfully\n");
      //perform Encoding
      if(do_encoding(&encInfo)!=e_success)
      {
        printf("Error:Encoding failed\n");
        return e_failure;
      }
      printf("----------Encoding completed successfully----------\n");
        return e_success;
      
      }
      else if(type==e_decode)
      {
       //check minimum arguments for decoding
       if(argc<3)
        {
          printf("Decoding:./a.out -d <destination.bmp file> [output file] \n");
         return d_failure;
        }
        printf("------------Decoding selected-------------\n");
        //declare structure variable
        DecodeInfo decInfo;
      if(read_and_validate_decode_args(argv,&decInfo)!=d_success)
      {
      printf("Error:Argument validation failed\n");
        return d_failure;
      }
      printf("Arguments validated successfully\n");
      if(do_decoding(&decInfo)!=d_success)
      {
        printf("Decoding failed\n");
        return d_failure;
      }
      printf("-------------Decoding completed successfully------------\n");
      return d_success; 
    } 
    
    
}
//check operation type is encoding or decoding
OperationType check_operation_type(char *symbol)
{
    if((strcmp(symbol,"-e")==0))
      return e_encode;
    else if(strcmp(symbol,"-d")==0)
      return e_decode;
    else 
      return e_unsupported;
}
