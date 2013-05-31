#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>

#include "trsv.h"
#include "trsv_blk.h"

using namespace std;

double errCalc(int n, double *exact, double *calc);
double errCalc2d(int n, int m, double *exact, double *calc);
void testTRSV(int n);
void testTRSV_BLK(int n, int m);

#define PREC 1e-5
//#define RAND_MAX 1000//

int main(int argc, char **argv) {

	if (argc != 3) {
		cerr << "Usage: " << argv[0] << " n m" << endl;
		return 1;
	}
	
	//System size:
	int n=atoi(argv[1]);
	int m=atoi(argv[2]);
	
	if (n>40) {
		cout << "Warning: routine failures may be due to truncation errors!\n" << endl;
	}
	
	testTRSV(n);
	testTRSV_BLK(n,m);
	
	
	return 0;
	
}


void testTRSV(int n) {

	//Build A and x, then compute b:
	double *A_U = new double[n*n];
	double *A_L = new double[n*n];
	double *A_U_D = new double[n*n];	//=A_U with 1s on diagonal
	double *A_L_D = new double[n*n];	//=A_L with 1s on diagonal
	double *x = new double[n];
	double *b_U = new double[n];
	double *b_L = new double[n];
	double *b_U_T = new double[n];		//=b_L
	double *b_L_T = new double[n];		//=b_U
	double *b_U_D = new double[n];
	double *b_L_D = new double[n];
	double *b_U_T_D = new double[n];
	double *b_L_T_D = new double[n];
	
	//Fill A, x with random numbers:
	srand(time(NULL));
    for (int i=0; i<n; ++i) {
        for (int j=i; j<n; ++j) {
        
        	A_U[j*n +i] = 0;
        	A_L[i*n +j] = 0;
        	
        	double tmp=rand()/(double)RAND_MAX;
            A_U[i*n +j] = tmp;
            A_L[j*n +i] = tmp;
            
            A_U_D[i*n +j] = A_U[i*n +j];
            A_L_D[j*n +i] = A_L[j*n +i];
            
        }
        
        A_U_D[i*n +i] = 1;
        A_L_D[i*n +i] = 1;
        
        x[i] = rand()/(double)RAND_MAX;
        
    }
    
    //b = Ax
    //cout << "b:" << endl;
    for (int i=0; i<n; ++i) {
    
    	b_U[i] = 0;
    	b_L[i] = 0;
    	
    	b_U_D[i] = 0;
    	b_L_D[i] = 0;
    	
    	for (int j=0; j<n; ++j) {
    	
    		b_U[i] += A_U[i*n +j] * x[j];
    		b_L[i] += A_L[i*n +j] * x[j];
    		
    		b_U_D[i] += A_U_D[i*n +j] * x[j];
    		b_L_D[i] += A_L_D[i*n +j] * x[j];
    		
    	}
    	
    	b_U_T[i] = b_L[i];
    	b_L_T[i] = b_U[i];
    	
    	b_U_T_D[i] = b_L_D[i];
    	b_L_T_D[i] = b_U_D[i];
    	
    	//cout << b_U_T[i] << endl;
    	
    }
	
	//Solve using the solver:
	int lda = n;
	int incX = 1;

	//Upper:
	trsv('U', 'N', 'N', n, A_U, lda, b_U, incX);
	trsv('U', 'T', 'N', n, A_U, lda, b_U_T, incX);
	trsv('U', 'N', 'U', n, A_U_D, lda, b_U_D, incX);
	trsv('U', 'T', 'U', n, A_U_D, lda, b_U_T_D, incX);

	//Lower:
	trsv('L', 'N', 'N', n, A_L, lda, b_L, incX);
	trsv('L', 'T', 'N', n, A_L, lda, b_L_T, incX);
	trsv('L', 'N', 'U', n, A_L_D, lda, b_L_D, incX);
	trsv('L', 'T', 'U', n, A_L_D, lda, b_L_T_D, incX);

	//Output results:
	cout << "\nResults:\n" << endl;
	
	double err_U = errCalc(n,x,b_U);
	double err_U_T = errCalc(n,x,b_U_T);
	double err_U_D = errCalc(n,x,b_U_D);
	double err_U_T_D = errCalc(n,x,b_U_T_D);
	
	double err_L = errCalc(n,x,b_L);
	double err_L_T = errCalc(n,x,b_L_T);
	double err_L_D = errCalc(n,x,b_L_D);
	double err_L_T_D = errCalc(n,x,b_L_T_D);
	
	cout << fixed << setprecision(4) << "Upper: " << err_U << "\t\t\t ----> " << ((err_U<PREC)?"PASS":"FAIL") << endl;
	cout << fixed << setprecision(4) << "Upper transpose: " << err_U_T << "\t\t ----> " << ((err_U_T<PREC)?"PASS":"FAIL") << endl;
	cout << fixed << setprecision(4) << "Upper unit: " << err_U_D << "\t\t ----> " << ((err_U_D<PREC)?"PASS":"FAIL") << endl;
	cout << fixed << setprecision(4) << "Upper unit transpose: " << err_U_T_D << "\t ----> " << ((err_U_T_D<PREC)?"PASS":"FAIL") << endl;
		
	cout << fixed << setprecision(4) << "Lower: " << err_L << "\t\t\t ----> " << ((err_L<PREC)?"PASS":"FAIL") << endl;
	cout << fixed << setprecision(4) << "Lower transpose: " << err_L_T << "\t\t ----> " << ((err_L_T<PREC)?"PASS":"FAIL") << endl;
	cout << fixed << setprecision(4) << "Lower unit: " << err_L_D << "\t\t ----> " << ((err_L_D<PREC)?"PASS":"FAIL") << endl;
	cout << fixed << setprecision(4) << "Lower unit transpose: " << err_L_T_D << "\t ----> " << ((err_L_T_D<PREC)?"PASS":"FAIL") << endl;


	//Clean up:
	delete[] A_U;
	delete[] A_L;
	delete[] A_U_D;
	delete[] A_L_D;
	delete[] x;
	delete[] b_U;
	delete[] b_L;
	delete[] b_U_T;
	delete[] b_L_T;
	delete[] b_U_D;
	delete[] b_L_D;
	delete[] b_U_T_D;
	delete[] b_L_T_D;
	
}

