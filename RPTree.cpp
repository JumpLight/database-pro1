#include "RPTree.h"
#include "Utility.h"
#include<cstdlib>
#include<ctime>
#include<cmath>
using namespace std;

//定义叶子结点储存数据个数和常数C
#define Minsize 20
#define c 5

/*在这里实现RP树的各个函数*/

RPTree::RPTree(){
	root = NULL;
}

RPTree::~RPTree(){}

float Distance(vector<float> query, vector<float> node, int d){
    float dis = 0;
	for (int i = 0; i < d; i++){
		dis += (query[i] - node[i])*(query[i] - node[i]);
	}
	return sqrt(dis);
}

float mul(vector<float>& a, vector <float>& b, int d){
    int s = a.size();
    float sum = 0;
    for(int i = 0; i < d; i ++){
        sum += a[i] * b[i];
    }
    return sum;
}

vector<float> get_ramdon_v(int d){
    srand((unsigned)time(NULL));
	vector<float> r;
	float sum = 0;
	for(int i = 0; i < d; i ++){
		r.push_back((float)((rand() % 101) - 50));
		sum += (r[i] * r[i]);
	}
	float v = sqrt(sum);
	for(int i = 0; i < d; i ++){
		r[i] = r[i] / v;
	}
	return r;
}

bool cmp(pair<float,int> p1, pair<float,int> p2){
    if(p1.first > p2.first)return true;
    else return false;
}


