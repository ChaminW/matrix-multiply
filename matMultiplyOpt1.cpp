/*
 * Parallel Matrix Multiplication using openMP with transpose - Optimization 1
 * Compile with -fopenmp flag
 * Author: Chamin Wickramarathna
 */

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <omp.h>
#include <cmath>

using namespace std;

void initMat(vector< vector<double> > &a,vector< vector<double> > &b,int n){
		// Initialize arrays.
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				a[i][j] = (double)rand()/RAND_MAX*10;
				b[i][j] = (double)rand()/RAND_MAX*10;
				
			}
		}
	}
void transpose(vector< vector<double> > &b, vector< vector<double> > &btrans, int n) {
		// Finding transpose of matrix b[][] and storing it in matrix btrans[][]
		for(int i = 0; i < n; ++i)
			for(int j = 0; j < n; ++j)
			{
				btrans[j][i]=b[i][j];
			}
	}
	
void multiplyTMatSeq(vector< vector<double> > &a,vector< vector<double> > &b, vector< vector<double> > &c, int n){
		// Compute matrix multiplication.
		// Transpose the B matrix
		// C <- C + A x Btrans
		vector< vector<double> > btrans(n,vector<double>(n));
		transpose(b,btrans,n);
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				double temp  = 0;
				for (int k = 0; k < n; ++k) {
					temp += a[i][k] * btrans[j][k];
				}
				c[i][j]=temp;
			}
		}
	}

	void multiplyTMatParallel(vector< vector<double> > &a,vector< vector<double> > &b, vector< vector<double> > &c, int n){
		// Compute matrix multiplication.
		// Transpose the B matrix
		// C <- C + A x Btrans
		// Use omp parrelle for loop
		vector< vector<double> > btrans(n,vector<double>(n));
		transpose(b,btrans,n);
		 int i,j,k;
		#pragma omp parallel shared(a,b,c) private(i,j,k) 
		{
			#pragma omp for schedule(static)
			for (i = 0; i < n; ++i) {
				for (j = 0; j < n; ++j) {	
					double temp  = 0;
					for (k = 0; k < n; ++k) {
						temp += a[i][k] * btrans[j][k];
					}
					c[i][j]=temp;
				}
			}
		}
	}
	
double calculateMean(vector<double> data, int size) {
    double sum = 0.0, mean = 0.0;
    for (int i = 0; i < size; ++i) {
        sum += data[i];
    }

    mean = sum / size;
    return mean;
}

double calculateSD(vector<double> data, int size, double mean) {
    double sum = 0.0, standardDeviation = 0.0;

    for (int i = 0; i < size; ++i)
        standardDeviation += pow(data[i] - mean, 2);

    return sqrt(standardDeviation / size);
}

double calculateSampleCount(double mean, double sd) {
    double sample_count = 100 * 1.96 * sd / (5 * mean);
    return sample_count;
}	
	
int main()
{
	srand(time(0));   //seed for random number generation
	
	const int sampleSize = 20;      // Number of sample size consider to evaluate average time taken
	const int maxSize = 600;       // maximum size of the 2d matrix
	double dtime;
	double seqMean;
	double parMean;
	double sd;
	double sampleCount;
	
	for (int n = 200; n <= maxSize; n+=200) {

		//vectors storing execution time values
		vector<double> seqTime(sampleSize);      
		vector<double> parTime(sampleSize);
		
		for (int k = 0; k < sampleSize; k++) {
			vector< vector<double> > a(n,vector<double>(n)),b(n,vector<double>(n)),c(n,vector<double>(n));	//c = a * b, c is the result matrix
			
			initMat(a,b,n);
			
			//sequential execution		
			dtime = omp_get_wtime();
			multiplyTMatSeq(a,b,c,n);
			dtime = omp_get_wtime() - dtime;
			//cout << "Time taken to execute in n-"<< n << " : "<< dtime << endl;
			seqTime[k] = dtime;
			
			//parallel execution
			dtime = 0;			
			dtime = omp_get_wtime();
			multiplyTMatParallel(a,b,c,n);
			dtime = omp_get_wtime() - dtime;
			parTime[k] = dtime;
		}
		cout << "Sequential multiplication"<< endl;
		seqMean = calculateMean(seqTime, sampleSize);
		sd = calculateSD(seqTime, sampleSize, seqMean);
		sampleCount = calculateSampleCount(seqMean, sd);
		
		cout << "Average time taken to execute in n-" << n << " : " << seqMean << endl;
		cout << "Standard deviation for execution in n-" << n << " : " << sd << endl;
		cout << "Sample count for n-" << n << " : " << sampleCount << endl;
		cout << endl;
		
		cout << "Parallel multiplication using openMP"<< endl;
		parMean = calculateMean(parTime, sampleSize);
		sd = calculateSD(parTime, sampleSize, parMean);
		sampleCount = calculateSampleCount(parMean, sd);
		
		cout << "Average time taken to execute in n-" << n << " : " << parMean << endl;
		cout << "Standard deviation for execution in n-" << n << " : " << sd << endl;
		cout << "Sample count for n-" << n << " : " << sampleCount << endl;
		cout << endl;
		
		cout << "Speed up after Parallelizing for n-" << n << " : " << seqMean/parMean << endl;
		cout << endl;
	}
	
	/* cout << "Sequential multiplication with transpose - Optimization 1"<< endl;
	count = 0;
	for (int n = 200; n <= maxSize; n+=200) {
		double total_time = 0;
		for (int k = 0; k < sampleSize; k++) {
			vector< vector<double> > a(n,vector<double>(n)),b(n,vector<double>(n)),btrans(n,vector<double>(n)),c(n,vector<double>(n));	//c = a * b, c is the result matrix
			
			initMat(a,b,n);
			
			dtime = omp_get_wtime();
			multiplyTMatSeq(a,b,c,n);
			dtime = omp_get_wtime() - dtime;
			//cout << "Time taken to execute in n-"<< n << " : "<< dtime << endl;
			total_time+= dtime;
		}
		cout << "Average time taken to execute in n-"<< n << " : "<< total_time/sampleSize << endl;
		trnTime[count] = total_time/sampleSize;
		count++;
	}
	
	cout << "Parallel multiplication using openMP with transpose - Optimization 1"<< endl;
	count = 0;
	for (int n = 200; n <= maxSize; n+=200) {
		double total_time = 0;
		for (int k = 0; k < sampleSize; k++) {
			vector< vector<double> > a(n,vector<double>(n)),b(n,vector<double>(n)),btrans(n,vector<double>(n)),c(n,vector<double>(n));	//c = a * b, c is the result matrix
			
			initMat(a,b,n);
			
			dtime = omp_get_wtime();
			multiplyTMatParallel(a,b,c,n);
			dtime = omp_get_wtime() - dtime;
			//cout << "Time taken to execute in n-"<< n << " : "<< dtime << endl;
			total_time+= dtime;
		}
		cout << "Average time taken to execute in n-"<< n << " : "<< total_time/sampleSize << endl;
		trnParTime[count] = total_time/sampleSize;
		count++;
	} */
	
    return 0;
}


	
