#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <iostream>
#include <windows.h>

int liczPiSekwencyjnie() {
	clock_t start, stop;
	double x, pi, sum = 0.0;
	int i;
	long long num_steps = 100000000;
	double step;
	step = 1. / (double)num_steps;
	start = clock();
	for (i = 0; i<num_steps; i++)
	{
		x = (i + .5)*step;
		sum = sum + 4.0 / (1. + x*x);
	}

	pi = sum*step;
	stop = clock();

	printf("Wartosc liczby PI wynosi %15.12f\n", pi);
	printf("Czas przetwarzania wynosi %f sekund\n", ((double)(stop - start) / 1000.0));
	return 0;
}

int liczPiRownolegle() {
	clock_t start, stop;
	double x, pi, sum = 0.0;
	int i;
	long long num_steps = 100000000;
	double step;
	step = 1. / (double)num_steps;
	start = clock();
#pragma omp parallel for
	for (i = 0; i<num_steps; i++) //Prywatna zmienna i
	{
		x = (i + .5)*step; //Wszystkie inne zmienne publiczne
		sum = sum + 4.0 / (1. + x*x);
	}

	pi = sum*step;
	stop = clock();

	printf("Wartosc liczby PI wynosi %15.12f\n", pi);
	printf("Czas przetwarzania wynosi %f sekund\n", ((double)(stop - start) / 1000.0));
	return 0;
}


int liczPi1() {
	clock_t start, stop;
	double x, pi, sum = 0.0;
	long long num_steps = 100000000;
	double step;
	int i;
	step = 1. / (double)num_steps;
	start = clock();
// Wykonywanie pętli for, gdzie wartość x jest prywatna dla każdego wątku	
#pragma omp parallel for private(x)
	for (i = 0; i<num_steps; i++)
	{
		x = (i + .5)*step;
#pragma omp atomic
		//atomowość modyfikacji
		//Żeby wczytywanie i zapisywanie ostatecznej wartości, było wykonywane tylko przez jeden proces
		sum += +4.0 / (1. + x*x);
	}

	pi = sum*step;
	stop = clock();

	printf("Wartosc liczby PI wynosi %15.12f\n",pi);
	printf("Czas przetwarzania wynosi %f sekund\n", ((double)(stop - start) / 1000.0));
	return 0;
}

int liczPi2() {
	clock_t start, stop;
	double x, pi, sum = 0.0;
	int i;
	long long num_steps = 100000000;
	double step;
	step = 1. / (double)num_steps;
	start = clock();
	omp_set_num_threads(2);
	#pragma omp parallel for reduction(+:sum) private(x)
	for (i = 0; i < num_steps; i++)
	{
		x = (i + .5)*step;
		sum += +4.0 / (1. + x*x);
	}
	pi = sum*step;
	stop = clock();

	printf("Wartosc liczby PI wynosi %15.12f\n", pi);
	printf("Czas przetwarzania wynosi %f sekund\n", ((double)(stop - start) / 1000.0));
	return 0;
}


int liczPi3() {
	clock_t start, stop;
	double x, pi, sum = 0.0;
	int i;
	long long num_steps = 100000000;
	double step;
	step = 1. / (double)num_steps;
	start = clock();
	const int liczbaWatkow = 4;
	omp_set_num_threads(liczbaWatkow);
	volatile double tab[liczbaWatkow] = { 0 };
#pragma omp parallel shared(tab) private(x)
	{
		int threadNumber = omp_get_thread_num();
		#pragma omp for 
		for (i = 0; i < num_steps; i++)
		{
			x = (i + .5)*step;
			tab[threadNumber] += +4.0 / (1. + x*x);
		}
	}
	for (int j = 0; j < liczbaWatkow; j++) {
		sum += tab[j];
	}
	pi = sum*step;
	stop = clock();

	printf("Wartosc liczby PI wynosi %15.12f\n", pi);
	printf("Czas przetwarzania wynosi %f sekund\n", ((double)(stop - start) / 1000.0));
	return 0;
}


int dlugoscLinii() {
	clock_t start, stop;
	double x, pi, sum = 0.0;
	//int i;
	long long num_steps = 100000000;
	double step;
	step = 1. / (double)num_steps;
	
	const int liczbaWatkow = 2;
	omp_set_num_threads(liczbaWatkow);
	const int liczbaPowtorzen = 20;
	double tablicaCzasow[liczbaPowtorzen] = { 0 };
	volatile double tab[liczbaPowtorzen+1] = { 0 };
	int j;
	for (j = 0; j < liczbaPowtorzen; j++) {
		tab[j] = 0;
		tab[j + 1] = 0;
		start = clock();
		#pragma omp parallel shared(tab) private(x) 
		{
			//#pragma omp parallel for private(x)
			for (int i = 0; i < num_steps; i++)
			{
				x = (i + .5)*step;
				tab[j+omp_get_thread_num()] += +4.0 / (1. + x*x);
			}
		}
		pi = tab[j]*step;
		stop = clock();
		tablicaCzasow[j] = ((double)stop - start) / 1000.0;
		printf("%f\n", tablicaCzasow[j]);
		//printf("Wartosc liczby PI wynosi %15.12f\n", pi);
	}
	printf("Wartosc liczby PI wynosi %15.12f\n", pi);
	return 0;
}

int watekDedykowany() {
	return 0;
}

int main() {
	printf("Liczba Pi sekwencyjnie\n");
	liczPiSekwencyjnie();
	printf("\nLiczba Pi rownolegle\n");
	liczPiRownolegle();
	printf("\nPierwsza wersja PI\n");
	liczPi1();
	printf("\nDruga wersja PI\n");
	liczPi2();
	printf("\nTrzecia wersja PI\n");
	liczPi3();
	printf("\nDlugosc linii\n");
	dlugoscLinii();
	//getchar();
}

/*Wyniki
Liczba Pi sekwencyjnie
Wartosc liczby PI wynosi  3.141592653590
Czas przetwarzania wynosi 0.580000 sekund

Liczba Pi rownolegle
Wartosc liczby PI wynosi  0.883524146910
Czas przetwarzania wynosi 0.472000 sekund

Pierwsza wersja PI
Wartosc liczby PI wynosi  3.141592653590
Czas przetwarzania wynosi 11.920000 sekund

Druga wersja PI
Wartosc liczby PI wynosi  3.141592653590
Czas przetwarzania wynosi 0.293000 sekund

Trzecia wersja PI
Wartosc liczby PI wynosi  3.141592653590
Czas przetwarzania wynosi 0.365000 sekund

Dlugosc linii
2.574000
2.568000
2.584000
1.239000 LINIA
2.326000
2.315000
2.352000
2.352000
2.333000
2.324000
2.291000
1.193000 LINIA
2.592000
2.570000
2.566000
2.581000
2.585000
2.578000
2.586000
1.213000 LINIA
Wartosc liczby PI wynosi  3.141592653590

Linia 8 elementów tablicy * 8 bajtów (bo tablica double) = 64 bajty
*/