int uuu = 0;
void RPTree::build_tree(int n, int d, vector<vector<float> > &block_data, Node* &node){
    //叶子节点

    if(n < Minsize){
        node = new Node(true,block_data);
        return;
    }

    //非叶子节点
    float max_dist = 0;             //两点间最长距离
    float ava_dist = 0;             //两点间平均距离
    float sum_dist = 0;             //两点间总距离
    vector<float> mean_S(d);        //节点平均值
    for(int i = 0; i < d; i ++){
        float sum = 0;
        for(int j = 0; j < n; j ++){
            sum += block_data[j][i];
        }
        sum = sum / n;
        mean_S[i] = sum;
    }

    for(int i = 0; i < n; i ++){
        sum_dist += pow(Distance(block_data[i], mean_S, d), 2);
    }
    ava_dist = (sum_dist * 2) / n;

    for(int i = 0; i < n; i ++){
    	max_dist = max(max_dist,Distance(block_data[i], mean_S, d));
	}

    //Rule
    //随机投影
    if(pow(2 * max_dist, 2) <= c * ava_dist){       //采取近似最大距离，否则建树太慢

        vector<float> ramdon_v = get_ramdon_v(d);
        //各点在随机向量上的映射大小及对应于数据集中的位置
        vector<pair<float,int> > projection(n);
        for(int i = 0; i < n; i ++){
            projection[i].first = mul(block_data[i], ramdon_v, d);
            projection[i].second = i;
        }
        sort(projection.begin(),projection.end(),cmp);
        float nice_cut;
        if(n % 2 == 1)nice_cut = projection[n / 2].first;
        else nice_cut = (projection[n / 2].first + projection[(n / 2) - 1].first) / 2;
        
        /*
        //根据映射长度排序
        sort(projection.begin(),projection.end(),cmp);
        //找到最小方差和对应的index
        int min_var_sum = INT_MAX;
        int min_index = -1;
        vector<float> u_sum(n);
        vector<float> u1(n-1);
        vector<float> u2(n-1);
        u_sum[0] = projection[0].first;
        for(int i = 1; i < n; i ++){
        	u_sum[i] = u_sum[i - 1] + projection[i].first;
		}
		for(int i = 0; i < n - 1; i ++){
        	u1[i] = u_sum[i] / (i + 1);
			u2[i] = (u_sum[n - 1] - u_sum[i + 1]) /(n - (i + 1));
		}
		
        //原来的方法太慢，改成了动态规划
        for(int i = 0; i < n - 1; i ++){
            float l_variance = 0;   //左方差
            for(int j = 1; j <= i; j ++){
                l_variance += pow(projection[j].first - u1[i], 2);
            }

            float r_variance = 0;   //右方差
            for(int j = i + 1; j <= n - 1; j ++){
                r_variance += pow(projection[j].first - u2[i], 2);
            }

            if((l_variance + r_variance) <= min_var_sum){
                min_var_sum = l_variance + r_variance;
                min_index = i;
            }
        }
        float nice_cut = (projection[min_index].first + projection[min_index + 1].first) / 2;
        */
        
        //大于分割线划分在右子树，小于等于分割线划分在左子树
        vector<vector<float> > l_block_data,r_block_data;
        
        for(int i = 0; i < n; i ++){
            if(projection[i].first <= nice_cut)l_block_data.push_back(block_data[projection[i].second]);
            else r_block_data.push_back(block_data[projection[i].second]);
        }
        
        node = new Node(ramdon_v,nice_cut);
        build_tree(l_block_data.size(), d ,l_block_data, node->left);
        if(node->left->is_leaf){
        	node->left_page = uuu++;
        	node->is_buttom = true;
		}
        build_tree(r_block_data.size(), d ,r_block_data, node->right);
        if(node->right->is_leaf){
        	node->right_page = uuu++;
        	node->is_buttom = true;
		}
    }
    //同心圆划分
    else{

        vector<pair<float,int> > all_dis(n);    //到平均点的距离，对应index
        for(int i = 0; i < n; i ++){
            all_dis[i].first = Distance(block_data[i], mean_S, d);
            all_dis[i].second = i;
        }
        sort(all_dis.begin(), all_dis.end(), cmp);
        //求中位数
        float median;
        if(n % 2 == 1)median = all_dis[n / 2].first;
        else median = (all_dis[n / 2].first + all_dis[(n / 2) - 1].first) / 2;
        //大于分割线划分在右子树(外圆)，小于分割线划分在左子树(内圆)
        vector<vector<float> > l_block_data,r_block_data;
        for(int i = 0; i < n; i ++){
            if(all_dis[i].first <= median)l_block_data.push_back(block_data[all_dis[i].second]);
            else r_block_data.push_back(block_data[all_dis[i].second]);
        }
        node = new Node(median,mean_S);
        build_tree(l_block_data.size(), d ,l_block_data, node->left);
        if(node->left->is_leaf){
        	node->left_page = uuu++;
        	node->is_buttom = true;
		}
        build_tree(r_block_data.size(), d ,r_block_data, node->right);
        if(node->right->is_leaf){
        	node->right_page = uuu++;
        	node->is_buttom = true;
		}
    }
}

bool RPTree::buildTree(int n, int d, float** data){
    vector<vector<float> > v_data(n, vector<float>(d + 1));
    matrix = v_data;
    for(int i = 0; i < n; i ++){
        for(int j = 0; j < d + 1; j ++){
            matrix[i][j] = data[i][j];
        }
    }
    build_tree(n, d, matrix, root);
    return true;
}

int RPTree::search_1(int d, Node* node, vector<float> &q){
    
    //随机投影分割
    if(node->rule){
        vector<float> get_dv = node->dv;
        if(mul(q, get_dv, d) <= node->median){
        	if(node->is_buttom)return node->left_page;
        	return search_1(d, node->left, q);
		}
        else {
        	if(node->is_buttom)return node->right_page;
			return search_1(d, node->right, q);
		}
    }
    //同心圆分割
    else{
        if(Distance(q, node->dv, d) <= node->median){
        	if(node->is_buttom)return node->left_page;
        	return search_1(d, node->left, q);
		}
        else{
        	if(node->is_buttom)return node->right_page;
        	return search_1(d, node->right, q);
		}
    }
    
}

int RPTree::search(int d, float* query){
    vector<float> q(d);
    for(int i = 0; i < d; i ++){
        q[i] = query[i];
    }
    int page = search_1(d,root,q);
    
	//给你页号了,你要去文件里找。 
    return page;
}
