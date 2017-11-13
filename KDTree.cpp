#include "KDTree.h"
#include "Utility.h"
#include <iostream>
#include <vector>
using namespace std;

/*在这里实现KD树的各个函数*/

KDTree::KDTree(){
	vector<float> v;
	root = NULL;
	count = 0;
	dist = 0;
}

KDTree::~KDTree(){}
void KDTree::_storeTree(const Node *ptr, int pos){
	if(!ptr)
		return;
	
	fileNode node(ptr->mid, ptr->dimen, ptr->value, pos);

	fwrite(&node, sizeof(node), 1, filePtr);

	_storeTree(ptr->left, pos*2);
	_storeTree(ptr->right, pos*2+1);
}

bool KDTree::storeTree(const char* index_path){
	filePtr = fopen(index_path, "wb");
	_storeTree(root, 1);
	fclose(filePtr);
	return true;
}

bool KDTree::restoreTree(const char* index_path){
	map<int, Node*> nodeMap;
	Node *ptr;
	fileNode fNode;
	filePtr = fopen(index_path, "rb");
	
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
	_buildTree(d, root, matrix);
	cout << "Build Complete" << endl;
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
	float mid = getMid(nodes, dimen, list, leftlist, rightlist, d);
	/*for (int i = 0; i < d; i++){
		cout << nodes->value[i] << " ";
	}
	cout << endl;*/
	//cout << nodes->value[50] << endl;
	//cout << leftlist[0].size() << " " << rightlist[0].size() << endl;
	
	_buildTree(d, nodes->left, leftlist);
	_buildTree(d, nodes->right, rightlist);
}

int KDTree::_search(vector<float> query, Node* tree, int d){
	if (tree == NULL){
		dist = INT_MAX;
		return NULL;
	}
	vector<float> nearest(d+1);
	Node* point = tree;
	nearest = point->value;
	while (point){
		searchPath.push(point);
		if (getDistance(query, point->value) < getDistance(query, nearest)){
			nearest = point->value;
			dist = getDistance(query, point->value);
		}
		int dimen = point->dimen;
		if (query[dimen] <= point->mid)
			point = point->left;
		else
			point = point->right;
	}
	
	Node* back;
	while (!searchPath.empty()){
		back = searchPath.top();
		searchPath.pop();
		int dimen = back->dimen;
		if (fabs(back->mid - query[dimen]) < dist){
			if (query[dimen] <= back->mid)
				point = back->right;
			else
				point = back->left;
			if (point != NULL)
				searchPath.push(point);
		}
		if (point != NULL)
			if (getDistance(query, point->value) < getDistance(query, nearest)){
				nearest = point->value;
				dist = getDistance(query, point->value);
			}
	}
	return nearest[d];
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

float KDTree::getMid(Node* &nodes, int dimen, vector<vector<float> > list, vector<vector<float> > &leftlist, vector<vector<float> > &rightlist, int d){
	int pos = list[dimen].size() / 2;
	//combine values in the dimension and corresponding index
	vector<pair<float, float> > p;
	for (int i = 0; i < list[dimen].size(); i++){
		p.push_back(make_pair(list[dimen][i], list[d][i]));
	}
	sort(p.begin(), p.end());
	float mid = p[pos].first;
	float pos1 = p[pos].second;
	//get median position in list
	int pos2 = 0;
	for (int i = 0; i < list[dimen].size(); i++){
		if (pos1 == list[d][i])
			pos2 = i;
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
			else if (list[dimen][i] > mid)
				for (int j = 0; j < d+1; j++)
					rightlist[j].push_back(list[j][i]);
		}
	}
	return mid;
}
