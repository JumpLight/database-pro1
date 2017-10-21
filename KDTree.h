#ifndef __KD_TREE_H
#define __KD_TREE_H

/*������������Ҫ�Ĵ���*/ 

class KDTree {
public:
	KDTree();
	~KDTree();

	bool buildTree(  //���������÷���ѡȡά�ȷ��� 
		int n,
		int d,
		float** data);
		
	int search(    //��ѯ��Ҫ�л��ݹ��̣� 
		int d,
		float* query);

	bool storeTree(    //д����� (�������ļ��� 
		const char* index_path);

	bool restoreTree(   //�����ڴ� 
		const char* index_path);
		
	/*���������Ҫ�Ĵ���*/ 
	
};

#endif
