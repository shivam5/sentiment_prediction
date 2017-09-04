#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <time.h>
#include <utility>
#include <vector>
using namespace std;

// The number of examples in test and train file each
#define SIZE 25000
// How many features to be selected with positive polarity and negative polarity respectively
#define no_features 2500
#define train_filename "../aclImdb/train/labeledBow.feat"
#define test_filename "../aclImdb/test/labeledBow.feat"
#define output_file "../data.feat"
#define feature_file "../aclImdb/imdbEr.txt"
#define feature_output_file "../selected-features-indices.txt"

void sample_test(string filename, string outputfile);
void sample_train(string filename, string outputfile);
void get_features(string inputfile, string outputfile);

int main(){

	srand(time(NULL));

	sample_train(train_filename, output_file );
	sample_test(test_filename, output_file );
	get_features (feature_file, feature_output_file);

	return 0;
}


void get_features(string inputfile, string outputfile){

	ifstream fin;
	fin.open(inputfile.c_str());
	ofstream fout;
	fout.open(outputfile.c_str());

	int avg_polarity;
	int i=0;

	vector < pair<double, int> > v;

	while(fin){
		pair <double, int> p;
		fin>>p.first;
		p.second = i;
		v.push_back(p);
		i++;
	}

	sort(v.begin(), v.end());

	int r;
	float x;
		
	int j=0;
	for (int i=0; i<no_features; i++){
		x = rand()/ double(RAND_MAX);
		if (x>0.8){
			fout<<v[j].second<<endl;
			j++;
		}
		else
			fout<<i<<endl;
	}

	i=v.size()-1;
	for(int j=no_features; j<=(2*no_features); j++){
		x = rand()/ double(RAND_MAX);
		if (x>0.8){
			fout<<v[i].second<<endl;
			i--;
		}
		else
			fout<<j<<endl;
	}

	fin.close();
	fout.close();

}

void sample_train(string filename, string outputfile){

	int positive_samples[SIZE/2], negative_samples[SIZE/2];	
	int r;

	for (int i=0; i<SIZE/2;i++){
		positive_samples[i] = i;
		negative_samples[i] = (SIZE/2) + i;
	}

	for (int i=0; i<1000; i++){
		r = rand()%( (SIZE/2)-i+1)+i;
		int temp = positive_samples[i];
		positive_samples[i] = positive_samples[r];
		positive_samples[r] = temp;
	}

	for (int i=0; i<1000; i++){
		r = rand()%( (SIZE/2)-i+1)+i;
		int temp = negative_samples[i];
		negative_samples[i] = negative_samples[r];
		negative_samples[r] = temp;
	}

	sort(positive_samples, positive_samples+501);
	sort(negative_samples, negative_samples+501);
	sort(positive_samples+500, positive_samples+1001);
	sort(negative_samples+500, negative_samples+1001);

	string line;
	ifstream fin;
	ofstream fout;
	fin.open(filename.c_str());
	fout.open(outputfile.c_str());

	int line_count= 0;
	int index_count = 0;
	while( getline(fin, line) ){
		if (index_count<500){
			if (line_count == positive_samples[index_count]){
				fout<<line<<endl;
				index_count++;
			}
		}
		else if (index_count<1000){
			if (line_count == negative_samples[index_count-500]){
				fout<<line<<endl;
				index_count++;
			}

		}
		line_count++;
	}
	fin.close();

	line_count = 0;
	index_count = 0;
	ifstream fin2;
	fin2.open(filename.c_str());
	while( getline(fin2, line) ){
		if (index_count<500){
			if (line_count == positive_samples[index_count+500]){
				fout<<line<<endl;
				index_count++;
			}
		}
		else if (index_count<1000){
			if (line_count == negative_samples[index_count]){
				fout<<line<<endl;
				index_count++;
			}

		}
		line_count++;
	}
	fin2.close();
	fout.close();
}


void sample_test(string filename, string outputfile){

	int positive_samples[SIZE/2], negative_samples[SIZE/2];	
	int r;

	for (int i=0; i<SIZE/2;i++){
		positive_samples[i] = i;
		negative_samples[i] = (SIZE/2) + i;
	}

	for (int i=0; i<500; i++){
		r = rand()%( (SIZE/2)-i+1)+i;
		int temp = positive_samples[i];
		positive_samples[i] = positive_samples[r];
		positive_samples[r] = temp;
	}

	for (int i=0; i<500; i++){
		r = rand()%( (SIZE/2)-i+1)+i;
		int temp = negative_samples[i];
		negative_samples[i] = negative_samples[r];
		negative_samples[r] = temp;
	}

	sort(positive_samples, positive_samples+501);
	sort(negative_samples, negative_samples+501);

	string line;
	ifstream fin;
	ofstream fout;
	fin.open(filename.c_str());
	fout.open(outputfile.c_str(), ios::app);

	int line_count= 0;
	int index_count = 0;
	while( getline(fin, line) ){
		if (index_count<500){
			if (line_count == positive_samples[index_count]){
				fout<<line<<endl;
				index_count++;
			}
		}
		else if (index_count<1000){
			if (line_count == negative_samples[index_count-500]){
				fout<<line<<endl;
				index_count++;
			}

		}
		line_count++;
	}
	fin.close();
	fout.close();
}
