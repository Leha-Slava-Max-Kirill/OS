#include <iostream>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <vector>

using namespace std;

#define MAX_NUMBER_OF_THREADS 100

mutex mtx;
int lower_counter = 0, upper_counter = 0;

void show(vector<vector<double>> A, int n);
void L_loop(int &k, int &n, vector<vector<double>> &L, 
	vector<vector<double>> &U);
void U_loop(int &k, int &n, vector<vector<double>> &L, 
	vector<vector<double>> &U);	
void LU(vector<vector<double>> A, vector<vector<double>> &L, 
	vector<vector<double>> &U, int n);
double determinant(vector<vector<double>> &L,
	vector<vector<double>> &U);
void mult(vector<vector<double>> A, vector<vector<double>> B, 
	vector<vector<double>> &R, int n);


void L_loop(int &k, int &n, vector<vector<double>> &L, 
		    vector<vector<double>> &U) {
    mtx.lock();
    lower_counter++;
	if (lower_counter >= MAX_NUMBER_OF_THREADS) {
		cout << "Превышено максимальное кол-во потоков на один процесс\n";
		return;
	}
    cout << "In " << lower_counter
         << " loop for lower matrix: thread id = "
         << this_thread::get_id() << endl;
	for(int i = k-1; i < n; i++)
		for(int j = i; j < n; j++)
			L[j][i]=U[j][i]/U[i][i];
    mtx.unlock();
}

void U_loop(int &k, int &n, vector<vector<double>> &L, 
		    vector<vector<double>> &U) {
    mtx.lock();
    upper_counter++;
	if (lower_counter >= MAX_NUMBER_OF_THREADS) {
		cout << "Превышено максимальное кол-во потоков на один процесс\n";
		return;
	}
    cout << "In " << upper_counter
         << " loop for upper matrix: thread id = "
         << this_thread::get_id() << endl;
	for(int i = k; i < n; i++)
		for(int j = k-1; j < n; j++)
			U[i][j]=U[i][j]-L[i][k-1]*U[k-1][j];
    mtx.unlock();

}

void LU(vector<vector<double>> A, vector<vector<double>> &L, 
		vector<vector<double>> &U, int n) {
	U=A;

	for(int k = 1; k < n; k++) {
		// j - строки
		// i - столбцы
		// делим каждое значение в столбцах матрицы L
		// на значение диагонального элемента матрицы U,
		// который находится в том же столбце
        thread L_loop_th(L_loop, ref(k), ref(n), ref(L), ref(U));
		// j - столбцы
		// i - строки
		// вычитаем из строк i >= 1 элемент матрицы L,
		// строящий на той же позиции, домноженный на
		// диагональный элемент матрицы U того же столбца
		thread U_loop_th(U_loop, ref(k), ref(n), ref(L), ref(U));
		L_loop_th.join();
		U_loop_th.join();
	}

}

void mult(vector<vector<double>> A, vector<vector<double>> B, 
			vector<vector<double>> &R, int n)
{
	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			for(int k = 0; k < n; k++)
				R[i][j] += A[i][k] * B[k][j];
}

void show(vector<vector<double>> A, int n)
{
	for(int i = 0; i < n; i++) {
		for(int j = 0; j < n; j++)
			cout <<" "<< A[i][j] << " ";
		cout << endl;
	}
}

double determinant(vector<vector<double>> &L, vector<vector<double>> &U)
{
    double det = 1;
    for (int i = 0; i < L.size(); ++i)
        det *= L[i][i]*U[i][i];
    return det;
}

int main()
{
	vector<vector<double>> A, L, U, R;

    int n;
    cout << "Enter size of the matrix: ";
    cin >> n;

    cout << "enter values:\n";
    for(size_t i = 0; i < n; ++i) {
        vector<double> temp, temp_for_A;
        for(size_t j = 0; j < n; ++j) {
            double t;
            cin >> t;
            temp_for_A.push_back(t);
            temp.push_back(0);
        }
        A.push_back(temp_for_A);
        L.push_back(temp);
        U.push_back(temp);
        R.push_back(temp);    
    }

	LU(A,L,U,n);
	cout << "Fisrt matrix" << endl;
	show(A,n);
	cout << "U matrix" << endl;
	show(U,n);
	cout << "L matrix" << endl;
	show(L,n);
	mult(L,U,R,n);
	cout << "L*U matrix" << endl;
	show(R,n);
	
    cout << "determinant = " << determinant(L, U) << endl;
	return 0;
}


/*     A = { { 2, -1, -2, 5 },
		  { -4, 6, 3, 6 },
		  { -4, -2, 8, 7 },
          { -5, -6, 2, -3 } }; */
/*
2 -1 -2 5
-4 6 3 6 
-4 -2 8 7 
-5 -6 2 -3
det = 2397

2 -1 -2
-4 6 3 
-4 -2 8
*/