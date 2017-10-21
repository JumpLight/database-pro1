#ifndef __KD_TREE_H
#define __KD_TREE_H

/*这里可以添加需要的代码*/ 

class KDTree {
public:
	KDTree();
	~KDTree();

	bool buildTree(  //建树（利用方差选取维度法） 
		int n,
		int d,
		float** data);
		
	int search(    //查询（要有回溯过程） 
		int d,
		float* query);

	bool storeTree(    //写到外存 (二进制文件） 
		const char* index_path);

	bool restoreTree(   //读进内存 
		const char* index_path);
		
	/*可以添加需要的代码*/ 
	
};

#endif
