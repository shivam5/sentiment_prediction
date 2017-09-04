#include <iostream>
#include <map>
#include <sstream>
#include <stdlib.h>
#include <fstream>
#include <set>
#include <vector>
#include <cfloat>
#include <math.h>
#include <algorithm>
using namespace std;

#define default_data_file "../data.feat"
#define no_features 5000
#define no_rows 1000
#define no_cols 5001

string feature_file = "../selected-features-indices.txt";

map <int, int> feature_indices;
int** training_data;
int** validation_data;
int** test_data;
struct TNode{
	int attrib;
	float min_threshold;
	float max_threshold;
	bool label;
	TNode *child1;
	TNode *child2;
	TNode *child3;
};

inline void parse_one_instance(string line, int *features);
inline void parse_data(string file, int **training_table, int **validation_table, int **test_table);
TNode *ID3(vector <int> examples, int target_attribute, set <int> attributes, float positive_per, int depth, float stopping_tolerance);
inline int select_attribute(vector <int> examples, set <int> attributes, float positive_percentage, int &threshold1, int &threshold2, int depth);
float check_accuracy(TNode *root, int **data);
bool predict (TNode *root, int* row);
void add_noise(float percentage);
int no_nodes (TNode *root);
int no_terminal_nodes (TNode *root);
void prune_tree(TNode *root, TNode *curr);
int height_tree (TNode *root);
float check_accuracy_forest(TNode *arr[], int size_forest, int **data);
void frequency_of_attributes(TNode *root, vector < pair <int,int> > &frequency_vector);
 
