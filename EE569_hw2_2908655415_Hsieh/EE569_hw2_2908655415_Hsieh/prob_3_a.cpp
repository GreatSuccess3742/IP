// This sample code reads in image data from a RAW image file and 
// writes it into another file

// NOTE:	The code assumes that the image is of size 256 x 256 and is in the
//			RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;
unsigned char ***Create3D_Array(int a, int b, int c);
void Delete3D_Array(unsigned char ***MyArray, int a, int b, int c);
unsigned char **Create2D_Array(int a, int b);
void Delete2D_Array(unsigned char **MyArray,int a , int b);

int main(int argc, char *argv[])

{
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size = 256;
	
	// Check for proper syntax
	if (argc < 3){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1] [Size = 256]" << endl;
		return 0;
	}
	
	// Check if image is grayscale or color
	if (argc < 4){
		BytesPerPixel = 1; // default is grey image
	} 
	else {
		BytesPerPixel = atoi(argv[(argc-2)]);
		// Check if size is specified
		if (argc >= 5){
			Size = atoi(argv[(argc-1)]);
		}
	}

	// Allocate image data array
	unsigned char ***Imagedata = Create3D_Array(Size,Size,BytesPerPixel);
	
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	for(int i = 0; i < Size; i++){
		for (int j = 0; j < Size; ++j)
		{
			fread(Imagedata[i][j], sizeof(unsigned char), BytesPerPixel, file);
		}
	}
	
	fclose(file);

	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
	
	// Write image data (filename specified by second argument) from image data matrix
	unsigned char *** output = Create3D_Array(Size,Size,BytesPerPixel);

	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	for (int i = 0; i < Size; ++i)
	{
		for(int j = 0 ; j < Size; j++){
			fwrite(output[i][j], sizeof(unsigned char), BytesPerPixel, file);
		}
	}
	
	fclose(file);

	Delete3D_Array(Imagedata,Size,Size,BytesPerPixel);
	Delete3D_Array(output,Size,Size,BytesPerPixel);
	return 0;
}

unsigned char ***Create3D_Array(int a, int b, int c) {
	unsigned char ***MyArray = new unsigned char**[a];
	for (int i = 0; i < a; i++) {
		MyArray[i] = new unsigned char*[b];
		for (int j = 0; j < b; j++) {
			MyArray[i][j] = new unsigned char[c];
		}
	}
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < b; j++) {
			for (int k = 0; k < c; k++) {
				MyArray[i][j][k] = 0;
			}
		}
	}
	return MyArray;
}

void Delete3D_Array(unsigned char ***MyArray, int a, int b, int c) {
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < b; j++) {
			delete[] MyArray[i][j];
		}
		delete[] MyArray[i];
	}
	
	delete[] MyArray;
}
unsigned char **Create2D_Array(int a, int b){
	unsigned char **MyArray = new unsigned char *[a];
	for(int i = 0; i < a; i++){
		MyArray[i] = new unsigned char[b];
	}
	for(int i = 0; i < a; i++){
		for(int j = 0; j < b; j++){
			MyArray[i][j] = 0;
		}
	}
	return MyArray;
}
void Delete2D_Array(unsigned char **MyArray,int a , int b){
	for(int i = 0; i < a; i++){
		delete[] MyArray[i];
	}
	delete[] MyArray;
}
