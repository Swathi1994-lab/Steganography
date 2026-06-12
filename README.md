# Steganography Project

## Description
Steganography is a technique of hiding secret information inside an image file.
This project hides a text message inside a BMP image using Least Significant Bit (LSB)
encoding technique and extracts the hidden message during decoding.

## Technologies Used
- C Programming
- File Handling
- Bit Manipulation
- Data Structures
- Command line arguments

## Features
- Encode secret message into image
- Decode hidden message from image
- Preserve original image appearance
- Handle different message sizes

## Working Principle

Encoding:
Message data → Convert into binary → Store bits in image pixels

Decoding:
Read image pixels → Extract hidden bits → Reconstruct message