void testTRSV_BLK(int n, int m) {

	//Build A and x, then compute b:
	double *A_U = new double[n*n];
	double *A_L = new double[n*n];
	double *A_U_D = new double[n*n];	//=A_U with 1s on diagonal
	double *A_L_D = new double[n*n];	//=A_L with 1s on diagonal
	double *x = new double[n*m];
	double *b_U = new double[n*m];
	double *b_L = new double[n*m];
	double *b_U_T = new double[n*m];		//=b_L
	double *b_L_T = new double[n*m];		//=b_U
	double *b_U_D = new double[n*m];
	double *b_L_D = new double[n*m];
	double *b_U_T_D = new double[n*m];
	double *b_L_T_D = new double[n*m];
	
	//Fill A, x with random numbers:
	srand(time(NULL));
    for (int i=0; i<n; ++i) {
        for (int j=i; j<n; ++j) {
        
        	A_U[j*n +i] = 0;
        	A_L[i*n +j] = 0;
        	
        	double tmp = rand()/(double)RAND_MAX;
            A_U[i*n +j] = tmp;
            A_L[j*n +i] = tmp;
            
            A_U_D[i*n +j] = A_U[i*n +j];
            A_L_D[j*n +i] = A_L[j*n +i];
            
        }
        
        A_U_D[i*n +i] = 1;
        A_L_D[i*n +i] = 1;
        
        double tmp = rand()/(double)RAND_MAX;
        for (int j=0; j<m; ++j) {
        
        	x[i*m +j] = tmp;						//Columns of x all the same.
        	
        }
        
    }
    
    //b = Ax
    //cout << "b:" << endl;
    for (int k=0; k<m; ++k) {
    	for (int i=0; i<n; ++i) {
    
    		b_U[i*m +k] = 0;
	    	b_L[i*m +k] = 0;
    	
    		b_U_D[i*m +k] = 0;
    		b_L_D[i*m +k] = 0;
    	
	    	for (int j=0; j<n; ++j) {
    	
    			b_U[i*m +k] += A_U[i*n +j] * x[j*m +k];
    			b_L[i*m +k] += A_L[i*n +j] * x[j*m +k];
    		
    			b_U_D[i*m +k] += A_U_D[i*n +j] * x[j*m +k];
	    		b_L_D[i*m +k] += A_L_D[i*n +j] * x[j*m +k];
    		
    		}
    	
    		b_U_T[i*m +k] = b_L[i*m +k];
	    	b_L_T[i*m +k] = b_U[i*m +k];
    	
    		b_U_T_D[i*m +k] = b_L_D[i*m +k];
    		b_L_T_D[i*m +k] = b_U_D[i*m +k];
    	
    	//cout << b_U_T[i] << endl;
    	}
    }
    
    cout<<"\nb:"<<endl;
    for (int i=0; i<n; ++i) {
    	for (int j=0; j<m; ++j) {
    		cout << b_U[i*m +j] << " ";
    	}
    	cout << endl;
    }
    cout<<"\nx:"<<endl;
    for (int i=0; i<n; ++i) {
    	for (int j=0; j<m; ++j) {
    		cout << x[i*m +j] << " ";
    	}
    	cout << endl;
    }
    cout<<"\nA:"<<endl;
    for (int i=0; i<n; ++i) {
    	for (int j=0; j<n; ++j) {
    		cout << A_U[i*n +j] << " ";
    	}
    	cout << endl;
    }
	
	//Solve using the solver:
	int lda = n;
	int ldx = m;
	int incX = 1;

	//Upper:
	trsv_blk('U', 'N', 'N', n, A_U, lda, b_U, incX, ldx, m);
	trsv_blk('U', 'T', 'N', n, A_U, lda, b_U_T, incX, ldx, m);
	trsv_blk('U', 'N', 'U', n, A_U_D, lda, b_U_D, incX, ldx, m);
	trsv_blk('U', 'T', 'U', n, A_U_D, lda, b_U_T_D, incX, ldx, m);

	//Lower:
	trsv_blk('L', 'N', 'N', n, A_L, lda, b_L, incX, ldx, m);
	trsv_blk('L', 'T', 'N', n, A_L, lda, b_L_T, incX, ldx, m);
	trsv_blk('L', 'N', 'U', n, A_L_D, lda, b_L_D, incX, ldx, m);
	trsv_blk('L', 'T', 'U', n, A_L_D, lda, b_L_T_D, incX, ldx, m);

	//Output results:
	cout << "\nResults:\n" << endl;
	
	double err_U = errCalc2d(n,m,x,b_U);
	double err_U_T = errCalc2d(n,m,x,b_U_T);
	double err_U_D = errCalc2d(n,m,x,b_U_D);
	double err_U_T_D = errCalc2d(n,m,x,b_U_T_D);
	
	double err_L = errCalc2d(n,m,x,b_L);
	double err_L_T = errCalc2d(n,m,x,b_L_T);
	double err_L_D = errCalc2d(n,m,x,b_L_D);
	double err_L_T_D = errCalc2d(n,m,x,b_L_T_D);
	
	cout << fixed << setprecision(4) << "Upper: " << err_U << "\t\t\t ----> " << ((err_U<PREC)?"PASS":"FAIL") << endl;
	cout << fixed << setprecision(4) << "Upper transpose: " << err_U_T << "\t\t ----> " << ((err_U_T<PREC)?"PASS":"FAIL") << endl;
	cout << fixed << setprecision(4) << "Upper unit: " << err_U_D << "\t\t ----> " << ((err_U_D<PREC)?"PASS":"FAIL") << endl;
	cout << fixed << setprecision(4) << "Upper unit transpose: " << err_U_T_D << "\t ----> " << ((err_U_T_D<PREC)?"PASS":"FAIL") << endl;
		
	cout << fixed << setprecision(4) << "Lower: " << err_L << "\t\t\t ----> " << ((err_L<PREC)?"PASS":"FAIL") << endl;
	cout << fixed << setprecision(4) << "Lower transpose: " << err_L_T << "\t\t ----> " << ((err_L_T<PREC)?"PASS":"FAIL") << endl;
	cout << fixed << setprecision(4) << "Lower unit: " << err_L_D << "\t\t ----> " << ((err_L_D<PREC)?"PASS":"FAIL") << endl;
	cout << fixed << setprecision(4) << "Lower unit transpose: " << err_L_T_D << "\t ----> " << ((err_L_T_D<PREC)?"PASS":"FAIL") << endl;


	//Clean up:
	delete[] A_U;
	delete[] A_L;
	delete[] A_U_D;
	delete[] A_L_D;
	delete[] x;
	delete[] b_U;
	delete[] b_L;
	delete[] b_U_T;
	delete[] b_L_T;
	delete[] b_U_D;
	delete[] b_L_D;
	delete[] b_U_T_D;
	delete[] b_L_T_D;
	
}

double errCalc(int n, double *exact, double *calc) {

	double error = 0;
	
	for (int i=0; i<n; ++i) {
	
		error += fabs(exact[i] - calc[i]);
		
	}
	
	return error;
		
}

double errCalc2d(int n, int m, double *exact, double *calc) {

	double error = 0;
	
	for (int i=0; i<n; ++i) {
		for (int j=0; j<m; ++j) {
		
			error += fabs(exact[i*m +j] - calc[i*m +j]);
			
		}
	}
	
	return error;
	
}