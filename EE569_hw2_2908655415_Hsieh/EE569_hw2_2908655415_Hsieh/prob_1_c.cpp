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

float sign(Point2f a, Point2f b, Point2f pt);
bool InTri(Point2f pt, Point2f a, Point2f b, Point2f c);
int WhichTri(Point2f pt);
Point2f Transform(Point2f pt);

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
	int Field_Width = 972;
	int Field_Height = 648;
	unsigned char ***Imagedata = Create3D_Array(Field_Height, Field_Width, BytesPerPixel);

	// Read image (filename specified by first argument) into image data matrix
	if (!(file = fopen(argv[1], "rb"))) {
		cout << "Cannot open file: " << argv[1] << endl;
		exit(1);
	}
	for (int i = 0; i < Field_Height; i++) {
		for (int j = 0; j < Field_Width; ++j)
		{
			fread(Imagedata[i][j], sizeof(unsigned char), BytesPerPixel, file);
		}
	}

	fclose(file);

	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
	int USC_Width = 350;
	int USC_Height = 197;

	unsigned char*** USC = Create3D_Array(USC_Height, USC_Width, BytesPerPixel);
	if (!(file = fopen("images/P1/Trojans.raw", "rb"))) {
		cout << "Cannot open file: " << argv[1] << endl;
		exit(1);
	}
	for (int i = 0; i < USC_Height; i++) {
		for (int j = 0; j < USC_Width; ++j)
		{
			fread(USC[i][j], sizeof(unsigned char), BytesPerPixel, file);
		}
	}

	fclose(file);
	unsigned char *** output = Create3D_Array(Field_Height, Field_Width, BytesPerPixel);

	for (int i = 0; i < Field_Height; i++) {
		for (int j = 0; j < Field_Width; j++) {
			for (int BPP = 0; BPP < BytesPerPixel; BPP++) {
				
				if (WhichTri({ (float)j,(float)i }) != 0) {
					Point2f temp = Transform({ (float)j,(float)i });
					if (USC[(int)temp.y][(int)temp.x][0] > 190 && USC[(int)temp.y][(int)temp.x][1] > 190 && USC[(int)temp.y][(int)temp.x][2] > 190) {
						output[i][j][BPP] = Imagedata[i][j][BPP];
					}
					else {
						output[i][j][BPP] = USC[(int)temp.y][(int)temp.x][BPP];
					}
				}
				else {
					output[i][j][BPP] = Imagedata[i][j][BPP];
				}
			}
		}
	}

	// Write image data (filename specified by second argument) from image data matrix
	

	if (!(file = fopen(argv[2], "wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	for (int i = 0; i < Field_Height; ++i)
	{
		for (int j = 0; j < Field_Width; j++) {
			fwrite(output[i][j], sizeof(unsigned char), BytesPerPixel, file);
		}
	}

	fclose(file);

	Delete3D_Array(Imagedata, Field_Height, Field_Width, BytesPerPixel);
	Delete3D_Array(USC, USC_Height, USC_Width, BytesPerPixel);
	Delete3D_Array(output, Field_Height, Field_Width, BytesPerPixel);
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

	if (InTri(pt, {318,596 }, { 528,612 }, { 653,430 })) {
		return 1;
	}
	else if (InTri(pt, { 318,596 }, { 540,427 }, { 653,430 })) {
		return 2;
	}
	else {
		return 0;
	}
}
Point2f Transform(Point2f pt) {

	double T1[3][3] = {
		{0.1    ,0.9         ,0},
		{-1.8    ,0.6         ,0},
		{1044.9 ,-648.4    ,1}
	};

	double T2[3][3] = {
		{0.0001    ,0.0017         ,0},
		{-0.0020    ,0.0022         ,0},
		{1.1758 ,-1.8546    ,0.0010}
	};
	
	//
	Mat A(8, 8, CV_32F);
	Mat B(8, 1, CV_32F);
	Mat H(8, 1, CV_32F);

	Point2f A_pt1 = { 318,596 };
	Point2f A_pt2 = { 528,612 };
	Point2f A_pt3 = { 653,430 };
	Point2f A_pt4 = { 540,427 };

	Point2f B_pt1 = { 0,0 };
	Point2f B_pt2 = { 0,197 };
	Point2f B_pt3 = { 350,197 };
	Point2f B_pt4 = { 350,0 };
	//row1
	A.at<float>(0, 0) = A_pt1.x;
	A.at<float>(0, 1) = A_pt1.y;
	A.at<float>(0, 2) = 1;
	A.at<float>(0, 3) = 0;
	A.at<float>(0, 4) = 0;
	A.at<float>(0, 5) = 0;
	A.at<float>(0, 6) = (-1) * A_pt1.x * B_pt1.x;
	A.at<float>(0, 7) = (-1) * A_pt1.y * B_pt1.x;
	//row2
	A.at<float>(1, 0) = A_pt2.x;
	A.at<float>(1, 1) = A_pt2.y;
	A.at<float>(1, 2) = 1;
	A.at<float>(1, 3) = 0;
	A.at<float>(1, 4) = 0;
	A.at<float>(1, 5) = 0;
	A.at<float>(1, 6) = (-1) * A_pt2.x * B_pt2.x;
	A.at<float>(1, 7) = (-1) * A_pt2.y * B_pt2.x;
	//row3
	A.at<float>(2, 0) = A_pt3.x;
	A.at<float>(2, 1) = A_pt3.y;
	A.at<float>(2, 2) = 1;
	A.at<float>(2, 3) = 0;
	A.at<float>(2, 4) = 0;
	A.at<float>(2, 5) = 0;
	A.at<float>(2, 6) = (-1) * A_pt3.x * B_pt3.x;
	A.at<float>(2, 7) = (-1) * A_pt3.y * B_pt3.x;
	//row4
	A.at<float>(3, 0) = A_pt4.x;
	A.at<float>(3, 1) = A_pt4.y;
	A.at<float>(3, 2) = 1;
	A.at<float>(3, 3) = 0;
	A.at<float>(3, 4) = 0;
	A.at<float>(3, 5) = 0;
	A.at<float>(3, 6) = (-1) * A_pt4.x * B_pt4.x;
	A.at<float>(3, 7) = (-1) * A_pt4.y * B_pt4.x;
	//row5
	A.at<float>(4, 0) = 0;
	A.at<float>(4, 1) = 0;
	A.at<float>(4, 2) = 0;
	A.at<float>(4, 3) = A_pt1.x;
	A.at<float>(4, 4) = A_pt1.y;
	A.at<float>(4, 5) = 1;
	A.at<float>(4, 6) = (-1) * A_pt1.x * B_pt1.y;
	A.at<float>(4, 7) = (-1) * A_pt1.y * B_pt1.y;
	//row6
	A.at<float>(5, 0) = 0;
	A.at<float>(5, 1) = 0;
	A.at<float>(5, 2) = 0;
	A.at<float>(5, 3) = A_pt2.x;
	A.at<float>(5, 4) = A_pt2.y;
	A.at<float>(5, 5) = 1;
	A.at<float>(5, 6) = (-1) * A_pt2.x * B_pt2.y;
	A.at<float>(5, 7) = (-1) * A_pt2.y * B_pt2.y;
	//row7
	A.at<float>(6, 0) = 0;
	A.at<float>(6, 1) = 0;
	A.at<float>(6, 2) = 0;
	A.at<float>(6, 3) = A_pt3.x;
	A.at<float>(6, 4) = A_pt3.y;
	A.at<float>(6, 5) = 1;
	A.at<float>(6, 6) = (-1) * A_pt3.x * B_pt3.y;
	A.at<float>(6, 7) = (-1) * A_pt3.y * B_pt3.y;
	//row8
	A.at<float>(7, 0) = 0;
	A.at<float>(7, 1) = 0;
	A.at<float>(7, 2) = 0;
	A.at<float>(7, 3) = A_pt4.x;
	A.at<float>(7, 4) = A_pt4.y;
	A.at<float>(7, 5) = 1;
	A.at<float>(7, 6) = (-1) * A_pt4.x * B_pt4.y;
	A.at<float>(7, 7) = (-1) * A_pt4.y * B_pt4.y;

	//Set B
	B.at<float>(0, 0) = B_pt1.x;
	B.at<float>(1, 0) = B_pt2.x;
	B.at<float>(2, 0) = B_pt3.x;
	B.at<float>(3, 0) = B_pt4.x;
	B.at<float>(4, 0) = B_pt1.y;
	B.at<float>(5, 0) = B_pt2.y;
	B.at<float>(6, 0) = B_pt3.y;
	B.at<float>(7, 0) = B_pt4.y;

	H = A.inv() * B;

	float* temp = &H.at<float>(0);
	double Tform[3][3];
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			//H only has 8 elements, so we need to set the value of Tform(3,3) by ourself
			if (i == 2 && j == 2) {
				Tform[2][2] = 1;
				continue;
			}
			Tform[i][j] = (double)temp[3 * i + j];
		}
	}
	
	Point2f ans = { 0,0 };
	double dummy[3] = { pt.x,pt.y,1 };
	int TypeOfTran = 0;
	TypeOfTran = WhichTri(pt);
	
	if (TypeOfTran != 0) {
		dummy[0] = pt.x * Tform[0][0] + pt.y * Tform[0][1] + Tform[0][2];
		dummy[1] = pt.x * Tform[1][0] + pt.y * Tform[1][1] + Tform[1][2];
		dummy[2] = pt.x * Tform[2][0] + pt.y * Tform[2][1] + Tform[2][2];
	}
	ans.x = (dummy[0] / dummy[2]);
	ans.y = (dummy[1] / dummy[2]);
	
	if (ans.x > 349) {
	ans.x = 349;
	}
	if (ans.x < 0) {
	ans.x = 0;
	}
	if (ans.y > 196) {
	ans.y = 196;
	}
	if (ans.y < 0) {
	ans.y = 0;
	}
	
	return ans;
}