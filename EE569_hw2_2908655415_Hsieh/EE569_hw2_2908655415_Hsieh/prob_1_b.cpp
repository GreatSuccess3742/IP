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
using namespace cv;
using namespace std;
unsigned char ***Create3D_Array(int a, int b, int c);
void Delete3D_Array(unsigned char ***MyArray, int a, int b, int c);
unsigned char **Create2D_Array(int a, int b);
void Delete2D_Array(unsigned char **MyArray, int a, int b);
float sign(Point2f a, Point2f b, Point2f pt);
bool InTri(Point2f pt, Point2f a, Point2f b, Point2f c);
int WhichTri(Point2f pt);
Point2f Transform(Point2f pt);

void MedianFilter(unsigned char *** Imagedata, unsigned char *** output, int Height, int Width, int BytesPerPixel);
void Build_Filter(unsigned char *** Imagedata, int Temp[8], int i, int j, int k, int n);

int main(int argc, char *argv[])

{
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size = 256;

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
	unsigned char ***Imagedata = Create3D_Array(Size, Size, BytesPerPixel);

	// Read image (filename specified by first argument) into image data matrix
	if (!(file = fopen(argv[1], "rb"))) {
		cout << "Cannot open file: " << argv[1] << endl;
		exit(1);
	}
	for (int i = 0; i < Size; i++) {
		for (int j = 0; j < Size; ++j)
		{
			fread(Imagedata[i][j], sizeof(unsigned char), BytesPerPixel, file);
		}
	}

	fclose(file);

	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////

	if (!(file = fopen("images/P1/piece.raw", "rb"))) {
		cout << "Can't open piece.raw " << endl;
		exit(1);
	}

	unsigned char *** piece = Create3D_Array(500, 500, BytesPerPixel);
	for (int i = 0; i < 500; i++) {
		for (int j = 0; j < 500; ++j)
		{
			fread(piece[i][j], sizeof(unsigned char), BytesPerPixel, file);
		}
	}



	unsigned char *** output = Create3D_Array(Size, Size, BytesPerPixel);
	for (int i = 0; i < Size; i++) {
		for (size_t j = 0; j < Size; j++)
		{
			for (size_t k = 0; k < BytesPerPixel; k++)
			{
				output[i][j][k] = Imagedata[i][j][k];
			}
		}
	}
	//Puzzle Matching
	for (int i = (161 ); i < (263 ); i++) {
		for (int j = (235 ); j <(336 ); j++) {
			for (int k = 0; k < BytesPerPixel; k++) {
				Point2f temp = Transform({ (float)i,(float)j });
				if (WhichTri({ (float)i,(float)j }) != 0) {
					output[j][i][k] = piece[(int)temp.y][(int)temp.x][k];
				}
				
			}
		}
	}
	//Apply Median Filter
	unsigned char *** temp_output = Create3D_Array(Size, Size, BytesPerPixel);

	for (int i = 0; i < Size; i++) {
		for (int j = 0; j < Size; j++) {
			for (int k = 0; k < BytesPerPixel; k++) {
				temp_output[i][j][k] = output[i][j][k];
			}
		}
	}

	//MedianFilter(temp_output, output, Size, Size, BytesPerPixel);

	// Write image data (filename specified by second argument) from image data matrix
	

	if (!(file = fopen(argv[2], "wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	for (int i = 0; i < Size; ++i)
	{
		for (int j = 0; j < Size; j++) {
			fwrite(output[i][j], sizeof(unsigned char), BytesPerPixel, file);
		}
	}

	fclose(file);

	Delete3D_Array(Imagedata, Size, Size, BytesPerPixel);
	Delete3D_Array(output, Size, Size, BytesPerPixel);
	Delete3D_Array(piece, 500, 500, BytesPerPixel);
	Delete3D_Array(temp_output, Size, Size, BytesPerPixel);
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
float sign(Point2f a, Point2f b, Point2f pt) {
	return (pt.x - b.x) * (a.y - b.y) - (a.x - b.x) * (pt.y - b.y);
}
bool InTri(Point2f pt, Point2f a, Point2f b, Point2f c) {
	bool s1, s2, s3;
	s1 = sign(a, b, pt) <= 0.0;
	s2 = sign(b, c, pt) <= 0.0;
	s3 = sign(c, a, pt) <= 0.0;
	return (s1 == s2) && (s2 == s3);
}
int WhichTri(Point2f pt) {
	//Hillary LetfUp
	if (InTri(pt, { 173,135 }, { 272,135 }, { 173,234 })) {
		return 1;
	}
	//Hillary RightDown
	else if (InTri(pt, { 272,234 }, { 272,135 }, { 173,234 })) {
		return 2;
	}
	else if (InTri(pt, { 161,235 }, { 263,235 }, { 263,336 })) {
		return 3;
	}
	else if (InTri(pt, { 162,236 }, { 162,336 }, { 262,336 })) {
		return 4;
	}
	else {
		return 0;
	}
}
Point2f Transform(Point2f pt) {
	
	//Tranform Matrix of Hillary LeftUp
	double T1[3][3] = {
		{1.4646    ,0.3939         ,0},
		{-0.3939    ,1.4545         ,0},
		{-104.2020 ,-207.5152    ,1.0000}
	};
	//Transform Matrix of Hillary RightDown
	double T2[3][3] = {
		{1.4646    ,0.3939         ,0},
		{-0.3939    ,1.4545         ,0},
		{-104.2020 ,-207.5152    ,1.0000}
	};
	//Transform Matrix of Trump RightUp
	double T3[3][3] = {
		{-0.0594 ,-0.7228         ,0},
		{0.7228 ,-0.0594        , 0},
		{148.7129  ,500.3267    ,1.0000}
	};
	//Transform Matrix of Trump LeftDown
	double T4[3][3] = {
		{-0.0700 ,-0.7400         ,0},
		{0.7500 ,-0.0700         ,0},
		{142.3400  ,507.4000    ,1.0000}
	};
	Point2f ans = { 0,0 };
	double dummy[3] = { pt.x,pt.y,1 };
	int TypeOfTran = 0;
	TypeOfTran = WhichTri(pt);

	if (TypeOfTran == 1) {
		dummy[0] = pt.x * T1[0][0] + pt.y * T1[1][0] + T1[2][0];
		dummy[1] = pt.x * T1[0][1] + pt.y * T1[1][1] + T1[2][1];
		dummy[2] = pt.x * T1[0][2] + pt.y * T1[1][2] + T1[2][2];
	}
	else if (TypeOfTran == 2) {
		dummy[0] = pt.x * T2[0][0] + pt.y * T2[1][0] + T2[2][0];
		dummy[1] = pt.x * T2[0][1] + pt.y * T2[1][1] + T2[2][1];
		dummy[2] = pt.x * T2[0][2] + pt.y * T2[1][2] + T2[2][2];
	}
	else if (TypeOfTran == 3) {
		dummy[0] = pt.x * T3[0][0] + pt.y * T3[1][0] + T3[2][0];
		dummy[1] = pt.x * T3[0][1] + pt.y * T3[1][1] + T3[2][1];
		dummy[2] = pt.x * T3[0][2] + pt.y * T3[1][2] + T3[2][2];
	}
	else if (TypeOfTran == 4) {
		dummy[0] = pt.x * T4[0][0] + pt.y * T4[1][0] + T4[2][0];
		dummy[1] = pt.x * T4[0][1] + pt.y * T4[1][1] + T4[2][1];
		dummy[2] = pt.x * T4[0][2] + pt.y * T4[1][2] + T4[2][2];
	}
	ans.x = (dummy[0] / dummy[2]);
	ans.y = (dummy[1] / dummy[2]);
	/*
	if (answer.x > 499) {
		answer.x = 499;
	}
	if (answer.x < 0) {
		answer.x = 0;
	}
	if (answer.y > 499) {
		answer.y = 499;
	}
	if (answer.y < 0) {
		answer.y = 0;
	}
	*/
	return ans;
}

void MedianFilter(unsigned char *** Imagedata, unsigned char *** output, int Height, int Width, int BytesPerPixel) {

	for (int i = 0; i < Height; ++i)
	{
		for (int j = 0; j < Width; ++j)
		{
			for (int k = 0; k < BytesPerPixel; ++k)
			{
				int Temp[8];
				Build_Filter(Imagedata, Temp, i - 1, j - 1, k, 0);// 0 1 2
				Build_Filter(Imagedata, Temp, i - 1, j, k, 1);// 3   4
				Build_Filter(Imagedata, Temp, i - 1, j + 1, k, 2);// 5 6 7
				Build_Filter(Imagedata, Temp, i, j - 1, k, 3);
				Build_Filter(Imagedata, Temp, i, j + 1, k, 4);
				Build_Filter(Imagedata, Temp, i + 1, j - 1, k, 5);
				Build_Filter(Imagedata, Temp, i + 1, j, k, 6);
				Build_Filter(Imagedata, Temp, i + 1, j + 1, k, 7);
				vector<int> Filter(Temp, Temp + 8);
				/*if(i == 0&& j == 0 && k == 0){
				for(int count = 0; count < 8; count ++){
				cout<<"count = "<<Filter[count]<<endl;
				}
				}*/
				sort(Filter.begin(), Filter.end());
				double Median = 0;

				Median = (Filter[3] + Filter[4]) / 2.0;

				output[i][j][k] = (int)Median;
			}
		}
	}
}

void Build_Filter(unsigned char *** Imagedata, int Temp[8], int i, int j, int k, int n) {
	if (i < 0 || i > 499 || j < 0 || j > 499) {
		Temp[n] = 0;
	}
	else {
		Temp[n] = (int)Imagedata[i][j][k];
	}

}