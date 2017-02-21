#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace cv;
using namespace std;


unsigned char ***Create3D_Array(int a, int b, int c);
void Delete3D_Array(unsigned char ***MyArray, int a, int b, int c);
unsigned char **Create2D_Array(int a, int b);
void Delete2D_Array(unsigned char **MyArray, int a, int b);
//using triangles to do the transform
float sign(Point2f a, Point2f b, Point2f pt);
bool InTri(Point2f pt, Point2f a, Point2f b, Point2f c);
int WhichTri(Point2f pt);
Point2f Transform(Point2f pt);
//to eliminate the black lines
void MedianFilter(unsigned char *** Imagedata, unsigned char *** output, int Height, int Width, int BytesPerPixel);
void Build_Filter(unsigned char *** Imagedata, int Temp[8], int i, int j, int k, int n);
//correct answer is like this
void RowByRowInterpolation(unsigned char *** Input, unsigned char *** Output, int Height, int Width, int BytesPerPixel);
void BilinearInterpolation(unsigned char *** Input, unsigned char *** Output, int Height, int Width, int BytesPerPixel,int curRow);

int main(int argc, char** argv)

{
	
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
	//unsigned char Imagedata[Size][Size][BytesPerPixel];
	unsigned char *** Imagedata = Create3D_Array(Size, Size, BytesPerPixel);
	// Read image (filename specified by first argument) into image data matrix
	if (!(file = fopen(argv[1], "rb"))) {
		cout << "Cannot open file: " << argv[1] << endl;
		exit(1);
	}
	
	for (int i = 0; i < Size; ++i)
	{
		for (int j = 0; j < Size; ++j)
		{
			fread(Imagedata[i][j], sizeof(unsigned char), BytesPerPixel, file);
		}
	}
	fclose(file);

	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
	unsigned char*** temp_output = Create3D_Array(Size, Size, BytesPerPixel);
	unsigned char*** output = Create3D_Array(Size, Size, BytesPerPixel);
	
	unsigned char*** SingleRow = Create3D_Array(1, Size, BytesPerPixel);
	RowByRowInterpolation(Imagedata, temp_output, Size, Size, BytesPerPixel);
	
	
	for (int i = 0; i < Size; i++) {
		for (int j = 0; j < Size; j++) {
			for (int k = 0; k < BytesPerPixel; k++) {

				Point2f temp = Transform({ (float)i,(float)j });
				if (WhichTri({ (float)i,(float)j }) != 0) {
					output[j][i][k] = temp_output[(int)temp.y][(int)temp.x][k];
				}

			}
		}
	}
	/*
	for (int i = 0; i < Size; i++) {
		for (int j = 0; j < Size; j++) {
			for (int k = 0; k < BytesPerPixel; k++) {
				
				Point2f temp = Transform({ (float)i,(float)j });
				if (WhichTri({ (float)i,(float)j }) != 0) {
					output[i][j][k] = Imagedata[(int)temp.x][(int)temp.y][k];
				}

			}
		}
	}
	*/


	// Write image data (filename specified by second argument) from image data matrix
	
	
	for (int i = 0; i < Size; i++) {
		for (int j = 0; j < Size; j++) {
			for (int k = 0; k < BytesPerPixel; k++) {
				temp_output[i][j][k] = output[i][j][k];
			}
		}
	}

	MedianFilter(temp_output, output, Size, Size, BytesPerPixel);
	
	


	if (!(file = fopen(argv[2], "wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}

	for (int i = 0; i < Size; ++i)
	{
		for (int j = 0; j < Size; ++j)
		{
			fwrite(output[i][j], sizeof(unsigned char), BytesPerPixel, file);
			
		}
	}
	fclose(file);

	Delete3D_Array(Imagedata, Size, Size, BytesPerPixel);
	Delete3D_Array(output, Size, Size, BytesPerPixel);

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
	if (InTri(pt, { 0,250 }, { 250,250 }, { 125,125 })) {
		return 1;
	}
	else if (InTri(pt, { 125,125 }, { 250,250 }, { 250,0 })) {
		return 2;
	}
	else if (InTri(pt, { 250,0 }, { 250,250 }, { 375,125 })) {
		return 3;
	}
	else if (InTri(pt, { 375,125 }, { 250,250 }, { 500,250 })) {
		return 4;
	}
	else if (InTri(pt, { 500,250 }, { 250,250 }, { 375,375 })) {
		return 5;
	}
	else if (InTri(pt, { 375,375 }, { 250,250 }, { 250,500 })) {
		return 6;
	}
	else if (InTri(pt, { 125,375 }, { 250,250 }, { 250,500 })) {
		return 7;
	}
	else if (InTri(pt, { 0,250 }, { 250,250 }, { 125,375 })) {
		return 8;
	}
	else {
		return 0;
	}
}
Point2f Transform(Point2f pt) {
	
	double T1[3][3] = {
		{ 1.0000    ,0.0000         ,0 },
		{ 1, 2, 0},
		{ -250 , -250, 1 }
	};
	double T2[3][3] = {
		{2.0000    ,1.0000        , 0},
		{0,    1.0000         ,0},
		{-250.0000 ,-250.0000    ,1.0000}
	};
	double T3[3][3] = {
		{2.0000 ,-1.0000         ,0},
		{0    ,1.0000         ,0},
		{-250.0000  ,250.0000    ,1.0000}
	};
	double T4[3][3] = {
		{ 1.0000,         0,         0},
		{-1.0000    ,2.0000         ,0},
		{250.0000 ,- 250.0000    ,1.0000 }
	};
	double T5[3][3] = {
		{ 1.0000         ,0         ,0},
		{1.0000    ,2.0000         ,0 },
		{- 250.0000 ,- 250.0000    ,1.0000 }
	};
	double T6[3][3] = {
		{ 2     ,1     ,0},
		{0     ,1     ,0},
		{- 250, - 250     ,1 }
	};
	double T7[3][3] = {
		{ 2.0000 ,-1.0000         ,0},
		{0    ,1.0000         ,0 },
		{- 250.0000  ,250.0000    ,1.0000 }
	};
	double T8[3][3] = {
		{ 1.0000         ,0         ,0},
		{-1.0000    ,2.0000         ,0},
		{250.0000 ,- 250.0000    ,1.0000 }
	};
	double Tup[3][3] = {
		{0.5000    ,0.0000         ,0},
		{-0.5000    ,1.0000         ,0},
		{125.0000    ,0.0000    ,1.0000}
	};
	double Tdown[3][3] = {
		{0.5000         ,0         ,0},
		{0.5000    ,1.0000         ,0},
		{-125.0000 ,-0.0000    ,1.0000}
	};

	int TypeOfTrans = 0;
	TypeOfTrans = WhichTri(pt);
	Point2f answer = { 0,0 };
	double dummy[3] = { pt.x,pt.y,1 };
	/*
	switch (TypeOfTrans)
	{
	case 1:
		dummy[0] = pt.x * T1[0][0] + pt.y * T1[1][0] + T1[2][0];
		dummy[1] = pt.x * T1[0][1] + pt.y * T1[1][1] + T1[2][1];
		dummy[2] = pt.x * T1[0][2] + pt.y * T1[1][2] + T1[2][2];
		break;
	case 2:
		dummy[0] = pt.x * T2[0][0] + pt.y * T2[1][0] + T2[2][0];
		dummy[1] = pt.x * T2[0][1] + pt.y * T2[1][1] + T2[2][1];
		dummy[2] = pt.x * T2[0][2] + pt.y * T2[1][2] + T2[2][2];
		break;
	case 3:
		dummy[0] = pt.x * T3[0][0] + pt.y * T3[1][0] + T3[2][0];
		dummy[1] = pt.x * T3[0][1] + pt.y * T3[1][1] + T3[2][1];
		dummy[2] = pt.x * T3[0][2] + pt.y * T3[1][2] + T3[2][2];
		break;

	case 4:
		dummy[0] = pt.x * T4[0][0] + pt.y * T4[1][0] + T4[2][0];
		dummy[1] = pt.x * T4[0][1] + pt.y * T4[1][1] + T4[2][1];
		dummy[2] = pt.x * T4[0][2] + pt.y * T4[1][2] + T4[2][2];
		break;
	case 5:
		dummy[0] = pt.x * T5[0][0] + pt.y * T5[1][0] + T5[2][0];
		dummy[1] = pt.x * T5[0][1] + pt.y * T5[1][1] + T5[2][1];
		dummy[2] = pt.x * T5[0][2] + pt.y * T5[1][2] + T5[2][2];
		break;
	case 6:
		dummy[0] = pt.x * T6[0][0] + pt.y * T6[1][0] + T6[2][0];
		dummy[1] = pt.x * T6[0][1] + pt.y * T6[1][1] + T6[2][1];
		dummy[2] = pt.x * T6[0][2] + pt.y * T6[1][2] + T6[2][2];
		break;
	case 7:
		dummy[0] = pt.x * T7[0][0] + pt.y * T7[1][0] + T7[2][0];
		dummy[1] = pt.x * T7[0][1] + pt.y * T7[1][1] + T7[2][1];
		dummy[2] = pt.x * T7[0][2] + pt.y * T7[1][2] + T7[2][2];
		break;
	case 8:
		dummy[0] = pt.x * T8[0][0] + pt.y * T8[1][0] + T8[2][0];
		dummy[1] = pt.x * T8[0][1] + pt.y * T8[1][1] + T8[2][1];
		dummy[2] = pt.x * T8[0][2] + pt.y * T8[1][2] + T8[2][2];
		break;
	default:
		break;
	
	}
	*/

	if (TypeOfTrans == 1 || TypeOfTrans == 2 || TypeOfTrans == 3 || TypeOfTrans == 4) {
		//transup
		dummy[0] = pt.x * Tup[0][0] + pt.y * Tup[1][0] + Tup[2][0];
		dummy[1] = pt.x * Tup[0][1] + pt.y * Tup[1][1] + Tup[2][1];
		dummy[2] = pt.x * Tup[0][2] + pt.y * Tup[1][2] + Tup[2][2];
	}
	else if (TypeOfTrans == 5 || TypeOfTrans == 6 || TypeOfTrans == 7 || TypeOfTrans == 8) {
		//transdown
		dummy[0] = pt.x * Tdown[0][0] + pt.y * Tdown[1][0] + Tdown[2][0];
		dummy[1] = pt.x * Tdown[0][1] + pt.y * Tdown[1][1] + Tdown[2][1];
		dummy[2] = pt.x * Tdown[0][2] + pt.y * Tdown[1][2] + Tdown[2][2];
	}
	answer.x = (dummy[0] / dummy[2]);
	answer.y = (dummy[1] / dummy[2]);
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
	return answer;
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
void RowByRowInterpolation(unsigned char *** Input, unsigned char *** Output, int Height, int Width, int BytesPerPixel) {

	unsigned char*** SingleRow = Create3D_Array(1, Width, BytesPerPixel);
	unsigned char*** TransferRow = Create3D_Array(1, Width, BytesPerPixel);
	for (int curRow = 0; curRow < 250; curRow ++) {

		for (int i = 0; i < 1; i++) {
			for (int j = 0; j < Width; j++) {
				for (int k = 0; k < BytesPerPixel; k++) {
					SingleRow[i][j][k] = Input[curRow][j][k];
				}
			}
		}

		BilinearInterpolation(SingleRow, TransferRow, 1, Width, BytesPerPixel,curRow);
		for (int i = 0; i < 1; i++) {
			for (int j = 0; j < Width; j++) {
				for (int k = 0; k < BytesPerPixel; k++) {
					Output[curRow][250 - curRow + j/2][k] = TransferRow[i][j][k];
				}
			}
		}
		
	}
	//BottomHalf
	
	for (int curRow = 250; curRow < 500; curRow++) {
		for (int i = 0; i < 1; i++) {
			for (int j = 0; j < Width; j++) {
				for (int k = 0; k < BytesPerPixel; k++) {
					SingleRow[i][j][k] = Input[curRow][j][k];
				}
			}
		}
		BilinearInterpolation(SingleRow, TransferRow, 1, Width, BytesPerPixel, curRow);
		for (int i = 0; i < 1; i++) {
			for (int j = 0; j < Width; j++) {
				for (int k = 0; k < BytesPerPixel; k++) {
					Output[curRow][curRow -250 + j / 2][k] = TransferRow[i][j][k];
				}
			}
		}
	}
	
	Delete3D_Array(SingleRow, 1, Width, BytesPerPixel);
	Delete3D_Array(TransferRow, 1, Width, BytesPerPixel);
}

void BilinearInterpolation(unsigned char *** Input, unsigned char *** Output, int Height, int Width, int BytesPerPixel,int curRow) {
	for (int BPP = 0; BPP < BytesPerPixel; BPP++) {
		for (int i = 0; i < Height; i++) {
			for (int j = 0; j < Width; j++) {
				if (curRow >= 0 && curRow <= 250) {
					double W_ratio = (double)curRow * 2 / 500.0;
					double H_ratio = (double)Height / 1.0;

					double xcoord = j / W_ratio;
					double ycoord = i / H_ratio;
					//rewrite interpolation
					int TopLeft[2] = { floor(xcoord),floor(ycoord) };
					int TopRight[2] = { ceil(xcoord),floor(ycoord) };
					int BottomLeft[2] = { floor(xcoord),ceil(ycoord) };
					int BottomRight[2] = { ceil(xcoord),ceil(ycoord) };

					double s = (xcoord - TopLeft[0]);
					double t = (ycoord - TopLeft[1]);
					/*
					if (TopLeft[0]<0) TopLeft[0] = 0;  if (TopLeft[0]> (Width - 1)) TopLeft[0] = (Width - 1);
					if (TopLeft[1]<0) TopLeft[1] = 0;  if (TopLeft[1]> (Height - 1)) TopLeft[1] = (Height - 1);
					if (TopRight[0]<0) TopRight[0] = 0;  if (TopRight[0]> (Width - 1)) TopRight[0] = (Width - 1);
					if (TopRight[1]<0) TopRight[1] = 0;  if (TopRight[1]> (Height - 1)) TopRight[1] = (Height - 1);
					if (BottomLeft[0]<0) BottomLeft[0] = 0;  if (BottomLeft[0]> (Width - 1)) BottomLeft[0] = (Width - 1);
					if (BottomLeft[1]<0) BottomLeft[1] = 0;  if (BottomLeft[1]> (Height - 1)) BottomLeft[1] = (Height - 1);
					if (BottomRight[0]< 0) BottomRight[0] = 0;  if (BottomRight[0]> (Width - 1)) BottomRight[0] = (Width - 1);
					if (BottomRight[1]< 0) BottomRight[1] = 0;  if (BottomRight[1]> (Height - 1)) BottomRight[1] = (Height - 1);
					*/

					if (TopLeft[0]<0 || TopLeft[0]>(Width - 1)) {
						Output[0][j][BPP] = 0;
					}
					else if (TopLeft[1]<0 || TopLeft[1]>(Height - 1)) {
						Output[0][j][BPP] = 0;
					}
					else if (TopRight[0]<0 || TopRight[0]>(Width - 1)) {
						Output[0][j][BPP] = 0;
					}
					else if (TopRight[1]<0 || TopRight[1]>(Height - 1)) {
						Output[0][j][BPP] = 0;
					}
					else if (BottomLeft[0]<0 || BottomLeft[0]>(Width - 1)) {
						Output[0][j][BPP] = 0;
					}
					else if (BottomLeft[1]<0 || BottomLeft[1]>(Height - 1)) {
						Output[0][j][BPP] = 0;
					}
					else if (BottomRight[0]<0 || BottomRight[0]>(Width - 1)) {
						Output[0][j][BPP] = 0;
					}
					else if (BottomRight[1]<0 || BottomRight[1]>(Height - 1)) {
						Output[0][j][BPP] = 0;
					}
					
					else {
						Output[0][/*250 - curRow + */j][BPP] = Input[(TopLeft[1])][(TopLeft[0])][BPP] * s * t
												+ Input[(TopRight[1])][(TopRight[0])][BPP] * (1 - s) * t
												+ Input[(BottomLeft[1])][(BottomLeft[0])][BPP] * s * (1 - t)
												+ Input[(BottomRight[1])][(BottomRight[0])][BPP] * (1 - s) * (1 - t);
					}
				}
				
				///Bottom Half
				else if (curRow > 250 && curRow < 500) {
					//Fix Bottom part
					double W_ratio = (double)(500 - (curRow-250) * 2 )/ 500.0;
					double H_ratio = (double)Height / 1.0;

					double xcoord = j / W_ratio;
					double ycoord = i / H_ratio;
					//rewrite interpolation
					int TopLeft[2] = { floor(xcoord),floor(ycoord) };
					int TopRight[2] = { ceil(xcoord),floor(ycoord) };
					int BottomLeft[2] = { floor(xcoord),ceil(ycoord) };
					int BottomRight[2] = { ceil(xcoord),ceil(ycoord) };

					double s = (xcoord - TopLeft[0]);
					double t = (ycoord - TopLeft[1]);
					/*
					if (TopLeft[0]<0) TopLeft[0] = 0;  if (TopLeft[0]> (Width - 1)) TopLeft[0] = (Width - 1);
					if (TopLeft[1]<0) TopLeft[1] = 0;  if (TopLeft[1]> (Height - 1)) TopLeft[1] = (Height - 1);
					if (TopRight[0]<0) TopRight[0] = 0;  if (TopRight[0]> (Width - 1)) TopRight[0] = (Width - 1);
					if (TopRight[1]<0) TopRight[1] = 0;  if (TopRight[1]> (Height - 1)) TopRight[1] = (Height - 1);
					if (BottomLeft[0]<0) BottomLeft[0] = 0;  if (BottomLeft[0]> (Width - 1)) BottomLeft[0] = (Width - 1);
					if (BottomLeft[1]<0) BottomLeft[1] = 0;  if (BottomLeft[1]> (Height - 1)) BottomLeft[1] = (Height - 1);
					if (BottomRight[0]< 0) BottomRight[0] = 0;  if (BottomRight[0]> (Width - 1)) BottomRight[0] = (Width - 1);
					if (BottomRight[1]< 0) BottomRight[1] = 0;  if (BottomRight[1]> (Height - 1)) BottomRight[1] = (Height - 1);
					*/

					if (TopLeft[0]<0 || TopLeft[0]>(Width - 1)) {
						Output[0][j][BPP] = 0;
					}
					else if (TopLeft[1]<0 || TopLeft[1]>(Height - 1)) {
						Output[0][j][BPP] = 0;
					}
					else if (TopRight[0]<0 || TopRight[0]>(Width - 1)) {
						Output[0][j][BPP] = 0;
					}
					else if (TopRight[1]<0 || TopRight[1]>(Height - 1)) {
						Output[0][j][BPP] = 0;
					}
					else if (BottomLeft[0]<0 || BottomLeft[0]>(Width - 1)) {
						Output[0][j][BPP] = 0;
					}
					else if (BottomLeft[1]<0 || BottomLeft[1]>(Height - 1)) {
						Output[0][j][BPP] = 0;
					}
					else if (BottomRight[0]<0 || BottomRight[0]>(Width - 1)) {
						Output[0][j][BPP] = 0;
					}
					else if (BottomRight[1]<0 || BottomRight[1]>(Height - 1)) {
						Output[0][j][BPP] = 0;
					}

					else {
						//cout << "else" << endl;
						Output[0][/*250 - curRow + */j][BPP] = Input[(TopLeft[1])][(TopLeft[0])][BPP] * s * t
							+ Input[(TopRight[1])][(TopRight[0])][BPP] * (1 - s) * t
							+ Input[(BottomLeft[1])][(BottomLeft[0])][BPP] * s * (1 - t)
							+ Input[(BottomRight[1])][(BottomRight[0])][BPP] * (1 - s) * (1 - t);
					}
				}
				//cout << "BP" << endl;
			}
		}
	}
}

