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
void BayerIndex( double** Iof2n, int dim);
void A4Matrix(double ** Iof2n, int dim);
double Tofxy( int weight, int NumberOfPixels);

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
	//Creating N * N Array of Double
	int N = 4;
	double** Iof2n = new double *[N];

	for (int i = 0; i < N; i++) {
		Iof2n[i] = new double[N];
	}
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			Iof2n[i][j] = 0;
		}
	}
	
	BayerIndex(Iof2n,N);
	//A4Matrix(Iof2n, N);
	
	for (int i = 0; i < N; i++) {
	for (int j = 0; j < N; j++) {
	cout << Iof2n[i][j] << "   ";
	}
	cout << endl;
	}
	
	for (int i = 0; i < 512; i++) {
		for (int j = 0; j < 512; j++) {
			//4 intensity levels of display
			if (Input[i][j] >= 0 && Input[i][j] < 64) {
				Output[i][j] = 0;
			}
			else if (Input[i][j] >= 64 && Input[i][j] < 128) {
				Output[i][j] = 85;
			}
			else if (Input[i][j] >= 128 && Input[i][j] < 192) {
				Output[i][j] = 170;
			}
			else {
				Output[i][j] = 255;
			}
			/*
			if (Input[i][j] > Iof2n[(i%N)][(j%N)]) {
				Output[i][j] = 0;
			}
			else {
				Output[i][j] = 255;
			}
			*/
		}
	}
	

	for (int i = 0; i < N; i++) {
		delete[] Iof2n[i];
	}
	delete[] Iof2n;
}
void BayerIndex( double** Iof2n,int dim) {
	double I2[2][2] = { {0,2},{3,1} };
	int i = 0;
	int j = 0;
	Iof2n[i][j] = Tofxy(I2[i][j],4);
	Iof2n[i][j+1] = Tofxy( I2[i][j+1], 4);
	Iof2n[i+1][j] = Tofxy( I2[i+1][j], 4);
	Iof2n[i+1][j+1] = Tofxy( I2[i+1][j+1], 4);

	double I4[4][4];
	for (int i = 0; i < 4; i+=2) {
		for (int j = 0; j < 4; j += 2) {
			if (i == 0 && j == 0) {
				I4[i][j] = 4 * I2[i/2][j/2];
				I4[i][j + 1] = 4 * I2[i/2][j/2 + 1];
				I4[i + 1][j] = 4 * I2[i/2 + 1][j/2];
				I4[i + 1][j + 1] = 4 * I2[i/2 + 1][j/2 + 1];
			}
			else if (i == 0 && j == 2) {
				I4[i][j] = 4 * I2[0][0] + 2;
				I4[i][j + 1] = 4 * I2[0][1] +2;
				I4[i + 1][j] = 4 * I2[1][0] +2;
				I4[i + 1][j + 1] = 4 * I2[1][1] +2 ;
			}
			else if (i == 2 && j == 0) {
				I4[i][j] = 4 * I2[0][0]+3;
				I4[i][j + 1] = 4 * I2[0][1]+3;
				I4[i + 1][j] = 4 * I2[1][0]+3;
				I4[i + 1][j + 1] = 4 * I2[1][1]+3;
			}
			else if (i == 2 && j == 2) {
				I4[i][j] = 4 * I2[0][0]+1;
				I4[i][j + 1] = 4 * I2[0][1]+1;
				I4[i + 1][j] = 4 * I2[1][0]+1;
				I4[i + 1][j + 1] = 4 * I2[1][1]+1;
			}
		}
	}
	double I8[8][8];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			I8[i][j] = I4[i][j] * 4;
		}
	}

	for (int i = 0; i < 8; i += 4) {
		for (int j = 0; j < 8; j += 4) {

			if (i == 0 && j == 4) {
				for (int k = 0; k < 4; k++) {
					for (int l = 0; l < 4; l++) {
						I8[i + k][j + l] = I8[k][l] + 2;
					}
				}
			}
			else if (i == 4 && j == 0) {
				for (int k = 0; k < 4; k++) {
					for (int l = 0; l < 4; l++) {
						I8[i + k][j + l] = I8[k][l] + 3;
					}
				}
			}
			else if (i == 4 && j == 4) {
				for (int k = 0; k < 4; k++) {
					for (int l = 0; l < 4; l++) {
						I8[i + k][j + l] = I8[k][l] + 1;
					}
				}
			}
		}
	}

	if (dim == 8) {
		for (int i = 0; i < dim; i++) {
			for (int j = 0; j < dim; j++) {
				Iof2n[i][j] = Tofxy(I8[i][j],(dim * dim));
				
			}
		}
	}
	else if (dim == 4) {		
		for (int i = 0; i < dim; i++) {
			for (int j = 0; j < dim; j++) {
				Iof2n[i][j] = Tofxy(I4[i][j], (dim * dim));
			}
		}
	}
}
void A4Matrix(double ** Iof2n,int dim) {
	double temp[4][4] = {
		{ 14,10,11,15 },
		{ 9,3,0,4 },
		{ 8,2,1,5 },
		{ 13,7,6,12 }
	};
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			Iof2n[i][j] = Tofxy(temp[i][j],(dim*dim));
		}
	}

}
double Tofxy( int weight, int NumberOfPixels) {
	double ans = 0;
	ans = ((double)weight + 0.5) / NumberOfPixels *255;
	return ans;
}
