#include "RPTree.h"
#include "Utility.h"

/*在这里实现RP树的各个函数*/

RPTree::RPTree(){
    root = NULL;
}

RPTree::~RPTree(){}

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

char storeTree_tree[L];
char storeTree_data[L];
void RPTree::_storeTree(const Node *ptr, int pos){
	if(ptr->is_leaf){
		fileNode node_2(ptr->store_data, ptr->data_size);
		fwrite(&node_2, sizeof(node_2), 1, filePtr2);
		return;
	}
		
	fileNode node(ptr->median, ptr->rule, ptr->is_buttom, ptr->left_page, ptr->right_page, pos, ptr->dv);
	fwrite(&node, sizeof(node), 1, filePtr);

	_storeTree(ptr->left, pos*2);
	_storeTree(ptr->right, pos*2+1);
}

bool RPTree::storeTree(const char* index_path){
	sprintf(storeTree_tree, "%s/storeTree_RPtree.txt", index_path);
	sprintf(storeTree_data, "%s/storeTree_RPdata.txt", index_path);
	filePtr = fopen(storeTree_tree, "wb");
	filePtr2 = fopen(storeTree_data, "wb");
	_storeTree(root, 1);
	fclose(filePtr);
	fclose(filePtr2);
	return true;
}

bool RPTree::restoreTree(const char* index_path){
	map<int, Node*> nodeMap;
	Node *ptr;
	fileNode fNode;
	sprintf(storeTree_tree, "%s/storeTree_RPtree.txt", index_path);
	sprintf(storeTree_data, "%s/storeTree_RPdata.txt", index_path);
	filePtr = fopen(storeTree_tree, "rb");
	while(fread(&fNode, sizeof(fNode), (size_t)1, filePtr)>0){
		ptr = new Node(fNode.mid, fNode.rule, fNode.is_buttom, fNode.left_page, fNode.right_page, fNode.value);
		nodeMap.insert(map<int, Node*>::value_type(fNode.pos, ptr));
	}

	map<int, Node*>::iterator it, temp_it;

	for(it=nodeMap.begin(); it!=nodeMap.end(); ++it){
		temp_it = nodeMap.find(2*(it->first));
		if(temp_it != nodeMap.end())
			it->second->left = temp_it->second;
		else
			it->second->left = NULL;

		temp_it = nodeMap.find(2*(it->first)+1);
		if(temp_it != nodeMap.end())
			it->second->right = temp_it->second;
		else
			it->second->right = NULL;
	}
	
	temp_it = nodeMap.find(1); 

    if (temp_it != nodeMap.end()) {  
        root = temp_it->second;  
    }  

    fclose(filePtr);
    return true;
}

int uuu = 0;
void RPTree::build_tree(int n, int d, vector<vector<float> > &block_data, Node* &node){
    //叶子节点

    if(n < Minsize){
    	vector<vector<float> >  leaf_data(Minsize);
    	vector<float> fill(51,0);
    	for(int i=0; i<Minsize; i++){
    		if(i < n)
				leaf_data[i] = block_data[i];//实际数据 
    		else 
				leaf_data[i] = fill;//填充点
		}
        node = new Node(true, block_data, n);
        //cout << n << "N!!! \n";
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
    cout << "Building RP tree..." << endl;
	clock_t startTime, endTime;
	startTime = clock();
	
    build_tree(n, d, matrix, root);
    
    endTime = clock();
	cout << "Build Complete" << endl;
	cout<<"Build time: "<<(double)(endTime-startTime)/CLOCKS_PER_SEC<<"s"<<endl;
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
static float D = 0;
static float totalD = 0;
int RPTree::search_2(int pageNum, char path[], float* query){
	fileNode fNode;
	filePtr2 = fopen(path, "rb");
	fseek(filePtr2, pageNum*sizeof(fNode), 0); 
	
	float dist=INT_MAX, ans=0;
	fread(&fNode, sizeof(fNode), (size_t)1, filePtr2);
	for(int i=0; i<fNode.data_size; ++i)
		if(dist > getDist(query, fNode.data[i])){
			dist = getDist(query, fNode.data[i]);
			D = dist;
			ans = fNode.data[i][50];
		}
	totalD += D;
	fclose(filePtr2);
	return ans;		
}

int RPTree::search(int d, float* query){
    vector<float> q(d);
    for(int i = 0; i < d; i ++){
        q[i] = query[i];
    }
    int page = search_1(d,root,q);

    int ans = search_2(page, storeTree_data, query);
    
    return ans;
}
