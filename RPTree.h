#ifndef __RP_TREE_H
#define __RP_TREE_H

#include<iostream>
#include<vector>
#include <map>
#include <fstream>
#include <climits>
#include<cstdlib>
#include<ctime>
#include<cmath>
using namespace std;

/*这里可以添加需要的代码*/

//定义叶子结点储存数据个数和常数C
#define Minsize 16
#define c 5

class RPTree {
public:
	struct Node{
	    Node *left;
	    Node *right;
	    bool is_leaf;   //是否是叶子节点
	    bool rule;      //true表示随机投影分割，false表示同心圆分割；前提是它非叶子节点
	    bool is_buttom;		//restoretree时只剩下非叶子节点构成的树，叶子节点放在文件中通过索引访问 
	    int left_page;		//左叶子节点页号 
	    int right_page;		//右叶子节点页号
	    int data_size;		//叶子实际存储数据大小 
	    float median;
	    vector<float> dv;   //速记投影分割为随机向量，同心圆分割为点的平均值
	    vector<vector<float> > store_data;
	    Node(){
	        left = NULL;
	        right = NULL;
	        is_leaf = false;
	        is_buttom = false;
	    }
	    //叶子节点
	    Node(bool b, vector<vector<float> > v, int n){
	        left = NULL;
	        right = NULL;
	        is_leaf = true;
	        store_data = v;
	        data_size = n;
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
	    //储存时使用 
	    Node(float m, bool r, bool buttom, int left, int right, float v[], Node* l=NULL, Node* ri=NULL)
			:median(m), rule(r), is_buttom(buttom), left_page(left), right_page(right), left(l), right(ri){
	    	for(int i=0; i<50; ++i)
				dv.push_back(v[i]);
			is_leaf = false;
	    }
	};
	
	struct fileNode{
		bool rule;      //true表示随机投影分割，false表示同心圆分割；前提是它非叶子节点
	    bool is_buttom;		//restoretree时只剩下非叶子节点构成的树，叶子节点放在文件中通过索引访问 
	    int left_page;		//左叶子节点页号 
	    int right_page;		//右叶子节点页号
		int pos;
		int data_size;
		float mid;
		float value[51];
		float data[Minsize][51];
		fileNode(vector<vector<float> > v, int n)
			:mid(0), rule(0), is_buttom(0), left_page(0), right_page(0), pos(0), data_size(n){
			for(int i=0; i<v.size(); ++i)
				for(int j=0; j<51; ++j)
					data[i][j] = v[i][j];
		}
		fileNode(float m, bool r, bool buttom, int left, int right, int p, vector<float> v)
			:mid(m), rule(r), is_buttom(buttom), left_page(left), right_page(right), pos(p){
			for(int i=0; i<50; ++i)
				value[i] = v[i];
		}
		fileNode(){}
	};
		
	vector<vector<float> > matrix;
	Node *root;
	FILE *filePtr; 
	FILE *filePtr2;
	RPTree();
	~RPTree();
	
	bool buildTree( 
		int n,
		int d,
		float** data);

	int search( 
		int d,
		float* query);

	
    //写到外存，二进制文件
	bool storeTree(
		const char* index_path);

	void _storeTree(const Node *ptr, int pos);
		
    //读进内存
	bool restoreTree(
		const char* index_path);

	/*可以添加需要的代码*/

	void build_tree(int n, int d, vector<vector<float> > &block_data, Node* &node); 

	int search_1(int d, Node* node, vector<float> &q);  //done,return 叶子节点的页号 
	
	int search_2(int pageNum, char path[], float *query);
};

#endif
