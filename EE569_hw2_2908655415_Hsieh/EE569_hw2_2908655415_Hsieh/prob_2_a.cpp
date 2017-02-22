// This sample code reads in image data from a RAW image file and 
// writes it into another file

// NOTE:	The code assumes that the image is of size 256 x 256 and is in the
//			RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d.hpp>

using namespace cv;
using namespace std;
unsigned char ***Create3D_Array(int a, int b, int c);
void Delete3D_Array(unsigned char ***MyArray, int a, int b, int c);
unsigned char **Create2D_Array(int a, int b);
void Delete2D_Array(unsigned char **MyArray, int a, int b);

void Dithering(unsigned char** Input, unsigned char ** Output);
void BayerIndex(unsigned char ** Input, double** Iof2n, int i, int j);
double Tofxy(unsigned char ** Input, int i, int j, int NumberOfPixels);

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
	Dithering(Imagedata, output);

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
unsigned char **Create2D_Array(int a, int b) {
	unsigned char **MyArray = new unsigned char *[a];
	for (int i = 0; i < a; i++) {
		MyArray[i] = new unsigned char[b];
	}
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < b; j++) {
			MyArray[i][j] = 0;
		}
	}
	return MyArray;
}

void Delete2D_Array(unsigned char **MyArray, int a, int b) {
	for (int i = 0; i < a; i++) {
		delete[] MyArray[i];
	}
	delete[] MyArray;
}

void Dithering(unsigned char** Input, unsigned char ** Output) {
	//Creating 512 * 512 Array of Double
	double** Iof2n = new double *[512];

	for (int i = 0; i < 512; i++) {
		Iof2n[i] = new double[512];
	}
	for (int i = 0; i < 512; i++) {
		for (int j = 0; j < 512; j++) {
			Iof2n[i][j] = 0;
		}
	}
	for (int i = 0; i < 512; i++) {
		for (int j = 0; j < 512; j++) {
			BayerIndex(Input, Iof2n, i, j);
		}
	}
	
	for (int i = 0; i < 512; i++) {
		for (int j = 0; j < 512; j++) {
			double T = Tofxy(Input, i, j, 4);
			if (Iof2n[i][j] > T) {
				Output[i][j] = 0;
			}
			else {
				Output[i][j] = 255;
			}
			if (i == 200 && j == 200) {
				cout << "qwer" << endl;
			}
		}
	}
	

	for (int i = 0; i < 512; i++) {
		delete[] Iof2n[i];
	}
	delete[] Iof2n;
}
void BayerIndex(unsigned char ** Input, double** Iof2n,int i, int j) {

	/*Iof2n[i][j] = (4 * (double)Input[i][j] / 255.0) / 4;
	Iof2n[i][j+1] = (4 * (double)Input[i][j+1]/255.0 + 2) / 4;
	Iof2n[i+1][j] = (4 * (double)Input[i+1][j]/255.0 + 3 )/ 4;
	Iof2n[i+1][j+1] =( 4 * (double)Input[i+1][j+1]/255.0 +1) / 4;*/
	int x1, x2, y1, y2;
	x1 = i;
	y1 = j;
	x2 = i + 1;
	y2 = j + 1;

	if (x2 > 511) {
		x2 = 511;
	}
	if (y2 > 511) {
		y2 = 511;
	}
	Iof2n[i][j] = ( (4 * (double)Input[x1][y1] / 255.0) + (4 * (double)Input[x1][y2] / 255.0 + 2) + (4 * (double)Input[x2][y1] / 255.0 + 3) + (4 * (double)Input[x2][y2] / 255.0 + 1) );
}
double Tofxy(unsigned char ** Input, int i, int j, int NumberOfPixels) {
	double ans = 0;
	ans = ((double)Input[i][j] / 255.0 + 0.5) / NumberOfPixels *255;
	return ans;
}