int main(int argc, char *argv[]){

	if (argc!=3 && argc!=4){
		cout<<"Correct input format is : executable data_filename exptno <feature_file> where feature_file is optional (default is ../selected-features-indices.txt )"<<endl;
		return -1;
	}

	if (argv[2][0] < '2' || argv[2][0] > '5'){
		cout<<"The experiment number should be between 2 and 5"<<endl;
		return -1;
	}

	if (argc==4)
		feature_file = argv[3];

	ifstream fin;
	fin.open(feature_file.c_str());
	int a, count=0;
    set <int> attributes;

	while(fin){
		fin>>a;
		feature_indices[a] = count;
		attributes.insert(a);
		count++;
	}
	fin.close();

	training_data = new int*[no_rows];
	for(int i = 0; i < no_rows; i++)
    	training_data[i] = new int[no_cols];

    for (int i=0; i<no_rows; i++)
    	for (int j=0; j<no_cols; j++)
    		training_data[i][j] = 0;
    
	validation_data = new int*[no_rows];
	for(int i = 0; i < no_rows; i++)
    	validation_data[i] = new int[no_cols];

    for (int i=0; i<no_rows; i++)
    	for (int j=0; j<no_cols; j++)
    		validation_data[i][j] = 0;
    
	test_data = new int*[no_rows];
	for(int i = 0; i < no_rows; i++)
    	test_data[i] = new int[no_cols];

    for (int i=0; i<no_rows; i++)
    	for (int j=0; j<no_cols; j++)
    		test_data[i][j] = 0;
    
    parse_data(argv[1], training_data, validation_data, test_data);

    vector <int> examples;
    for (int i=0; i<1000; i++)
    	examples.push_back(i);

    TNode *root;
    float stopping_tolerance = 0;

    if (argv[2][0] == '2'){
    	cout<<"Experiment number 2 (ID3)"<<endl;
    	TNode* arr[10];
    	int i=0;
    	vector < pair <int,int> > frequency_vector;
    	for (stopping_tolerance=0; stopping_tolerance<0.4; stopping_tolerance+=0.1){
		    arr[i] = ID3(examples, 0, attributes, 0.5, 0, stopping_tolerance);
		    cout<<endl<<"Using stopping_tolerance "<<stopping_tolerance<<" to build the tree"<<endl;
		    cout<<"Height of the tree is "<<height_tree(arr[i])<<endl;
		    cout<<"Number of nodes of the tree is "<<no_nodes(arr[i])<<endl;
		    cout<<"Number of terminal nodes of the tree is "<<no_terminal_nodes(arr[i])<<endl;
		    cout<<"Accuracy on training_data "<<check_accuracy(arr[i], training_data)<<endl;
		    cout<<"Accuracy on validation_data "<<check_accuracy(arr[i], validation_data)<<endl;
		    cout<<"Accuracy on test_data "<<check_accuracy(arr[i], test_data)<<endl;
		    frequency_of_attributes(arr[i], frequency_vector);
		    i++;
    	}
    	cout<<endl<<"Average number of times an attribute is used as the splitting function in a tree. Ignoring average frequency <= 1"<<endl;
    	sort(frequency_vector.begin(), frequency_vector.end());
    	for (int j=frequency_vector.size()-1; j>=0; j--){
    		float freq = (float)frequency_vector[j].first;
    		freq /= 4;
    		if (freq>1)
	    		cout<<frequency_vector[j].second<<" "<<freq<<endl;
    	}

    }

    if (argv[2][0] == '3'){
    	cout<<"Experiment number 3 (Adding noise)"<<endl;
    	float noise_arr[] = {0, 0.1, 1, 5, 10};
    	float noise = 0;
    	for (int i =0; i<5; i++){

    		if (i>0){
			    for (int k=0; k<no_rows; k++)
			    	for (int j=0; j<no_cols; j++)
			    		training_data[k][j] = 0;
			    
			    parse_data(argv[1], training_data, validation_data, test_data);
    		}

	    	noise = noise_arr[i];
	    	cout<<endl<<"Adding "<<noise<<" percent noise to the data"<<endl;
	    	add_noise(noise);
	    	stopping_tolerance = 0;
		    root = ID3(examples, 0, attributes, 0.5, 0, stopping_tolerance);
		    cout<<"Using stopping_tolerance "<<stopping_tolerance<<" to build the tree"<<endl;
		    cout<<"Height of the tree is "<<height_tree(root)<<endl;
		    cout<<"Number of nodes of the tree is "<<no_nodes(root)<<endl;
		    cout<<"Accuracy on training_data "<<check_accuracy(root, training_data)<<endl;
		    cout<<"Accuracy on validation_data "<<check_accuracy(root, validation_data)<<endl;
		    cout<<"Accuracy on test_data "<<check_accuracy(root, test_data)<<endl;
    	}

    }

    if (argv[2][0] == '4'){
    	cout<<"Experiment number 4 (Pruning)"<<endl;
    	stopping_tolerance = 0;
	    root = ID3(examples, 0, attributes, 0.5, 0, stopping_tolerance);
	    cout<<endl<<"Using stopping_tolerance "<<stopping_tolerance<<" to build the tree"<<endl;
    	cout<<"Before pruning : "<<endl;
	    cout<<"Height of the tree is "<<height_tree(root)<<endl;
	    cout<<"Number of nodes of the tree is "<<no_nodes(root)<<endl;
	    cout<<"Accuracy on training_data "<<check_accuracy(root, training_data)<<endl;
	    cout<<"Accuracy on validation_data "<<check_accuracy(root, validation_data)<<endl;
	    cout<<"Accuracy on test_data "<<check_accuracy(root, test_data)<<endl;

    	prune_tree(root, root);

    	cout<<endl<<"After pruning : "<<endl;
	    cout<<"Height of the tree is "<<height_tree(root)<<endl;
	    cout<<"Number of nodes of the tree is "<<no_nodes(root)<<endl;
	    cout<<"Accuracy on training_data "<<check_accuracy(root, training_data)<<endl;
	    cout<<"Accuracy on validation_data "<<check_accuracy(root, validation_data)<<endl;
	    cout<<"Accuracy on test_data "<<check_accuracy(root, test_data)<<endl;

    }

    if (argv[2][0] == '5'){
    	cout<<"Experiment number 5 (Decision forest)"<<endl;
    	stopping_tolerance = 0;
	    cout<<"Using stopping_tolerance "<<stopping_tolerance<<" to build the trees"<<endl;
    	int no_feature_subset = 500;
    	int size_forest = 100;
    	cout<<"Making "<<size_forest<<" trees of "<<no_feature_subset<<" features each."<<endl;
    	TNode* arr[size_forest];
    	float accuracy[size_forest];
    	vector <int> attribs;

    	for (set <int>::iterator it = attributes.begin(); it!=attributes.end(); it++)
    		attribs.push_back(*it);


    	for(int i=0; i<size_forest; i++){
    		cout<<"Building "<<i<<"th tree."<<endl;
			int r;
			for (int j=0; j<no_feature_subset; j++){
				r = rand()%( no_features-j+1)+j;
				int temp = attribs[j];
				attribs[j] = attribs[r];
				attribs[r] = temp;
			}

	    	// Make set of attributes of no_feature_subset size
		    set <int> forest_attributes;
		    forest_attributes.clear();
			for (int j=0; j<no_feature_subset; j++){
				forest_attributes.insert(attribs[j]);
			}

		    arr[i] = ID3(examples, 0, forest_attributes, 0.5, 0, stopping_tolerance);
    	}
		cout<<"Effect of number of trees in the forest on train and test accuracies"<<endl;
    	for (int i=10; i<size_forest+1; i+=10){
    		cout<<i<<" "<<check_accuracy_forest(arr, i, training_data)<<" "<<check_accuracy_forest(arr, i, test_data)<<endl;
    	}

    }


}

