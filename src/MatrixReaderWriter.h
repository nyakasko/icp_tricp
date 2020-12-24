#ifndef MATRIX_READER
#define MATRIX_READER

//#include <string>
#include <iostream>
#include <fstream>
#include "stdlib.h"

using namespace std;

class MatrixReaderWriter {

public:

	double* data;
	int rowNum;
	int columnNum;

	MatrixReaderWriter(double* data, int rownum, int columnNum);
	MatrixReaderWriter(const char* fileName);

	~MatrixReaderWriter();

	void load(const char* fileName);
	void save(const char* fileName);
};

#endif