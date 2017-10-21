#ifndef __RP_TREE_H
#define __RP_TREE_H

/*这里可以添加需要的代码*/ 

class RPTree {
public:
	RPTree();
	~RPTree();

	bool buildTree( 
		int n,
		int d,
		float** data);
		
	int search(
		int d,
		float* query);

	bool storeTree( 
		const char* index_path);

	bool restoreTree(
		const char* index_path);
		
	/*可以添加需要的代码*/ 
	
};

#endif