void frequency_of_attributes(TNode *root, vector < pair <int,int> > &frequency_vector){
	if (root->child1 == NULL && root->child2 == NULL && root->child3 == NULL)
		return;
	int attribute = root->attrib;
	bool flag = false;
	for (int i=0; i<frequency_vector.size(); i++){
		if (frequency_vector[i].second == attribute){
			flag = true;
			frequency_vector[i].first = frequency_vector[i].first + 1;
		}
	}
	if (!flag){
		pair <int, int> p;
		p.first = 1;
		p.second = attribute;
		frequency_vector.push_back(p);
	}
	frequency_of_attributes(root->child1, frequency_vector);
	frequency_of_attributes(root->child2, frequency_vector);
	frequency_of_attributes(root->child3, frequency_vector);

}


void prune_tree(TNode *root, TNode *curr){
	if (curr == NULL)
		return;
	prune_tree(root, curr->child1);
	prune_tree(root, curr->child2);
	prune_tree(root, curr->child3);
	TNode *temp = new TNode;
	temp->attrib = curr->attrib;
	temp->min_threshold = curr->min_threshold;
	temp->max_threshold = curr->max_threshold;
	temp->label = curr->label;
	temp->child1 = curr->child1;
	temp->child2 = curr->child2;
	temp->child3 = curr->child3;

	float initial = check_accuracy(root, validation_data);
	curr->child1 = NULL;
	curr->child2 = NULL;
	curr->child3 = NULL;
	float pruning = check_accuracy(root, validation_data);

	if (pruning < initial){
		curr->child1 = temp->child1;
		curr->child2 = temp->child2;
		curr->child3 = temp->child3;		
	}

}

int height_tree (TNode *root){
    if (root==NULL)
    	return 0;
    else
    	return 1+max(height_tree(root->child1), max(height_tree(root->child2),height_tree(root->child3)) );
}


int no_nodes (TNode *root){
    if (root==NULL)
    	return 0;
    else
    	return 1+no_nodes(root->child1)+no_nodes(root->child2)+no_nodes(root->child3);
}

int no_terminal_nodes (TNode *root){
    if (root->child1==NULL && root->child2==NULL && root->child3==NULL)
    	return 1;
    else
    	return no_terminal_nodes(root->child1)+no_terminal_nodes(root->child2)+no_terminal_nodes(root->child3);
}

void add_noise(float percentage){
	int x = percentage*no_rows;
	x /= 100;
	int arr[no_rows];

	for (int i=0; i<no_rows;i++)
		arr[i] = i;
	
	int r;
	for (int i=0; i<x; i++){
		r = rand()%( no_rows-i+1)+i;
		int temp = arr[i];
		arr[i] = arr[r];
		arr[r] = temp;
	}
	
	for (int i=0; i<x; i++){
		if (training_data[i][0] == 0)
			training_data[i][0] = 1;
		else 
			training_data[i][0] = 0;
	}

}


bool predict (TNode *root, int* row){
    TNode *x=root;
    while(x->child1!=NULL || x->child2!=NULL || x->child3!=NULL){
        int attribute=x->attrib;
		if ( row[feature_indices[attribute]+1] <= x->min_threshold )
            x=x->child1;
		else if ( row[feature_indices[attribute]+1] > x->max_threshold )
			x = x->child3;
		else 
			x = x->child2;
    }
    return x->label;
}

float check_accuracy_forest(TNode *arr[], int size_forest, int **data){
	float n_true=0;
	float n_pos=0, n_neg=0;
	for (int i=0; i<1000; i++){

		n_pos = 0;
		n_neg = 0;
		bool predicted_label;
		for (int j=0; j<size_forest; j++){
			predicted_label = predict(arr[j], data[i]);
			if (predicted_label)
				n_pos += 1;
			else
				n_neg += 1;
		}
		if (n_pos > n_neg)
			predicted_label = true;
		else if (n_neg > n_pos)
			predicted_label = false;

		if (predicted_label == data[i][0] )
			n_true++;
	}
	n_true /= 10;
	return n_true;
}


float check_accuracy(TNode *root, int **data){
	float n_true=0;
	for (int i=0; i<1000; i++){
		bool predicted_label = predict(root, data[i]);
		if (predicted_label == data[i][0] )
			n_true++;
	}
	n_true /= 10;
	return n_true;
}

