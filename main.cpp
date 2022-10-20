#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <fstream>
#include <chrono>
using namespace std;

mutex mat_mutex;

ostream& operator<<(ostream& os, vector<vector<int>> res){
    for(int i = 0;i<res.size();i++){
        for(int j = 0;j<res[0].size();j++)
            os << res[i][j] << ' ';
        os << '\n';
    }
    return os;
}
istream& operator>>(istream& in, vector<vector<int>> &res){
    int n,m;
    in >> n >> m;
    res.resize(n,vector<int>(m));
    for(int i = 0;i<res.size();i++){
        for(int j = 0;j<res[0].size();j++)
            in >> res[i][j];
    }
    return in;
}

void calcBlock(vector<vector<int>> const &mat1,vector<vector<int>> const &mat2,vector<vector<int>> &res,int i1,int k1,int j1,int k2,int p1, int k3){
    lock_guard<mutex> lock(mat_mutex);
    for(int i = i1;i<k1;i++)
        for (int j = j1; j < k2; j++)
            for (int p = p1; p < k3;p++)
                res[i][j] += mat1[i][p] * mat2[p][j];
}

void MatricesByBlocks(int k,vector<vector<int>> const &mat1,vector<vector<int>> const &mat2,vector<vector<int>> &res){
        const int n = res.size();
        const int m = mat2.size();
        vector<thread> threads;
        int i =0,j,p;
        while(i+k<=n){
            j = 0;
            while(j+k<=n){
                p = 0;
                while(p+k<=m) {
                    threads.push_back(thread(calcBlock,mat1, mat2, ref(res), i, i + k, j, j + k, p, p + k));
                    p+=k;
                }
                if(m%k!=0) {

                    threads.push_back(thread(calcBlock,mat1, mat2, ref(res), i, i + k, j, j + k, p, p + m % k));
                }
                j+=k;
            }
            if(n%k!=0){
                p = 0;
                while(p+k<=m) {
                    threads.push_back(thread(calcBlock,mat1, mat2, ref(res), i, i + k, j, j + n%k, p, p + k));
                    p+=k;
                }
                if(m%k!=0) {
                    threads.push_back(thread(calcBlock,mat1, mat2, ref(res), i, i + k, j, j + n % k, p, p + m % k));
                }
            }
            i+=k;
        }
        if(n%k!=0){
            j = 0;
            while(j+k<=n){
                p = 0;
                while(p+k<=m) {
                    threads.push_back(thread(calcBlock,mat1, mat2, ref(res), i, i + n%k, j, j + k, p, p + k));
                    p+=k;
                }
                if(m%k!=0)
                    threads.push_back(thread(calcBlock,mat1,mat2,ref(res),i,i+n%k,j,j+k,p,p+m%k));
                j+=k;
            }
            if(n%k!=0){
                p = 0;
                while(p+k<m) {
                    threads.push_back(thread(calcBlock,mat1, mat2, ref(res), i, i + n%k, j, j + n%k, p, p + k));
                    p+=k;
                }
                if(m%k!=0)
                    threads.push_back(thread(calcBlock,mat1, mat2,ref(res),i,i+n%k,j,j+n%k,p,p+m%k));
            }
        }
        
        for(int i1 = 0;i1<threads.size();i1++){
            threads[i1].join();
        }
}

int main() {
    cout << fixed << setprecision(9) << left;
    vector<vector<int>> mat1;
    vector<vector<int>> mat2;
    long long t;
    ifstream fin("../dat.txt");
    fin >> mat1 >> mat2;
    int n = mat1.size();
    for(int i = 1;i<=n;i++) {
        cout << "Block size: "<< i;
        vector<vector<int>> res(mat1.size(),vector<int>(mat1.size(),0));
        auto start = chrono::high_resolution_clock::now();
        MatricesByBlocks(i, mat1, mat2, res);
        auto end = chrono::high_resolution_clock::now();
        cout << "   Time: " <<chrono::duration<double>(end-start).count()<< endl;
    }
    return 0;
}
