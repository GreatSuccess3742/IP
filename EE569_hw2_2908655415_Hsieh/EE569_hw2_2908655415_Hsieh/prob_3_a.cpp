// This sample code reads in image data from a RAW image file and 
// writes it into another file

// NOTE:	The code assumes that the image is of size 256 x 256 and is in the
//			RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string.h>

using namespace std;
unsigned char ***Create3D_Array(int a, int b, int c);
void Delete3D_Array(unsigned char ***MyArray, int a, int b, int c);
unsigned char **Create2D_Array(int a, int b);
void Delete2D_Array(unsigned char **MyArray,int a , int b);
void Shrinking();
bool SpurCornerCluster(unsigned char* P);
void PrintTableContent(unsigned char ** S_Table, int start, int end);

int main(int argc, char *argv[])

{
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size = 480;

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
	Shrinking();


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
void Shrinking(){
	unsigned char ** S_Table =  Create2D_Array(58,9);
	//S1
	memcpy(S_Table[0],"001010000",9);
	memcpy(S_Table[1],"100010000",9);
	memcpy(S_Table[2],"000010100",9);
	memcpy(S_Table[3],"000010001",9);
	//S2
	memcpy(S_Table[4],"000011000",9);
	memcpy(S_Table[5],"010010000",9);
	memcpy(S_Table[6],"000110000",9);
	memcpy(S_Table[7],"000010010",9);
	//S3
	memcpy(S_Table[8],"001011000",9);
	memcpy(S_Table[9],"011010000",9);
	memcpy(S_Table[10],"110010000",9);
	memcpy(S_Table[11],"100110000",9);
	memcpy(S_Table[12],"000110100",9);
	memcpy(S_Table[13],"000010110",9);
	memcpy(S_Table[14],"000010011",9);
	memcpy(S_Table[15],"000011001",9);
	//TK4

	//STK4
	memcpy(S_Table[16],"001011001",9);
	memcpy(S_Table[17],"111010000",9);
	memcpy(S_Table[18],"100110100",9);
	memcpy(S_Table[19],"000010111",9);
	//ST5-1
	memcpy(S_Table[20],"110011000",9);
	memcpy(S_Table[21],"010011001",9);
	memcpy(S_Table[22],"011110000",9);
	memcpy(S_Table[23],"001011010",9);
	//PrintTableContent(S_Table,20,23);
	///////// content aboved is checked!!!////////////
	//ST5-2
	memcpy(S_Table[24],"011011000",9);
	memcpy(S_Table[25],"110110000",9);
	memcpy(S_Table[26],"000110110",9);
	memcpy(S_Table[27],"000011011",9);
	//ST6
	memcpy(S_Table[28],"110011001",9);
	memcpy(S_Table[29],"011110100",9);
	//STK6
	memcpy(S_Table[30],"111011000",9);
	memcpy(S_Table[31],"011011001",9);
	memcpy(S_Table[32],"111110000",9);
	memcpy(S_Table[33],"110110100",9);
	memcpy(S_Table[34],"100110110",9);
	memcpy(S_Table[35],"000110111",9);
	memcpy(S_Table[36],"000011111",9);
	memcpy(S_Table[37],"001011011",9);
	//STK7
	memcpy(S_Table[38],"111011001",9);
	memcpy(S_Table[39],"111110100",9);
	memcpy(S_Table[40],"100110111",9);
	memcpy(S_Table[41],"001011111",9);
	//STK8
	memcpy(S_Table[42],"011011011",9);
	memcpy(S_Table[43],"111111000",9);
	memcpy(S_Table[44],"110110110",9);
	memcpy(S_Table[45],"000111111",9);
	//STK9
	memcpy(S_Table[46],"111011011",9);
	memcpy(S_Table[47],"011011111",9);
	memcpy(S_Table[48],"111111100",9);
	memcpy(S_Table[49],"111111001",9);
	memcpy(S_Table[50],"111110110",9);
	memcpy(S_Table[51],"110110111",9);
	memcpy(S_Table[52],"100111111",9);
	memcpy(S_Table[53],"001111111",9);
	//STK10
	memcpy(S_Table[54],"111011111",9);
	memcpy(S_Table[55],"111111101",9);
	memcpy(S_Table[56],"111110111",9);
	memcpy(S_Table[57],"101111111",9);
	//K

	///////end of table. 1//////////////
	unsigned char ** ST_Unconditional_Mark_Pattern_Table = Create2D_Array(37,9);
	//Spur
	memcpy(ST_Unconditional_Mark_Pattern_Table[0],"001010000",9);
	memcpy(ST_Unconditional_Mark_Pattern_Table[1],"100010000",9);
	//Single 4-con
	memcpy(ST_Unconditional_Mark_Pattern_Table[2],"000010010",9);
	memcpy(ST_Unconditional_Mark_Pattern_Table[3],"000011000",9);
	//L-Cluster
	memcpy(ST_Unconditional_Mark_Pattern_Table[4],"001011000",9);
	memcpy(ST_Unconditional_Mark_Pattern_Table[5],"011010000",9);
	memcpy(ST_Unconditional_Mark_Pattern_Table[6],"110010000",9);
	memcpy(ST_Unconditional_Mark_Pattern_Table[7],"100110000",9);
	memcpy(ST_Unconditional_Mark_Pattern_Table[8],"000110100",9);
	memcpy(ST_Unconditional_Mark_Pattern_Table[9],"000010110",9);
	memcpy(ST_Unconditional_Mark_Pattern_Table[10],"000010011",9);
	memcpy(ST_Unconditional_Mark_Pattern_Table[11],"000011001",9);
	//4-Con Offset
	memcpy(ST_Unconditional_Mark_Pattern_Table[12],"011110000",9);
	memcpy(ST_Unconditional_Mark_Pattern_Table[13],"110011000",9);
	memcpy(ST_Unconditional_Mark_Pattern_Table[14],"010011001",9);
	memcpy(ST_Unconditional_Mark_Pattern_Table[15],"001011010",9);

	unsigned char * P;
	P = new unsigned char[9];
	SpurCornerCluster(P);

	//PrintTableContent(ST_Unconditional_Mark_Pattern_Table,12,15);
}
bool SpurCornerCluster(unsigned char* P){
	memcpy(P,"011011100",9);

	//if( (P[0] == '0')&& (P[1] == '1' || P[5] == '1') && P[2] == '1' && P[3] == '0' && P[4] == '1' && P[6] == '1' && P[7] == '0' && P[8] == '0' ){
	//	cout<<"Hit!!"<<endl;
	//}
	if(P[0] == P[3] && P[3] == P[7] && P[7] == P[8] && P[8] == '0' && P[2] == P[4] && P[4] == P[6] && P[6] == '1' && (P[1] == '1' || P[5] == '1') ){
		cout<<"HIT"<<endl;
	}
	PrintTableContent(&P,0,0);
	/*cout<<"Table"<<" = "<<endl;
		cout<< P[0]<<" "<<P[1]<<" "<<P[2]<<endl;
		cout<< P[3]<<" "<<P[4]<<" "<<P[5]<<endl;
		cout<< P[6]<<" "<<P[7]<<" "<<P[8]<<endl;*/
	return 0;
}
void PrintTableContent(unsigned char ** Table, int start, int end){
	for(int i = start; i <= end; i++){
		cout<<"Table"<<i<<" = "<<endl;
		cout<< Table[i][0]<<" "<<Table[i][1]<<" "<<Table[i][2]<<endl;
		cout<< Table[i][3]<<" "<<Table[i][4]<<" "<<Table[i][5]<<endl;
		cout<< Table[i][6]<<" "<<Table[i][7]<<" "<<Table[i][8]<<endl;
	}
}