TNode *ID3(vector <int> examples, int target_attribute, set <int> attributes, float positive_percentage, int depth, float stopping_tolerance){

	TNode *root = new TNode;
	root->child1 = NULL;
	root->child2 = NULL;
	root->child3 = NULL;
	root->attrib = -1;
	if (positive_percentage >= 0.5)
		root->label = true;
	else 
		root->label = false;

	if (stopping_tolerance == 0){
		if (positive_percentage == 1 || positive_percentage == 0 || attributes.size() == 0)
			return root;
	}
	else{		
		if (positive_percentage > (1.0-stopping_tolerance) || positive_percentage < stopping_tolerance || attributes.size() == 0)
			return root;
	}

	int selected_attribute;
	int threshold1, threshold2;
	selected_attribute = select_attribute(examples, attributes, positive_percentage, threshold1, threshold2, depth);

	root->attrib = selected_attribute;
	root->min_threshold = threshold1;
	root->max_threshold = threshold2;

	float positive_per1=0, positive_per2=0, positive_per3=0;
	vector <int> set1_examples, set2_examples, set3_examples;

	for (int i=0; i<examples.size(); i++){
		int x = training_data[examples[i]][ feature_indices[selected_attribute]+1 ];
		int label = training_data[examples[i]][0];
		if ( x <= threshold1 ){
			if ( label == 1 )
				positive_per1++;
			set1_examples.push_back(examples[i]);
		}
		else if ( x > threshold2 ){
			if ( label == 1 )
				positive_per3++;
			set3_examples.push_back(examples[i]);
		}
		else {
			if ( label == 1 )
				positive_per2++;
			set2_examples.push_back(examples[i]);
		}
	}

	attributes.erase(selected_attribute);

	if (set1_examples.size() != 0){
		positive_per1 /= set1_examples.size();
		root->child1 = ID3(set1_examples, target_attribute, attributes, positive_per1, depth+1, stopping_tolerance);
	}
	else{
		TNode *child = new TNode;
		child->child1 = NULL;
		child->child2 = NULL;
		child->child3 = NULL;
		if (positive_percentage >= 0.5)
			child->label = true;
		else 
			child->label = false;
		child->attrib = -1;
		root->child1 = child;
	}
	if (set2_examples.size() != 0){
		positive_per2 /= set2_examples.size();
		root->child2 = ID3(set2_examples, target_attribute, attributes, positive_per2, depth+1, stopping_tolerance);
	}
	else{
		TNode *child = new TNode;
		child->child1 = NULL;
		child->child2 = NULL;
		child->child3 = NULL;
		if (positive_percentage >= 0.5)
			child->label = true;
		else 
			child->label = false;
		child->attrib = -1;
		root->child2 = child;
	}
	if (set3_examples.size() != 0){
		positive_per3 /= set3_examples.size();
		root->child3 = ID3(set3_examples, target_attribute, attributes, positive_per3, depth+1, stopping_tolerance);
	}
	else{
		TNode *child = new TNode;
		child->child1 = NULL;
		child->child2 = NULL;
		child->child3 = NULL;
		if (positive_percentage >= 0.5)
			child->label = true;
		else 
			child->label = false;
		child->attrib = -1;
		root->child3 = child;
	}

	return root;

}

