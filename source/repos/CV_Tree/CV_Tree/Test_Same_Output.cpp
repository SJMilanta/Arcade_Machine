#include "pch.h"
/*#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <Windows.h>

#define SEQUENTIAL 0
#define PARALLEL_INIT 1
#define PARALLEL_COMP 2
#define PARALLEL_POOL 3
#define PARALLEL_BARRIER 4

#define MODE 4


SYNCHRONIZATION_BARRIER barrier_lock;
time_t timer2;
int number_bacteria;
char** bacteria_name;
long M, M1, M2;
short code[27] = {0, 2, 1, 2, 3, 4, 5, 6, 7, -1, 8, 9, 10, 11, -1, 12, 13, 14, 15, 16, 1, 17, 18, 5, 19, 3};
#define encode(ch)		code[ch-'A']
#define LEN				6
#define AA_NUMBER		20
#define	EPSILON			1e-010
#define NUM_THREADS		8

void Init() {
	M2 = 1;
	for ( int i = 0; i < LEN - 2; i++ )	// M2 = AA_NUMBER ^ (LEN-2);
		M2 *= AA_NUMBER;
	M1 = M2 * AA_NUMBER;		// M1 = AA_NUMBER ^ (LEN-1);
	M = M1 * AA_NUMBER;			// M  = AA_NUMBER ^ (LEN);
}


class Bacteria {
private:
	long* vector;
	long* second;
	long one_l[AA_NUMBER];
	long indexs;
	long total;
	long total_l;
	long complement;

	void InitVectors() {
		vector = new long[M];
		second = new long[M1];
		memset( vector, 0, M * sizeof( long ) );			// Filling vector with zeros
		memset( second, 0, M1 * sizeof( long ) );			// Filling vector with zeros
		memset( one_l, 0, AA_NUMBER * sizeof( long ) );		// Filling vector with zeros
		total = 0;
		total_l = 0;
		complement = 0;
	}

	void init_buffer( char* buffer ) {
		complement++;
		indexs = 0;
		for ( int i = 0; i < LEN - 1; i++ ) {
			short enc = encode( buffer[i] );
			one_l[enc]++;
			total_l++;
			indexs = indexs * AA_NUMBER + enc;
		}
		second[indexs]++;
	}

	void cont_buffer( char ch ) {
		short enc = encode( ch );
		one_l[enc]++;
		total_l++;
		long index = indexs * AA_NUMBER + enc;
		vector[index]++;
		total++;
		indexs = (indexs % M2) * AA_NUMBER + enc;
		second[indexs]++;
	}

public:
	long count;
	double* tv;
	long *ti;

	Bacteria( char* filename ) {
		FILE * bacteria_file = fopen( filename, "r" );
		InitVectors();

		char ch;
		while ( (ch = fgetc( bacteria_file )) != EOF ) {
			if ( ch == '>' ) {
				while ( fgetc( bacteria_file ) != '\n' ); // skip rest of line

				char buffer[LEN - 1];
				fread( buffer, sizeof( char ), LEN - 1, bacteria_file );
				init_buffer( buffer );
			} else if ( ch != '\n' ) {
				cont_buffer( ch );
			}

		}

		long total_plus_complement = total + complement;
		double total_div_2 = total * 0.5;
		int i_mod_aa_number = 0;
		int i_div_aa_number = 0;
		long i_mod_M1 = 0;
		long i_div_M1 = 0;

		double one_l_div_total[AA_NUMBER];
		for ( int i = 0; i < AA_NUMBER; i++ ) {
			one_l_div_total[i] = (double) one_l[i] / total_l;
		}


		double* second_div_total = new double[M1];
		for ( int i = 0; i < M1; i++ ) {
			second_div_total[i] = (double) second[i] / total_plus_complement;
		}


		count = 0;
		double* t = new double[M];

		for ( long i = 0; i < M; i++ ) {
			double p1 = second_div_total[i_div_aa_number];
			double p2 = one_l_div_total[i_mod_aa_number];
			double p3 = second_div_total[i_mod_M1];
			double p4 = one_l_div_total[i_div_M1];
			double stochastic = (p1 * p2 + p3 * p4) * total_div_2;

			if ( i_mod_aa_number == AA_NUMBER - 1 ) {
				i_mod_aa_number = 0;
				i_div_aa_number++;
			} else {
				i_mod_aa_number++;
			}

			if ( i_mod_M1 == M1 - 1 ) {
				i_mod_M1 = 0;
				i_div_M1++;
			} else {
				i_mod_M1++;
			}


			if ( stochastic > EPSILON ) {
				t[i] = (vector[i] - stochastic) / stochastic;
				count++;
			} else {
				t[i] = 0;
			}
		}

		delete second_div_total;
		delete vector;
		delete second;

		tv = new double[count];
		ti = new long[count];

		int pos = 0;
		for ( long i = 0; i < M; i++ ) {
			if ( t[i] != 0 ) {
				tv[pos] = t[i];
				ti[pos] = i;
				pos++;
			}
		}
		delete t;

		fclose( bacteria_file );
	}
};

void ReadInputFile( char* input_name ) {
	FILE* input_file = fopen( input_name, "r" );
	fscanf( input_file, "%d", &number_bacteria );
	bacteria_name = new char*[number_bacteria];

	for ( long i = 0; i < number_bacteria; i++ ) {
		bacteria_name[i] = new char[20];
		fscanf( input_file, "%s", bacteria_name[i] );
		strcat( bacteria_name[i], ".faa" );
	}
	fclose( input_file );
}

double CompareBacteria( Bacteria* b1, Bacteria* b2 ) {
	double correlation = 0;
	double vector_len1 = 0;
	double vector_len2 = 0;
	long p1 = 0;
	long p2 = 0;
	while ( p1 < b1->count && p2 < b2->count ) {
		long n1 = b1->ti[p1];
		long n2 = b2->ti[p2];
		if ( n1 < n2 ) {
			double t1 = b1->tv[p1];
			vector_len1 += (t1 * t1);
			p1++;
		} else if ( n2 < n1 ) {
			double t2 = b2->tv[p2];
			p2++;
			vector_len2 += (t2 * t2);
		} else {
			double t1 = b1->tv[p1++];
			double t2 = b2->tv[p2++];
			vector_len1 += (t1 * t1);
			vector_len2 += (t2 * t2);
			correlation += t1 * t2;
		}
	}
	while ( p1 < b1->count ) {
		long n1 = b1->ti[p1];
		double t1 = b1->tv[p1++];
		vector_len1 += (t1 * t1);
	}
	while ( p2 < b2->count ) {
		long n2 = b2->ti[p2];
		double t2 = b2->tv[p2++];
		vector_len2 += (t2 * t2);
	}

	return correlation / (sqrt( vector_len1 ) * sqrt( vector_len2 ));
}

Bacteria** b;

// Original Sequential Version
void CompareAllBacteriaSeq() {
	b = new Bacteria*[number_bacteria];

	for ( int i = 0; i < number_bacteria; i++ ) {
		printf( "load %d of %d\n", i + 1, number_bacteria );
		b[i] = new Bacteria( bacteria_name[i] );
	}

	timer2 = time( NULL );

	for ( int i = 0; i < number_bacteria - 1; i++ ) {
		for ( int j = i + 1; j < number_bacteria; j++ ) {
			printf( "%2d %2d -> ", i, j );
			double correlation = CompareBacteria( b[i], b[j] );
			printf( "%.20lf\n", correlation );
		}
	}
}


//	Parallel
DWORD WINAPI BacteriaInit( LPVOID lpThreadParameter ) {
	int input = (int) lpThreadParameter;
	int chunk_size = (number_bacteria + (NUM_THREADS - 1)) / NUM_THREADS;
	int first = chunk_size * input;
	int last = (number_bacteria < (first + chunk_size) ? number_bacteria : (first + chunk_size));

	for ( int i = first; i < last; i++ ) {
		printf( "load %d of %d\n", i + 1, number_bacteria );
		b[i] = new Bacteria( bacteria_name[i] );
	}
	return 0;
}

DWORD WINAPI BacteriaComp1( LPVOID lpThreadParameter ) {
	int input = (int) lpThreadParameter;
	int chunk_size = (number_bacteria + (NUM_THREADS - 1)) / NUM_THREADS;
	int first = chunk_size * input;
	int last = (number_bacteria < (first + chunk_size) ? number_bacteria : (first + chunk_size));

	for ( int i = first; i < last - 1; i++ ) {
		for ( int j = i + 1; j < number_bacteria; j++ ) {
			printf( "%2d %2d -> ", i, j );
			double correlation = CompareBacteria( b[i], b[j] );
			printf( "%.20lf\n", correlation );
		}
	}
	return 0;
}

DWORD WINAPI BacteriaComp2( LPVOID lpThreadParameter ) {
	int thread_id = (int) lpThreadParameter;

	for ( int i = thread_id; i < ceil( number_bacteria / 2 ); i += NUM_THREADS ) {
		for ( int j = i + 1; j < number_bacteria; j++ ) {
			printf( "%2d %2d -> ", i, j );
			double correlation = CompareBacteria( b[i], b[j] );
			printf( "%.20lf\n", correlation );
		}
	}

	for ( int i = number_bacteria - (thread_id + 1); i > floor( number_bacteria / 2 ); i -= NUM_THREADS ) {
		for ( int j = i + 1; j < number_bacteria; j++ ) {
			printf( "%2d %2d -> ", i, j );
			double correlation = CompareBacteria( b[i], b[j] );
			printf( "%.20lf\n", correlation );
		}
	}

	return 0;
}

void CompareAllBacteriaInitP() {

	b = new Bacteria*[number_bacteria];

	HANDLE *threadHandles = (HANDLE*) malloc( sizeof( HANDLE ) * NUM_THREADS );
	for ( int threadId = 0; threadId < NUM_THREADS; threadId++ ) {
		threadHandles[threadId] = CreateThread( NULL, NULL, BacteriaInit, (LPVOID) threadId, NULL, NULL );
	}
	WaitForMultipleObjects( NUM_THREADS, threadHandles, TRUE, INFINITE );

	timer2 = time( NULL );

	for ( int i = 0; i < number_bacteria - 1; i++ ) {
		for ( int j = i + 1; j < number_bacteria; j++ ) {
			printf( "%2d %2d -> ", i, j );
			double correlation = CompareBacteria( b[i], b[j] );
			printf( "%.20lf\n", correlation );
		}
	}
}

void CompareAllBacteriaCompP() {

	b = new Bacteria*[number_bacteria];

	for ( int i = 0; i < number_bacteria; i++ ) {
		printf( "load %d of %d\n", i + 1, number_bacteria );
		b[i] = new Bacteria( bacteria_name[i] );
	}

	timer2 = time( NULL );

	HANDLE *threadHandles = (HANDLE*) malloc( sizeof( HANDLE ) * NUM_THREADS );
	for ( int threadId = 0; threadId < NUM_THREADS; threadId++ ) {
		threadHandles[threadId] = CreateThread( NULL, NULL, BacteriaComp1, (LPVOID) threadId, NULL, NULL );
	}
	WaitForMultipleObjects( NUM_THREADS, threadHandles, TRUE, INFINITE );
}


void CompareAllBacteriaParallel() {

	b = new Bacteria*[number_bacteria];

	HANDLE *threadHandles = (HANDLE*) malloc( sizeof( HANDLE ) * NUM_THREADS );
	for ( int threadId = 0; threadId < NUM_THREADS; threadId++ ) {
		threadHandles[threadId] = CreateThread( NULL, NULL, BacteriaInit, (LPVOID) threadId, NULL, NULL );
	}
	WaitForMultipleObjects( NUM_THREADS, threadHandles, TRUE, INFINITE );

	timer2 = time( NULL );

	for ( int threadId = 0; threadId < NUM_THREADS; threadId++ ) {
		threadHandles[threadId] = CreateThread( NULL, NULL, BacteriaComp1, (LPVOID) threadId, NULL, NULL );
	}
	WaitForMultipleObjects( NUM_THREADS, threadHandles, TRUE, INFINITE );
}

DWORD WINAPI BacteriaParallelBarrier( LPVOID lpThreadParameter ) {
	int input = (int) lpThreadParameter;
	int chunk_size = (number_bacteria + (NUM_THREADS - 1)) / NUM_THREADS;
	int first = chunk_size * input;
	int last = (number_bacteria < (first + chunk_size) ? number_bacteria : (first + chunk_size));

	for ( int i = first; i < last; i++ ) {
		printf( "load %d of %d\n", i + 1, number_bacteria );
		b[i] = new Bacteria( bacteria_name[i] );
	}

	if ( EnterSynchronizationBarrier( &barrier_lock, 0 ) ) {
		timer2 = time( NULL );
	}

	for ( int i = first; i < last - 1; i++ ) {
		for ( int j = i + 1; j < number_bacteria; j++ ) {
			printf( "%2d %2d -> ", i, j );
			double correlation = CompareBacteria( b[i], b[j] );
			printf( "%.20lf\n", correlation );
		}
	}
	return 0;
}


void CompareAllBacteriaParallelBarrier() {

	b = new Bacteria*[number_bacteria];

	HANDLE *threadHandles = (HANDLE*) malloc( sizeof( HANDLE ) * NUM_THREADS );
	for ( int threadId = 0; threadId < NUM_THREADS; threadId++ ) {
		threadHandles[threadId] = CreateThread( NULL, NULL, BacteriaParallelBarrier, (LPVOID) threadId, NULL, NULL );
	}
	WaitForMultipleObjects( NUM_THREADS, threadHandles, TRUE, INFINITE );
}


int main( int argc, char * argv[] ) {
	char *filename = const_cast<char*>("list.txt");
	time_t timer1 = time( NULL );

	switch ( MODE ) {
	case SEQUENTIAL:
		printf( "Sequential Version\n" );
		Init();
		ReadInputFile( filename );
		CompareAllBacteriaSeq();
		break;
	case PARALLEL_INIT:
		printf( "Parallel Initialisation, Sequentional Comparison\n" );
		Init();
		ReadInputFile( filename );
		CompareAllBacteriaInitP();
		break;
	case PARALLEL_COMP:
		printf( "Sequential Initialisation, Parallel Comparison\n" );
		Init();
		ReadInputFile( filename );
		CompareAllBacteriaCompP();
		break;
	case PARALLEL_POOL:
		printf( "Parallel Initialisation, Parallel Comparison - No Barrier Lock\n" );
		Init();
		ReadInputFile( filename );
		CompareAllBacteriaParallel();
		break;
	case PARALLEL_BARRIER:
		printf( "Parallel with Barrier Lock Version\n" );
		if ( InitializeSynchronizationBarrier( &barrier_lock, NUM_THREADS, -1 ) ) {
			Init();
			ReadInputFile( filename );
			CompareAllBacteriaParallelBarrier();
			DeleteSynchronizationBarrier( &barrier_lock );
		} else {
			printf( "Error: Barrier Failed" );
		}
		break;
	default:
		printf( "Error: Undefined Mode" );
		break;
	}
	time_t timer3 = time( NULL );
	printf( "Total time elapsed: %d seconds\n", (int) (timer3 - timer1) );
	printf( "Time taken for initialisation: %d\n", (int) (timer2 - timer1) );
	printf( "Time taken for comparisons: %d", (int) (timer3 - timer2) );

	return 0;
}
*/