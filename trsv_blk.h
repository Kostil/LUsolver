#ifndef LUSOLVER_TRSV_BLK_H
#define LUSOLVER_TRSV_BLK_H 1

template <typename IndexType, typename A, typename X>
void
trsv_blk(char uplo, char trans, char diag, IndexType n,
		const A *a, IndexType lda,
				X *x, IndexType incX,
					IndexType ldx, IndexType m)
{

/***** Triangular Solver where b is a matrix *****
I.e. we solve Ax(:,i) = b(:,i) where i=0,...,#of cols of b -1

Implementation works bottom up, solving each system at the same time - should be most
cache friendly wrt accessing x.

***** Input parameters: *****

uplo:
	CHARACTER*1. 
	Specifies whether the matrix A is upper or lower triangular:
		if uplo = 'U' or 'u', then the matrix is upper triangular;
		if uplo = 'L' or 'l', then the matrix is low triangular.

trans:
	CHARACTER*1. 
	Specifies the systems of equations:
		if trans = 'N' or 'n', then A*x = b;
		if trans = 'T' or 't', then A'*x = b;
		if trans = 'C' or 'c', then oconjg(A')*x = b.		<--- NOT IMPLEMENTED

diag:
	CHARACTER*1. 
	Specifies whether the matrix A is unit triangular:
		if diag = 'U' or 'u' then the matrix is unit triangular;
		if diag = 'N' or 'n', then the matrix is not unit triangular.

n:
	INTEGER. 
	Specifies the order of the matrix A. The value of n must be at least zero.

a:
	REAL for strsv
	DOUBLE PRECISION for dtrsv
	COMPLEX for ctrsv
	DOUBLE COMPLEX for ztrsv
	Array, DIMENSION (lda,n). 
		Before entry with uplo = 'U' or 'u', the leading n-by-n upper triangular part of 
		the array a must contain the upper triangular matrix and the strictly lower 
		triangular part of a is not referenced. --> It contains junk?
		Before entry with uplo = 'L' or 'l', the leading n-by-n lower triangular part of 
		the array a must contain the lower triangular matrix and the strictly upper 
		triangular part of a is not referenced.
		When diag = 'U' or 'u', the diagonal elements of a are not referenced either, but are assumed to be unity.

lda:
	INTEGER. 
	Specifies the leading dimension of a as declared in the calling (sub)program. 
	The value of lda must be at least max(1, n).

x:
	REAL for strsv
	DOUBLE PRECISION for dtrsv
	COMPLEX for ctrsv
	DOUBLE COMPLEX for ztrsv
	Array, DIMENSION at least (1 + (n - 1)*abs(incx)). 
		Before entry, the incremented array x must contain the n-element right-hand 
		side vector b.

incx:
	INTEGER. 
	Specifies the increment for the elements of x.
		The value of incx must not be zero.

ldx:
	INTEGER
	Specified the leading dimension of x.
	The value of ldx must be at least max(1,m).
	
m:
	INTEGER
	Specifies the number of columns of x. Must be at least zero.
		
ASSUMES ROW-MAJOR STORAGE

****************************************/

/*****UPPER TRIANGULAR*****/
if (uplo == 'U' || uplo == 'u') {

	/*****UPPER TRIANGULAR / UNIT TRIANGULAR*****/
	if (diag == 'U' || diag == 'u') {

		/*****UPPER TRIANGULAR / UNIT TRIANGULAR / NO TRANSPOSE*****/
		if (trans == 'N' || trans == 'n') {

			for (int i=n-1; i>=0; --i) {
				for (int j=i+1; j<n; ++j) {
					for (int k=0; k<m; ++k) {			//For each system

						x[i*ldx +k] -= a[i*lda +j] * x[j*ldx +k];
						
					}
				}
			}

		}

		/*****UPPER TRIANGULAR / UNIT TRIANGULAR / TRANSPOSE*****/
		else if (trans == 'T' || trans == 't') {

			for (int i=0; i<n; ++i) {
				for (int j=0; j<i; ++j) {
					for (int k=0; k<m; ++k) {			//For each system

						x[i*ldx +k] -= a[j*lda +i] * x[j*ldx +k];
						
					}

				}
			}

		}

	}	//UNIT TRIANGULAR

	/*****UPPER TRIANGULAR / NOT UNIT TRIANGULAR*****/
	else if (diag == 'N' || diag == 'n') {

		/*****UPPER TRIANGULAR / NOT UNIT TRIANGULAR / NO TRANSPOSE*****/
		if (trans == 'N' || trans == 'n') {

			for (int i=n-1; i>=0; --i) {

				for (int j=i+1; j<n; ++j) {
					for (int k=0; k<m; ++k) {			//For each system

						x[i*ldx +k] -= a[i*lda +j] * x[j*ldx +k];
						
					}
				}

				for (int k=0; k<m; ++k) {			//For each system
					
					x[i*ldx +k] /= a[i*lda +i];
					
				}

			}

		}

		/*****UPPER TRIANGULAR / NOT UNIT TRIANGULAR / TRANSPOSE*****/
		else if (trans == 'T' || trans == 't') {

			for (int i=0; i<n; ++i) {

				for (int j=0; j<i; ++j) {
					for (int k=0; k<m; ++k) {			//For each system

						x[i*ldx +k] -= a[j*lda +i] * x[j*ldx +k];
						
					}
				}

				for (int k=0; k<m; ++k) {			//For each system
				
					x[i*ldx +k] /= a[i*lda +i];
					
				}

			}

		}

	}	//NOT UNIT TRIANGULAR

}	//UPPER TRIANGULAR

/*****LOWER TRIANGULAR*****/
else if (uplo == 'L' || uplo == 'l') {

	/*****LOWER TRIANGULAR / UNIT TRIANGULAR*****/
	if (diag == 'U' || diag == 'u') {
		/*****LOWER TRIANGULAR / UNIT TRIANGULAR / NO TRANSPOSE*****/
		if (trans == 'N' || trans == 'n') {

			for (int i=0; i<n; ++i) {
				for (int j=0; j<i; ++j) {
					for (int k=0; k<m; ++k) {			//For each system

						x[i*ldx +k] -= a[i*lda +j] * x[j*ldx +k];

					}
				}
			}

		}
	
		/*****LOWER TRIANGULAR / UNIT TRIANGULAR / TRANSPOSE*****/
		else if (trans == 'T' || trans == 't') {

			for (int i=n-1; i>=0; --i) {
				for (int j=i+1; j<n; ++j) {
					for (int k=0; k<m; ++k) {			//For each system

						x[i*ldx +k] -= a[j*lda +i] * x[j*ldx +k];

					}
				}
			}

		}
		
	}	//UNIT TRIANGULAR
	
	/*****LOWER TRIANGULAR / NOT UNIT TRIANGULAR*****/
	else if (diag == 'N' || diag == 'n') {
	
		/*****LOWER TRIANGULAR / NOT UNIT TRIANGULAR / NO TRANSPOSE*****/
		if (trans == 'N' || trans == 'n') {

			for (int i=0; i<n; ++i) {

				for (int j=0; j<i; ++j) {
					for (int k=0; k<m; ++k) {			//For each system
			
						x[i*ldx +k] -= a[i*lda +j] * x[j*ldx +k];
						
					}
				}

				for (int k=0; k<m; ++k) {			//For each system
				
					x[i*ldx +k] /= a[i*lda +i];
					
				}

			}

		}
	
		/*****LOWER TRIANGULAR / NOT UNIT TRIANGULAR / TRANSPOSE*****/
		else if (trans == 'T' || trans == 't') {
	
			for (int i=n-1; i>=0; --i) {

				for (int j=i+1; j<n; ++j) {
					for (int k=0; k<m; ++k) {			//For each system

						x[i*ldx +k] -= a[j*lda +i] * x[j*ldx +k];

					}
				}

				for (int k=0; k<m; ++k) {			//For each system
				
					x[i*ldx +k] /= a[i*lda +i];
					
				}

			}

		}

	}	//NOT UNIT TRIANGULAR

}	//LOWER TRIANGULAR

}

#endif	// LUSOLVER_TRSV_BLK_H
