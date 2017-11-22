#ifndef __UTILITY_H
#define __UTILITY_H

#include <iostream>
#include <vector>
#include <cstdio>
#include <cmath>
#include <algorithm>
using namespace std;
#define L 256

static bool read_data(  // 读取数据专用函数，同学们不要修改 
	int n,
	int d,
	float** &data,
	const char* file_name)
{
	FILE* fin = fopen(file_name, "r");
	if (!fin) {
		printf("%s doesn't exist!\n", file_name);
		return false;
	}

	int id;
	data = new float*[n];
	for (int i = 0; i < n; i++) {
		data[i] = new float[d + 1];
		fscanf(fin, "%d", &id);
		data[i][d] = id;   // d号位置用来存储此数据点在源文件中的编号 
		for (int j = 0; j < d; j++) {
			fscanf(fin, "%f", &data[i][j]);
		}
	}

	printf("Finish reading %s\n", file_name);
	fclose(fin);

	return true;
}

/*
这里可以添加代码
*/
static float getDist(float* query, float* data){
	float tempDist=0;
	for(int i=0; i<50; ++i)
		tempDist += (query[i]-data[i])*(query[i]-data[i]);
	return sqrt(tempDist);
}

static float Distance(vector<float> query, vector<float> node, int d){
    float dis = 0;
    for (int i = 0; i < d; i++){
        dis += (query[i] - node[i])*(query[i] - node[i]);
    }
    return sqrt(dis);
}

static float getDistance(vector<float> query, vector<float> node){
	if (query.size() != node.size()){
		cout << "Dimension not match" << endl;
		exit(1);
	}
	float dis = 0;
	for (int i = 0; i < query.size()-1; i++){
		dis += (query[i] - node[i])*(query[i] - node[i]);
	}
	return sqrt(dis);
}
#endif
