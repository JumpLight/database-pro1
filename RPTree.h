#ifndef __RP_TREE_H
#define __RP_TREE_H
#include<iostream>
#include<vector>
using namespace std;

/*这里可以添加需要的代码*/

struct Node{
    Node *left;
    Node *right;
    bool is_leaf;   //是否是叶子节点
    bool rule;      //true表示随机投影分割，false表示同心圆分割；前提是它非叶子节点
    bool is_buttom;		//restoretree时只剩下非叶子节点构成的树，叶子节点放在文件中通过索引访问 
    vector<float> dv;   //速记投影分割为随机向量，同心圆分割为点的平均值
    float median;
    vector<vector<float> > store_data;
    int left_page;		//左叶子节点页号 
    int right_page;		//右叶子节点页号
    Node(){
        left = NULL;
        right = NULL;
        is_leaf = false;
        is_buttom = false;
    }
    //叶子节点
    Node(bool b, vector<vector<float> > v){
        left = NULL;
        right = NULL;
        is_leaf = true;
        store_data = v;
    }
    //非叶子节点，随机投影分割
    Node(vector<float> v, float x){
        left = NULL;
        right = NULL;
        is_leaf = false;
        rule = true;
        dv = v;
        median = x;
        is_buttom = false;
    }
    //非叶子节点，同心圆分割
    Node(float x, vector<float> v){
        left = NULL;
        right = NULL;
        is_leaf = false;
        rule = false;
        dv = v;
        median = x;
		is_buttom = false;
    }
};

class RPTree {
public:

    vector<vector<float> > matrix;
    Node *root;

	RPTree();
	~RPTree();

	bool buildTree( //done
		int n,
		int d,
		float** data);

	int search( //done
		int d,
		float* query);

    //写到外存，二进制文件
	bool storeTree(
		const char* index_path);

    //读进内存
	bool restoreTree(
		const char* index_path);

	/*可以添加需要的代码*/

	void build_tree(int n, int d, vector<vector<float> > &block_data, Node* &node); //done

	int search_1(int d, Node* node, vector<float> &q);  //done,return 叶子节点的页号 

};

#endif
