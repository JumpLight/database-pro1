#include "KDTree.h"
#include "Utility.h"
/*在这里实现KD树的各个函数*/

KDTree::KDTree(){
	vector<float> v;
	root = NULL;
	count = 0;
	dist = INT_MAX;
}

KDTree::~KDTree(){}

int KDTree::countNode(Node *root){
	int nodes = 0;
        if(root == NULL)
            return 0;
        else
            nodes = 1 + countNode(root->left) + countNode(root->right);
    return nodes;
}
void KDTree::_storeTree(const Node *ptr, int pos){
	if(!ptr)
		return;
	
	fileNode node(ptr->mid, ptr->dimen, ptr->value, pos);

	fwrite(&node, sizeof(node), 1, filePtr);

	_storeTree(ptr->left, pos*2);
	_storeTree(ptr->right, pos*2+1);
}

bool KDTree::storeTree(const char* index_path){
	char storeTree[L];
	sprintf(storeTree, "%s/storeTree.txt", index_path);
	filePtr = fopen(storeTree, "wb");
	_storeTree(root, 1);
	fclose(filePtr);
	return true;
}

bool KDTree::restoreTree(const char* index_path){
	map<int, Node*> nodeMap;
	Node *ptr;
	fileNode fNode;
	char storeTree[L];
	sprintf(storeTree, "%s/storeTree.txt", index_path);
	filePtr = fopen(storeTree, "rb");
	while(fread(&fNode, sizeof(fNode), (size_t)1, filePtr)>0){
		ptr = new Node(fNode.mid, fNode.dimen, fNode.value);
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
bool KDTree::buildTree(int n, int d, float** data){
	vector<vector<float> > m(d+1, vector<float>(n, 0));
	matrix = m;
	for (int i = 0; i < d+1; i++){
		for (int j = 0; j < n; j++){
			matrix[i][j] = data[j][i];
		}
	}
	cout << "Building k-d tree..." << endl;
	clock_t startTime, endTime;
	startTime = clock();
	_buildTree(d, root, matrix);
	endTime = clock();
	cout << "Build Complete" << endl;
	cout<<"Build time: "<<(double)(endTime-startTime)/CLOCKS_PER_SEC<<"s"<<endl;
	return true;
}

int KDTree::search(int d, float* query){
	vector<float> q(d+1, 0);
	for (int i = 0; i < d+1; i++){
		q[i] = query[i];
	}
	return _search(q, root, d);
}

void KDTree::_buildTree(int d, Node* &nodes, vector<vector<float> > list){
	if (list[0].empty())
		return;

	int n = list[0].size();
	float dimen = getDimen(list, n);

	vector<vector<float> > leftlist(d+1, vector<float>());
	vector<vector<float> > rightlist(d+1, vector<float>());
	
	split(nodes, dimen, list, leftlist, rightlist, d);
	
	_buildTree(d, nodes->left, leftlist);
	_buildTree(d, nodes->right, rightlist);
}

int KDTree::_search(vector<float> query, Node* tree, int d){
	if (tree == NULL){
		dist = INT_MAX;
		return 0;
	}
	dist = INT_MAX;
	Node* point = tree;
	Node* nearest;
	//first traverse
	while (point){
		searchPath.push(point);
		int dimen = point->dimen;
		if (query[dimen] <= point->mid)
			point = point->left;
		else
			point = point->right;
	}
	//backtrack steps
	Node* back;
	while (!searchPath.empty()){
		back = searchPath.top();
		searchPath.pop();
		float tempD = getDistance(query, back->value);
		if (tempD < dist){
			nearest = back;
			dist = tempD;
		}
		int dimen = back->dimen;
		//if the circle and hyper-space intersect, enter the other child
		//if(fabs(back->mid - query[dimen]) < sqrt(dist)){
			if (query[dimen] <= back->mid)
				point = back->right;
			else
				point = back->left;
			if(point){
				while (point){
					searchPath.push(point);
					int dimen = point->dimen;
					if (query[dimen] <= point->mid)
						point = point->left;
					else
						point = point->right;
				}
			}
		//}
	}
	return nearest->value[d];
}

int KDTree::getDimen(vector<vector<float> > v, int n){
	int dimen = 0;
	float varience = -1;
	for (int j = 0; j < v.size()-1; j++){
		float sum = 0;
		for (int i = 0; i < n; i++)
			sum += v[j][i];
		float avg = sum / n;
		float vari = 0;
		for (int i = 0; i < n; i++){
			vari += (v[j][i] - avg)*(v[j][i] - avg);
		}
		vari /= n;
		if (vari > varience){
			varience = vari;
			dimen = j;
		}
	}
	return dimen;
}

void KDTree::split(Node* &nodes, int dimen, vector<vector<float> > list, vector<vector<float> > &leftlist, vector<vector<float> > &rightlist, int d){
	int pos = list[dimen].size() / 2;
	//combine values in the dimension and corresponding index
	vector<pair<float, float> > p;
	for (int i = 0; i < list[dimen].size(); i++){
		p.push_back(make_pair(list[dimen][i], list[d][i]));
	}
	nth_element(p.begin(),p.begin()+pos,p.end());
	float mid = p[pos].first;
	float pos1 = p[pos].second;
	p.clear();
	//get median position in list
	int pos2 = 0;
	for (int i = 0; i < list[dimen].size(); i++){
		if (pos1 == list[d][i]){
			pos2 = i;
			break;
		}
	}
	vector<float> value(d+1, 0);
	for (int i = 0; i < d+1; i++){
		value[i] = list[i][pos2];
	}
	//set node value
	nodes = new Node(mid, dimen, value);
	for (int i = 0; i < list[dimen].size(); i++){
		if (list[d][i] != list[d][pos2]){
			if (list[dimen][i] <= mid)
				for (int j = 0; j < d+1; j++)
					leftlist[j].push_back(list[j][i]);
			else
				for (int j = 0; j < d+1; j++)
					rightlist[j].push_back(list[j][i]);
		}
	}
}
