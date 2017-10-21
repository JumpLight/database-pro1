#ifndef __RP_TREE_H
#define __RP_TREE_H

/*������������Ҫ�Ĵ���*/ 

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
		
	/*���������Ҫ�Ĵ���*/ 
	
};

#endif
