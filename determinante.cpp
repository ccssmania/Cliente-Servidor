#include <bits/stdc++.h>
using namespace std;



pair< vector< vector<double> >, vector<vector<double> > > lu_descomposition(vector< vector<int> >* mat){
	vector< vector<int> > x = *mat;
	int n = x.size();
	vector< vector<double> > L;
	vector< vector<double> > U;	
	for(int i = 0 ; i < n;  i++){
		vector< double > auxiliar_L,auxiliar_U;
		for(int j = 0; j < n; j++){
			if(i == j){
				auxiliar_L.push_back(1);
				auxiliar_U.push_back(1);
			}
			else if(i > j){
				auxiliar_L.push_back(0);
				auxiliar_U.push_back(0);
			}
			else if(i < j){
				auxiliar_U.push_back(0);
				auxiliar_L.push_back(0);
			}

		}
		L.push_back(auxiliar_L);
		U.push_back(auxiliar_U);
	}
	for(int k = 1; k <= n; k++){
		U[k-1][k-1] = x[k-1][k-1];
		for(int i = k+1; i <= n; i++){
			L[i-1][k-1] = (x[i-1][k-1])/(U[k-1][k-1]);
			U[k-1][i-1] = x[k-1][i-1];
		}
		for(int i = k+1; i <= n; i++){
			for(int j= k+1; j <= n; j++){
				x[i-1][j-1] = x[i-1][j-1] - L[i-1][k-1]*U[k-1][j-1];
			}
		}
	}
	return make_pair(L,U);
	
}





int main(){
	vector< vector<int> > v;
	for(int i=0; i <=2;i++){
		vector<int> x;
		for(int j=1; j<=3; j++){
			int aux;
			cin >> aux;
			x.push_back(aux);
		}
		v.push_back(x);
	}
	for(int i = 0; i < 3; i++ ){
		for(int j = 0; j < 3; j++){
			cout << v[i][j] << " ";
		}
		cout << endl;
	} 
	vector< vector<double> > L,U;
	pair< vector< vector<double> > , vector< vector<double> > > p;
	p = lu_descomposition(&v);
	L = p.first;
	U = p.second;
	int n = L.size();
	double deter = 1.0;
	for(int i = 0; i < n; i++ ){
		deter*=L[i][i]*U[i][i];
	}
	cout << (int)deter << endl;
	return 0;
}