inline int select_attribute(vector <int> examples, set <int> attributes, float positive_percentage, int &threshold1, int &threshold2, int depth){
	double max_ig = -DBL_MAX;
	double ig;
	int select_attribute = *(attributes.begin());
	int threshold1_greedy, threshold2_greedy;

	double max_ig_greedy = -DBL_MAX;
	double ig_greedy;
	int select_attribute_greedy = *(attributes.begin());

	int thresh1, thresh2;
	bool first = false;

	for (set <int>::iterator it = attributes.begin(); it!=attributes.end(); it++){		

		first = (*it>5000);

		int attribute = *it;
		float pos_proportion=positive_percentage;
		double information_gain;
		float size = examples.size();

		information_gain = (pos_proportion * (log(pos_proportion)/log(2) )) + ( (1-positive_percentage) * (log((1-positive_percentage))/log(2) ));
		information_gain = (-1) * information_gain;

		float pos_threshold, neg_threshold;
		int total_pos = 0;
		int total_neg = 0;
		int pos_thresh=0, neg_thresh=0;

		for (int i=0; i<size; i++){
			int x = training_data[examples[i]][ feature_indices[attribute]+1 ];
			int label = training_data[examples[i]][0];
			if ( label == 1 && x!=0){
				pos_thresh += x;
				total_pos++;
			}
			else if( label == 0 && x!=0){
				neg_thresh += x;
				total_neg++;
			}
		}

		pos_threshold = pos_thresh;
		neg_threshold = neg_thresh;

		if (pos_threshold != 0)
			pos_threshold = pos_threshold/total_pos;
		if (neg_threshold != 0)
			neg_threshold = neg_threshold/total_neg;

		threshold1 = min(pos_threshold, neg_threshold);
		threshold2 = max(pos_threshold, neg_threshold);


		float set1_positive, set2_positive, set3_positive;
		int set1_pos=0, set2_pos=0, set3_pos=0;
		int set1_total = 0, set2_total = 0, set3_total = 0;
		for (int i=0; i<size; i++){
			int x = training_data[examples[i]][ feature_indices[attribute]+1 ];
			int label = training_data[examples[i]][0];
			if ( x <= threshold1 ){
				if ( label == 1 )
					set1_pos++;
				set1_total++;
			}
			else if ( x > threshold2 ){
				if ( label == 1 )
					set3_pos++;
				set3_total++;
			}
			else {
				if ( label == 1 )
					set2_pos++;
				set2_total++;
			}
		}

		set1_positive = set1_pos;
		set2_positive = set2_pos;
		set3_positive = set3_pos;

		if (set1_total != 0){
			set1_positive = set1_positive/set1_total;
		}
		if (set2_total != 0){
			set2_positive = set2_positive/set2_total;
		}
		if (set3_total != 0){
			set3_positive = set3_positive/set3_total;
		}


		double entropy;
		if (set1_positive != 0 && set1_positive!=1){
			double proportion = (float) set1_total / (set1_total+set2_total+set3_total);
			entropy =  (set1_positive * (log(set1_positive)/log(2) )) + ( (1-set1_positive) * ( log((1-set1_positive))/log(2) )) ;
			entropy = (-1)*entropy;
			information_gain -= (proportion*entropy);
		}

		if (set2_positive != 0 && set2_positive!=1){

			double proportion = (float) set2_total / (set1_total+set2_total+set3_total);
			entropy = (set2_positive * (log(set2_positive)/log(2) )) + ((1-set2_positive) * (log((1-set2_positive))/log(2) )) ;
			entropy *= (-1);
			information_gain -= (proportion*entropy);
		}

		if (set3_positive != 0 && set3_positive!=1){
			double proportion = (float) set3_total / (set1_total+set2_total+set3_total);
			entropy = (set3_positive * (log(set3_positive)/log(2))) + ((1-set3_positive) * (log((1-set3_positive))/log(2) )) ;
			entropy *= (-1);
			information_gain -= (proportion*entropy);
		}

		if (first){
			ig_greedy = information_gain*1000;
		}
		else{
			ig = information_gain;
		}			

		if (first){
			if (ig_greedy>max_ig_greedy){
				max_ig_greedy = ig_greedy;
				threshold1_greedy = thresh1;
				threshold2_greedy = thresh2;
				select_attribute_greedy = *it;
			}
			
		}
		else{
			if (ig>max_ig){
				max_ig = ig;
				threshold1 = thresh1;
				threshold2 = thresh2;
				select_attribute = *it;
			}
		}
	}


	if (depth>8 && max_ig_greedy > max_ig){		
		threshold1 = threshold1_greedy;
		threshold2 = threshold2_greedy;
		return select_attribute_greedy;
	}
	return select_attribute;
}


inline void parse_data(string file, int **training_table, int **validation_table, int **test_table){
	ifstream fin;
	fin.open(file.c_str());
	if (!fin.is_open()){
		fin.open(default_data_file );
	}

	string line;
	int count = 0;
	while( count<1000 ){
		getline(fin, line);
		parse_one_instance(line, training_table[count]);
		count++;
	}

	count = 0;
	while( count<1000 ){
		getline(fin, line);
		parse_one_instance(line, validation_table[count]);
		count++;
	}

	count = 0;
	while( count<1000 ){
		getline(fin, line);
		parse_one_instance(line, test_table[count]);
		count++;
	}

}

inline void parse_one_instance(string line, int *features){
	stringstream ss;
	ss << line;
	int rating;
	int counter = 0;
	
	ss>>rating;
	if (rating >=7)
		features[counter] = 1;
	else 
		features[counter] = 0;

	string token;
	int feature, feature_index, frequency;
	while(ss >> token){
		int pos = token.find(":"); 
		feature = atoi(token.substr(0, pos).c_str());
		frequency = atoi(token.substr(pos+1).c_str());

		map <int, int>::iterator it = feature_indices.find(feature);
		if ( it != feature_indices.end())
			features[ feature_indices[feature]+1 ] = frequency;

	}

}
