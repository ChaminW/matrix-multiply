/*
 * Parallel Matrix Multiplication using openMP with transpose - Optimization 1
 * Compile with -fopenmp flag
 * Author: Chamin Wickramarathna
 */

#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <omp.h>
#include <cmath>

using namespace std;

const int NUM_THREADS = 4;

void initMat(vector< vector<double> > &a,vector< vector<double> > &b,int n){
		// Initialize arrays.
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				a[i][j] = (double)rand()/RAND_MAX*10;
				b[i][j] = (double)rand()/RAND_MAX*10;
				
			}
		}
	}

void multiplyMatParallel(vector< vector<double> > &a,vector< vector<double> > &b, vector< vector<double> > &c, int n){
		// Compute matrix multiplication.
		// C <- C + A x B
		// Use omp parrelle for loop

		vector< vector<double> > btrans(n,vector<double>(n));

		for(int i = 0; i < n; ++i){
			for(int j = 0; j < n; ++j){
				btrans[j][i]=b[i][j];
			}
		}

		
		#pragma omp parallel for num_threads(NUM_THREADS)
		
			for (int i = 0; i < n; ++i) {
				vector<double> Rowmatrix1  = a[i];
				#pragma omp parallel for num_threads(NUM_THREADS) 
				for (int j = 0; j < n; ++j) {	
					double temp  = 0;
					vector<double> Rowmatrix2 = btrans[j];
					for (int k = 0; k < n; ++k) {
						temp += Rowmatrix1[k] * Rowmatrix2[k];
					}
					c[i][j]=temp;
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
	const char *filename = "results/mat_multi_para_optimized_1.txt";    //file to store results of execution 
	ofstream f;

	srand(time(0));   //seed for random number generation
	const int sampleSize = 20;      // Number of sample size consider to evaluate average time taken
	const int maxSize = 2000;       // maximum size of the 2d matrix
	double dtime;
	double parMean;
	double sd;
	double sampleCount;
	
	//open file to append
	f.open(filename, ios::trunc);
    if (!f.is_open()) {
        cout << "Unable to open the file" << endl;
        exit(1);
    }

	cout << "Parallel multiplication using openMP - Optimized - v1"<< endl;
	f << "Parallel multiplication using openMP - Optimized - v1\n";

	for (int n = 200; n <= maxSize; n+=200) {

		//vectors storing execution time values
		vector<double> seqTime(sampleSize);      
		vector<double> parTime(sampleSize);
		
		for (int k = 0; k < sampleSize; k++) {
			vector< vector<double> > a(n,vector<double>(n)),b(n,vector<double>(n)),c(n,vector<double>(n));	//c = a * b, c is the result matrix
			
			initMat(a,b,n);
		
			//parallel execution
			dtime = 0;			
			dtime = omp_get_wtime();
			multiplyMatParallel(a,b,c,n);
			dtime = omp_get_wtime() - dtime;
			parTime[k] = dtime;
		}
		
		parMean = calculateMean(parTime, sampleSize);
		sd = calculateSD(parTime, sampleSize, parMean);
		sampleCount = calculateSampleCount(parMean, sd);
		
		cout << "Average time taken to execute in n-" << n << " : " << parMean << endl;
		cout << "Standard deviation for execution in n-" << n << " : " << sd << endl;
		cout << "Sample count for n-" << n << " : " << sampleCount << endl;
		cout << endl;

		f << "\n--- n : " << n << " ---\n";
		f << "Average time taken to execute in n-" << n << " : " << parMean << " seconds\n\n";		
		
	}
	f.close();
    return 0;
}


	
