// This sample code reads in image data from a RAW image file and 
// writes it into another file

// NOTE:	The code assumes that the image is of size 256 x 256 and is in the
//			RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;
unsigned char ***Create3D_Array(int a, int b, int c);
void Delete3D_Array(unsigned char ***MyArray, int a, int b, int c);
unsigned char **Create2D_Array(int a, int b);
void Delete2D_Array(unsigned char **MyArray,int a , int b);
void FloydSteinberg(unsigned char ** Input, unsigned char ** Output);
void JJN(unsigned char ** Input, unsigned char ** Output);
void Stucki(unsigned char ** Input, unsigned char ** Output);

int main(int argc, char *argv[])

{
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size = 512;

	// Check for proper syntax
	if (argc < 3) {
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1] [Size = 256]" << endl;
		return 0;
	}

	// Check if image is grayscale or color
	if (argc < 4) {
		BytesPerPixel = 1; // default is grey image
	}
	else {
		BytesPerPixel = atoi(argv[(argc - 2)]);
		// Check if size is specified
		if (argc >= 5) {
			Size = atoi(argv[(argc - 1)]);
		}
	}

	// Allocate image data array

	unsigned char **Imagedata = Create2D_Array(Size, Size);

	// Read image (filename specified by first argument) into image data matrix
	if (!(file = fopen(argv[1], "rb"))) {
		cout << "Cannot open file: " << argv[1] << endl;
		exit(1);
	}
	for (int i = 0; i < Size; i++) {
		fread(Imagedata[i], sizeof(unsigned char), Size, file);
	}

	fclose(file);

	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
	unsigned char ** output = Create2D_Array(Size, Size);

	//FloydSteinberg(Imagedata,output);
	//JJN(Imagedata,output);
	Stucki(Imagedata,output);

	// Write image data (filename specified by second argument) from image data matrix


	if (!(file = fopen(argv[2], "wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	for (int i = 0; i < Size; ++i)
	{
		fwrite(output[i], sizeof(unsigned char), Size, file);
	}

	fclose(file);

	Delete2D_Array(Imagedata, Size, Size);
	Delete2D_Array(output, Size, Size);
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
void FloydSteinberg(unsigned char ** Input, unsigned char ** Output){
	//constructing a  N * N array of double
	double ** Err_Table = new double* [512];
	for(int i = 0; i < 512; i++){
		Err_Table[i] = new double[512];
	}

	for(int i =0; i < 512; i++){
		for(int j =0 ; j< 512;j++){
			Err_Table[i][j] = 0;
		}
	}

	for(int i = 0; i < 512; i++){
		if((i%2) == 0){
			for (int j = 0; j < 512; ++j)
			{
				double value = 0;
				value = round ( ((double)Input[i][j] + Err_Table[i][j]) / 256.0);

				double error = 0;
				if(value == 0){
					Output[i][j] = 0;
					error = (double)Input[i][j];
					if( (i+1) < 512 ){
						Err_Table[i+1][j] = Err_Table[i+1][j] + (error * 7.0 / 16.0);
					}
					if( (i-1) >= 0 && (j+1) <512 ){
						Err_Table[i-1][j+1] = Err_Table[i-1][j+1] + (error * 3.0 / 16.0);
					}
					if( (j+1) < 512 ){
						Err_Table[i][j+1] = Err_Table[i][j+1] + (error * 5.0 / 16.0);
					}
					if( (i+1) < 512 && (j+1) <512 ){
						Err_Table[i+1][j+1] = Err_Table[i+1][j+1] + (error * 1.0 / 16.0);
					}
				}

				else if(value == 1){
					Output[i][j] = 255;
					error = (double)Input[i][j] - 255;
					if( (i+1) < 512 ){
						Err_Table[i+1][j] = Err_Table[i+1][j] + (error * 7.0 / 16.0);
					}
					if( (i-1) >= 0 && (j+1) <512 ){
						Err_Table[i-1][j+1] = Err_Table[i-1][j+1] + (error * 3.0 / 16.0);
					}
					if( (j+1) < 512 ){
						Err_Table[i][j+1] = Err_Table[i][j+1] + (error * 5.0 / 16.0);
					}
					if( (i+1) < 512 && (j+1) <512 ){
						Err_Table[i+1][j+1] = Err_Table[i+1][j+1] + (error * 1.0 / 16.0);
					}
				}
			}
		}
		else{
			for (int j = 511; j >= 0; --j)
			{
				double value = 0;
				value = round ( ((double)Input[i][j] + Err_Table[i][j]) / 256.0);

				double error = 0;
				if(value == 0){
					Output[i][j] = 0;
					error = (double)Input[i][j];
					if( (i-1) >= 0 ){
						Err_Table[i-1][j] = Err_Table[i-1][j] + (error * 7.0 / 16.0);
					}
					if( (i-1) >= 0 && (j+1) <512 ){
						Err_Table[i-1][j+1] = Err_Table[i-1][j+1] + (error * 1.0 / 16.0);
					}
					if( (j+1) < 512 ){
						Err_Table[i][j+1] = Err_Table[i][j+1] + (error * 5.0 / 16.0);
					}
					if( (i+1) < 512 && (j+1) <512 ){
						Err_Table[i+1][j+1] = Err_Table[i+1][j+1] + (error * 3.0 / 16.0);
					}
				}

				else if(value == 1){
					Output[i][j] = 255;
					error = (double)Input[i][j] - 255;
					if( (i-1) >= 0 ){
						Err_Table[i-1][j] = Err_Table[i-1][j] + (error * 7.0 / 16.0);
					}
					if( (i-1) >= 0 && (j+1) <512 ){
						Err_Table[i-1][j+1] = Err_Table[i-1][j+1] + (error * 1.0 / 16.0);
					}
					if( (j+1) < 512 ){
						Err_Table[i][j+1] = Err_Table[i][j+1] + (error * 5.0 / 16.0);
					}
					if( (i+1) < 512 && (j+1) <512 ){
						Err_Table[i+1][j+1] = Err_Table[i+1][j+1] + (error * 3.0 / 16.0);
					}
				}
			}
		}
	}
}
void JJN(unsigned char ** Input, unsigned char ** Output){
	//constructing a  N * N array of double
	double ** Err_Table = new double* [512];
	for(int i = 0; i < 512; i++){
		Err_Table[i] = new double[512];
	}

	for(int i =0; i < 512; i++){
		for(int j =0 ; j< 512;j++){
			Err_Table[i][j] = 0;
		}
	}

	for(int i = 0; i < 512; i++){
		if((i%2) == 0){
			for (int j = 0; j < 512; ++j)
			{
				double value = 0;
				value = round ( ((double)Input[i][j] + Err_Table[i][j]) / 256.0);

				double error = 0;
				if(value == 0){
					Output[i][j] = 0;
					error = (double)Input[i][j];
					//row3
					if( (i+1) < 512 ){
						Err_Table[i+1][j] = Err_Table[i+1][j] + (error * 7.0 / 48.0);
					}
					if( (i+2) < 512 ){
						Err_Table[i+2][j] = Err_Table[i+2][j] + (error * 5.0 / 48.0);	
					}
					//row4
					if( (i-2) >= 0 && (j+1) < 512 ){
						Err_Table[i-2][j+1] = Err_Table[i-2][j+1] + (error * 3.0 / 48.0);	
					}
					if( (i-1) >= 0 && (j+1) < 512 ){
						Err_Table[i-1][j+1] = Err_Table[i-1][j+1] + (error * 5.0 / 48.0);
					}					
					if( (j+1) < 512 ){
						Err_Table[i][j+1] = Err_Table[i][j+1] + (error * 7.0 / 48.0);
					}
					if( (i+1) < 512 && (j+1) < 512 ){
						Err_Table[i+1][j+1] = Err_Table[i+1][j+1] + (error * 5.0 / 48.0);
					}
					if( (i+2) < 512 && (j+1) < 512 ){
						Err_Table[i+2][j+1] = Err_Table[i+2][j+1] + (error * 3.0 / 48.0);
					}
					//row5
					if( (i-2) >= 0 && (j+2) < 512 ){
						Err_Table[i-2][j+2] = Err_Table[i-2][j+2] + (error * 1.0 / 48.0);
					}
					if( (i-1) >= 0 && (j+2) < 512 ){
						Err_Table[i-1][j+2] = Err_Table[i-1][j+2] + (error * 3.0 / 48.0);
					}
					if( (j+2) < 512 ){
						Err_Table[i][j+2] = Err_Table[i][j+2] + (error * 5.0 / 48.0);
					}
					if( (i+1) < 512 && (j+2) < 512 ){
						Err_Table[i+1][j+2] = Err_Table[i+1][j+2] + (error * 3.0 / 48.0);
					}
					if( (i+2) < 512 && (j+2) < 512 ){
						Err_Table[i+2][j+2] = Err_Table[i+2][j+2] + (error * 1.0 / 48.0);
					}
				}

				else if(value == 1){
					Output[i][j] = 255;
					error = (double)Input[i][j] - 255;
					//row3
					if( (i+1) < 512 ){
						Err_Table[i+1][j] = Err_Table[i+1][j] + (error * 7.0 / 48.0);
					}
					if( (i+2) < 512 ){
						Err_Table[i+2][j] = Err_Table[i+2][j] + (error * 5.0 / 48.0);	
					}
					//row4
					if( (i-2) >= 0 && (j+1) < 512 ){
						Err_Table[i-2][j+1] = Err_Table[i-2][j+1] + (error * 3.0 / 48.0);	
					}
					if( (i-1) >= 0 && (j+1) < 512 ){
						Err_Table[i-1][j+1] = Err_Table[i-1][j+1] + (error * 5.0 / 48.0);
					}					
					if( (j+1) < 512 ){
						Err_Table[i][j+1] = Err_Table[i][j+1] + (error * 7.0 / 48.0);
					}
					if( (i+1) < 512 && (j+1) < 512 ){
						Err_Table[i+1][j+1] = Err_Table[i+1][j+1] + (error * 5.0 / 48.0);
					}
					if( (i+2) < 512 && (j+1) < 512 ){
						Err_Table[i+2][j+1] = Err_Table[i+2][j+1] + (error * 3.0 / 48.0);
					}
					//row5
					if( (i-2) >= 0 && (j+2) < 512 ){
						Err_Table[i-2][j+2] = Err_Table[i-2][j+2] + (error * 1.0 / 48.0);
					}
					if( (i-1) >= 0 && (j+2) < 512 ){
						Err_Table[i-1][j+2] = Err_Table[i-1][j+2] + (error * 3.0 / 48.0);
					}
					if( (j+2) < 512 ){
						Err_Table[i][j+2] = Err_Table[i][j+2] + (error * 5.0 / 48.0);
					}
					if( (i+1) < 512 && (j+2) < 512 ){
						Err_Table[i+1][j+2] = Err_Table[i+1][j+2] + (error * 3.0 / 48.0);
					}
					if( (i+2) < 512 && (j+2) < 512 ){
						Err_Table[i+2][j+2] = Err_Table[i+2][j+2] + (error * 1.0 / 48.0);
					}
				}
			}
		}
		else{
			for (int j = 511; j >= 0; --j)
			{
				double value = 0;
				value = round ( ((double)Input[i][j] + Err_Table[i][j]) / 256.0);

				double error = 0;
				if(value == 0){
					Output[i][j] = 0;
					error = (double)Input[i][j];
					//row3
					if( (i-1) < 512 ){
						Err_Table[i-1][j] = Err_Table[i-1][j] + (error * 7.0 / 48.0);
					}
					if( (i-2) < 512 ){
						Err_Table[i-2][j] = Err_Table[i-2][j] + (error * 5.0 / 48.0);	
					}
					//row4
					if( (i-2) >= 0 && (j+1) < 512 ){
						Err_Table[i-2][j+1] = Err_Table[i-2][j+1] + (error * 3.0 / 48.0);	
					}
					if( (i-1) >= 0 && (j+1) < 512 ){
						Err_Table[i-1][j+1] = Err_Table[i-1][j+1] + (error * 5.0 / 48.0);
					}					
					if( (j+1) < 512 ){
						Err_Table[i][j+1] = Err_Table[i][j+1] + (error * 7.0 / 48.0);
					}
					if( (i+1) < 512 && (j+1) < 512 ){
						Err_Table[i+1][j+1] = Err_Table[i+1][j+1] + (error * 5.0 / 48.0);
					}
					if( (i+2) < 512 && (j+1) < 512 ){
						Err_Table[i+2][j+1] = Err_Table[i+2][j+1] + (error * 3.0 / 48.0);
					}
					//row5
					if( (i-2) >= 0 && (j+2) < 512 ){
						Err_Table[i-2][j+2] = Err_Table[i-2][j+2] + (error * 1.0 / 48.0);
					}
					if( (i-1) >= 0 && (j+2) < 512 ){
						Err_Table[i-1][j+2] = Err_Table[i-1][j+2] + (error * 3.0 / 48.0);
					}
					if( (j+2) < 512 ){
						Err_Table[i][j+2] = Err_Table[i][j+2] + (error * 5.0 / 48.0);
					}
					if( (i+1) < 512 && (j+2) < 512 ){
						Err_Table[i+1][j+2] = Err_Table[i+1][j+2] + (error * 3.0 / 48.0);
					}
					if( (i+2) < 512 && (j+2) < 512 ){
						Err_Table[i+2][j+2] = Err_Table[i+2][j+2] + (error * 1.0 / 48.0);
					}
				}

				else if(value == 1){
					Output[i][j] = 255;
					error = (double)Input[i][j] - 255;
					//row3
					if( (i-1) < 512 ){
						Err_Table[i-1][j] = Err_Table[i-1][j] + (error * 7.0 / 48.0);
					}
					if( (i-2) < 512 ){
						Err_Table[i-2][j] = Err_Table[i-2][j] + (error * 5.0 / 48.0);	
					}
					//row4
					if( (i-2) >= 0 && (j+1) < 512 ){
						Err_Table[i-2][j+1] = Err_Table[i-2][j+1] + (error * 3.0 / 48.0);	
					}
					if( (i-1) >= 0 && (j+1) < 512 ){
						Err_Table[i-1][j+1] = Err_Table[i-1][j+1] + (error * 5.0 / 48.0);
					}					
					if( (j+1) < 512 ){
						Err_Table[i][j+1] = Err_Table[i][j+1] + (error * 7.0 / 48.0);
					}
					if( (i+1) < 512 && (j+1) < 512 ){
						Err_Table[i+1][j+1] = Err_Table[i+1][j+1] + (error * 5.0 / 48.0);
					}
					if( (i+2) < 512 && (j+1) < 512 ){
						Err_Table[i+2][j+1] = Err_Table[i+2][j+1] + (error * 3.0 / 48.0);
					}
					//row5
					if( (i-2) >= 0 && (j+2) < 512 ){
						Err_Table[i-2][j+2] = Err_Table[i-2][j+2] + (error * 1.0 / 48.0);
					}
					if( (i-1) >= 0 && (j+2) < 512 ){
						Err_Table[i-1][j+2] = Err_Table[i-1][j+2] + (error * 3.0 / 48.0);
					}
					if( (j+2) < 512 ){
						Err_Table[i][j+2] = Err_Table[i][j+2] + (error * 5.0 / 48.0);
					}
					if( (i+1) < 512 && (j+2) < 512 ){
						Err_Table[i+1][j+2] = Err_Table[i+1][j+2] + (error * 3.0 / 48.0);
					}
					if( (i+2) < 512 && (j+2) < 512 ){
						Err_Table[i+2][j+2] = Err_Table[i+2][j+2] + (error * 1.0 / 48.0);
					}
				}
			}
		}
	}
}
void Stucki(unsigned char ** Input, unsigned char ** Output){
	//constructing a  N * N array of double
	double ** Err_Table = new double* [512];
	for(int i = 0; i < 512; i++){
		Err_Table[i] = new double[512];
	}

	for(int i =0; i < 512; i++){
		for(int j =0 ; j< 512;j++){
			Err_Table[i][j] = 0;
		}
	}
	cout<<"lala"<<endl;
	for(int i = 0; i < 512; i++){
		if((i%2) == 0){
			for (int j = 0; j < 512; ++j)
			{
				double value = 0;
				value = round ( ((double)Input[i][j] + Err_Table[i][j]) / 256.0);

				double error = 0;
				if(value == 0){
					Output[i][j] = 0;
					error = (double)Input[i][j];
					//row3
					if( (i+1) < 512 ){
						Err_Table[i+1][j] = Err_Table[i+1][j] + (error * 8.0 / 42.0);
					}
					if( (i+2) < 512 ){
						Err_Table[i+2][j] = Err_Table[i+2][j] + (error * 4.0 / 42.0);	
					}
					//row4
					if( (i-2) >= 0 && (j+1) < 512 ){
						Err_Table[i-2][j+1] = Err_Table[i-2][j+1] + (error * 2.0 / 42.0);	
					}
					if( (i-1) >= 0 && (j+1) < 512 ){
						Err_Table[i-1][j+1] = Err_Table[i-1][j+1] + (error * 4.0 / 42.0);
					}					
					if( (j+1) < 512 ){
						Err_Table[i][j+1] = Err_Table[i][j+1] + (error * 8.0 / 42.0);
					}
					if( (i+1) < 512 && (j+1) < 512 ){
						Err_Table[i+1][j+1] = Err_Table[i+1][j+1] + (error * 4.0 / 42.0);
					}
					if( (i+2) < 512 && (j+1) < 512 ){
						Err_Table[i+2][j+1] = Err_Table[i+2][j+1] + (error * 2.0 / 42.0);
					}
					//row5
					if( (i-2) >= 0 && (j+2) < 512 ){
						Err_Table[i-2][j+2] = Err_Table[i-2][j+2] + (error * 1.0 / 42.0);
					}
					if( (i-1) >= 0 && (j+2) < 512 ){
						Err_Table[i-1][j+2] = Err_Table[i-1][j+2] + (error * 2.0 / 42.0);
					}
					if( (j+2) < 512 ){
						Err_Table[i][j+2] = Err_Table[i][j+2] + (error * 4.0 / 42.0);
					}
					if( (i+1) < 512 && (j+2) < 512 ){
						Err_Table[i+1][j+2] = Err_Table[i+1][j+2] + (error * 2.0 / 42.0);
					}
					if( (i+2) < 512 && (j+2) < 512 ){
						Err_Table[i+2][j+2] = Err_Table[i+2][j+2] + (error * 1.0 / 42.0);
					}
				}

				else if(value == 1){
					Output[i][j] = 255;
					error = (double)Input[i][j] - 255;
					//row3
					if( (i+1) < 512 ){
						Err_Table[i+1][j] = Err_Table[i+1][j] + (error * 8.0 / 42.0);
					}
					if( (i+2) < 512 ){
						Err_Table[i+2][j] = Err_Table[i+2][j] + (error * 4.0 / 42.0);	
					}
					//row4
					if( (i-2) >= 0 && (j+1) < 512 ){
						Err_Table[i-2][j+1] = Err_Table[i-2][j+1] + (error * 2.0 / 42.0);	
					}
					if( (i-1) >= 0 && (j+1) < 512 ){
						Err_Table[i-1][j+1] = Err_Table[i-1][j+1] + (error * 4.0 / 42.0);
					}					
					if( (j+1) < 512 ){
						Err_Table[i][j+1] = Err_Table[i][j+1] + (error * 8.0 / 42.0);
					}
					if( (i+1) < 512 && (j+1) < 512 ){
						Err_Table[i+1][j+1] = Err_Table[i+1][j+1] + (error * 4.0 / 42.0);
					}
					if( (i+2) < 512 && (j+1) < 512 ){
						Err_Table[i+2][j+1] = Err_Table[i+2][j+1] + (error * 2.0 / 42.0);
					}
					//row5
					if( (i-2) >= 0 && (j+2) < 512 ){
						Err_Table[i-2][j+2] = Err_Table[i-2][j+2] + (error * 1.0 / 42.0);
					}
					if( (i-1) >= 0 && (j+2) < 512 ){
						Err_Table[i-1][j+2] = Err_Table[i-1][j+2] + (error * 2.0 / 42.0);
					}
					if( (j+2) < 512 ){
						Err_Table[i][j+2] = Err_Table[i][j+2] + (error * 4.0 / 42.0);
					}
					if( (i+1) < 512 && (j+2) < 512 ){
						Err_Table[i+1][j+2] = Err_Table[i+1][j+2] + (error * 2.0 / 42.0);
					}
					if( (i+2) < 512 && (j+2) < 512 ){
						Err_Table[i+2][j+2] = Err_Table[i+2][j+2] + (error * 1.0 / 42.0);
					}
				}
			}
		}
		else{
			for (int j = 511; j >= 0; --j)
			{

				double value = 0;
				value = round ( ((double)Input[i][j] + Err_Table[i][j]) / 256.0);
				double error = 0;
				if(value == 0){
					Output[i][j] = 0;
					error = (double)Input[i][j];
					//row3
					if( (i-1) < 512 ){
						Err_Table[i-1][j] = Err_Table[i-1][j] + (error * 8.0 / 42.0);
					}
					if( (i-2) < 512 ){
						Err_Table[i-2][j] = Err_Table[i-2][j] + (error * 4.0 / 42.0);	
					}
					//row4
					if( (i-2) >= 0 && (j+1) < 512 ){
						Err_Table[i-2][j+1] = Err_Table[i-2][j+1] + (error * 2.0 / 42.0);	
					}
					if( (i-1) >= 0 && (j+1) < 512 ){
						Err_Table[i-1][j+1] = Err_Table[i-1][j+1] + (error * 4.0 / 42.0);
					}					
					if( (j+1) < 512 ){
						Err_Table[i][j+1] = Err_Table[i][j+1] + (error * 8.0 / 42.0);
					}
					if( (i+1) < 512 && (j+1) < 512 ){
						Err_Table[i+1][j+1] = Err_Table[i+1][j+1] + (error * 4.0 / 42.0);
					}
					if( (i+2) < 512 && (j+1) < 512 ){
						Err_Table[i+2][j+1] = Err_Table[i+2][j+1] + (error * 2.0 / 42.0);
					}
					//row5
					if( (i-2) >= 0 && (j+2) < 512 ){
						Err_Table[i-2][j+2] = Err_Table[i-2][j+2] + (error * 1.0 / 42.0);
					}
					if( (i-1) >= 0 && (j+2) < 512 ){
						Err_Table[i-1][j+2] = Err_Table[i-1][j+2] + (error * 2.0 / 42.0);
					}
					if( (j+2) < 512 ){
						Err_Table[i][j+2] = Err_Table[i][j+2] + (error * 4.0 / 42.0);
					}
					if( (i+1) < 512 && (j+2) < 512 ){
						Err_Table[i+1][j+2] = Err_Table[i+1][j+2] + (error * 2.0 / 42.0);
					}
					if( (i+2) < 512 && (j+2) < 512 ){
						Err_Table[i+2][j+2] = Err_Table[i+2][j+2] + (error * 1.0 / 42.0);
					}
				}

				else if(value == 1){
					Output[i][j] = 255;
					error = (double)Input[i][j] - 255;
					//row3
					if( (i-1) < 512 ){
						Err_Table[i-1][j] = Err_Table[i-1][j] + (error * 8.0 / 42.0);
					}
					if( (i-2) < 512 ){
						Err_Table[i-2][j] = Err_Table[i-2][j] + (error * 4.0 / 42.0);	
					}
					//row4
					if( (i-2) >= 0 && (j+1) < 512 ){
						Err_Table[i-2][j+1] = Err_Table[i-2][j+1] + (error * 2.0 / 42.0);	
					}
					if( (i-1) >= 0 && (j+1) < 512 ){
						Err_Table[i-1][j+1] = Err_Table[i-1][j+1] + (error * 4.0 / 42.0);
					}					
					if( (j+1) < 512 ){
						Err_Table[i][j+1] = Err_Table[i][j+1] + (error * 8.0 / 42.0);
					}
					if( (i+1) < 512 && (j+1) < 512 ){
						Err_Table[i+1][j+1] = Err_Table[i+1][j+1] + (error * 4.0 / 42.0);
					}
					if( (i+2) < 512 && (j+1) < 512 ){
						Err_Table[i+2][j+1] = Err_Table[i+2][j+1] + (error * 2.0 / 42.0);
					}
					//row5
					if( (i-2) >= 0 && (j+2) < 512 ){
						Err_Table[i-2][j+2] = Err_Table[i-2][j+2] + (error * 1.0 / 42.0);
					}
					if( (i-1) >= 0 && (j+2) < 512 ){
						Err_Table[i-1][j+2] = Err_Table[i-1][j+2] + (error * 2.0 / 42.0);
					}
					if( (j+2) < 512 ){
						Err_Table[i][j+2] = Err_Table[i][j+2] + (error * 4.0 / 42.0);
					}
					if( (i+1) < 512 && (j+2) < 512 ){
						Err_Table[i+1][j+2] = Err_Table[i+1][j+2] + (error * 2.0 / 42.0);
					}
					if( (i+2) < 512 && (j+2) < 512 ){
						Err_Table[i+2][j+2] = Err_Table[i+2][j+2] + (error * 1.0 / 42.0);
					}
				}
			}
		}
	}


}






