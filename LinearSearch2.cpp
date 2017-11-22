#include <iostream>
#include <stdio.h>
#include <float.h>
#include <iomanip>
#include <time.h>
#include "Utility.h"
#include <fstream>

using namespace std;

int n = 60000, d = 50, qn = 1000;

float calc(const float * data, const float * array)
{
	float sum = 0;
	for(int i = 0; i < d; i++){
		sum += pow(abs(data[i] - array[i]), 2);
	}
	return sqrt(sum);
}

int Find(float ** data, const float * array)
{
	float min = FLT_MAX - 1;
	int index = -1;
	for(int i = 0; i < n; i++){
		float temp = calc(data[i], array);
		if(temp < min){
			min = temp;
			index = i;
		}
	}
	return index;
}

int main()
{
	//MNIST
	
	char data_path[512] = "Mnist/src/dataset.txt";
	char query_path[512] = "Mnist/src/query.txt";
	float** data = NULL;
	float** query = NULL;
	
	if (!read_data(n, d, data, data_path)) {  
		cout << "error: read data" << endl;
		return 1;
	}
	if (!read_data(qn, d, query, query_path)){
		cout << "error: read query" << endl;
		return 1;
	}
	
	ofstream outfile("linearAns.txt");
	//int row = 1; //Ë÷Òý 
	float total = 0;
	for(int row=1; row<=1000; ++row){
		int index = Find(data, query[row - 1]);
		
//		cout << row << endl;
//		for(int i = 0; i < d; i++){
//			cout << setw(3) << query[row - 1][i] << " ";
//			if(i % 10 == 9)cout << endl;
//		}
		
//		cout << endl << "-------------------" << endl;
		cout << index + 1 << "  " << calc(data[index], query[row - 1])<< endl;
//		for(int i = 0; i < d; i++){
//			cout << setw(3) << data[index][i] << " ";
//			if(i % 10 == 9)cout << endl;
//		}
//		cout << endl;
		outfile << row-1 << " " << index+1<< "  " << calc(data[index], query[row - 1])<< endl;
		total += calc(data[index], query[row - 1]);
	}
	outfile << total/1000;
	outfile.close();
    return 0;
}
