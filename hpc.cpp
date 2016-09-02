#include <stdio.h>
//#include <cuda.h>
#define N 128
#include <bits/stdc++.h>
using namespace std;

vector<int> suma(vector<int> &h_a, vector<int> &h_b){
	vector<int> v;
	for(int i = 0; i < N; i++){
		v[i] = h_a[i] + h_b[i];
	}

	return v;
}

int main(){
	int *h_a, *h_b, *h_c, *d_a, *d_b, *d_c, *h_result;

	//asignacion de memoria

	h_a=(int*)malloc(N *sizeof(int));
	h_b=(int*)malloc(N *sizeof(int));
	h_c=(int*)malloc(N *sizeof(int));

	for(int i = 1; i <= N; i++){
		h_a[i] =  i*i;
		h_b[i] = i+i+i;
	}

	h_c = suma(h_a,h_b);

	for(int i = 0; i< N; i++){
		cout << h_c[i] << " ";
	}
	cout<< endl;
}