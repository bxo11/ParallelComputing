#include "SimpleMinimization.h"

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

const double DR_SHRINK = 0.8;

using namespace std;

SimpleMinimization::SimpleMinimization(Function *f, double timeLimit) : Minimization(f, timeLimit)
{
	int SIZE;

	#pragma omp parallel
	{
		SIZE = omp_get_num_threads();
	}

	seed = new drand48_data[SIZE];
	random_value_holder = new double[SIZE];

	unsigned long a = (unsigned long)time(NULL);

	for (int i = 0; i < SIZE; i++)
	{
		srand48_r(a, &seed[i]);
		a++;
	}
}

SimpleMinimization::~SimpleMinimization()
{
	free(random_value_holder);
	free(seed);
}

void SimpleMinimization::find(double dr_ini, double dr_fin,
							  int idleStepsLimit)
{
	#pragma omp parallel
	{
		int idleSteps = 0; // liczba krokow, ktore nie poprawily lokalizacji
		int id = omp_get_thread_num();

		double v, xnew, ynew, znew, vnew, dr;
		double localX, localY, localZ;
		double localBestX, localBestY, localBestZ, localBestV;

		generateRandomPosition(&localBestX, &localBestY, &localBestZ, id);
		localBestV = function->value(localBestX, localBestY, localBestZ);

		#pragma omp critical
				{
					std::cout << "Init position - " << id << ": " << localBestX << ", " << localBestY << ", " << localBestZ
							  << " value = " << localBestV << std::endl;
				}

		while (hasTimeToContinue())
		{
			// inicjujemy losowo polozenie startowe w obrebie kwadratu o bokach od min do max
			generateRandomPosition(&localX, &localY, &localZ, id);

			v = function->value(localX, localY, localZ); // wartosc funkcji w punkcie startowym

			idleSteps = 0;
			dr = dr_ini;

			while ((dr > dr_fin) && (idleSteps < idleStepsLimit))
			{
				drand48_r(&seed[id], &random_value_holder[id]);
				xnew = localX + (random_value_holder[id] - 0.5) * dr;
				drand48_r(&seed[id], &random_value_holder[id]);
				ynew = localY + (random_value_holder[id] - 0.5) * dr;
				drand48_r(&seed[id], &random_value_holder[id]);
				znew = localZ + (random_value_holder[id] - 0.5) * dr;

				// upewniamy sie, ze nie opuscilismy przestrzeni poszukiwania rozwiazania
				xnew = limitX(xnew);
				ynew = limitY(ynew);
				znew = limitZ(znew);

				// wartosc funkcji w nowym polozeniu
				vnew = function->value(xnew, ynew, znew);

				if (vnew < v)
				{
					localX = xnew; // przenosimy sie do nowej, lepszej lokalizacji
					localY = ynew;
					localZ = znew;
					v = vnew;
					idleSteps = 0; // resetujemy licznik krokow, bez poprawy polozenia
				}
				else
				{
					idleSteps++; // nic sie nie stalo

					if (idleSteps == idleStepsLimit)
					{
						dr *= DR_SHRINK; // zmniejszamy dr
						idleSteps = 0;
					}
				}
			} // dr wciaz za duze

			#pragma omp critical
			{
				x = localX;
				y = localY;
				z = localZ;
				addToHistory();
			}

			if (v < localBestV)
			{ // znalezlismy najlepsze polozenie lokalne
				localBestV = v;
				localBestX = localX;
				localBestY = localY;
				localBestZ = localZ;

				std::cout << "New better position - " << id << ": " << localX << ", " << localY << ", " << localZ
						  << " value = " << v << std::endl;
			}
		}
		// mamy czas na obliczenia

		#pragma omp critical
		{
			if (localBestV < bestV)
			{
				bestV = localBestV;
				bestX = localBestX;
				bestY = localBestY;
				bestZ = localBestZ;
			}
		}
	}
}

void SimpleMinimization::generateRandomPosition(double *pointerX, double *pointerY, double *pointerZ, int id)
{
	drand48_r(&seed[id], &random_value_holder[id]);
	*pointerX = random_value_holder[id] * (maxX - minX) + minX;
	drand48_r(&seed[id], &random_value_holder[id]);
	*pointerY = random_value_holder[id] * (maxY - minY) + minY;
	drand48_r(&seed[id], &random_value_holder[id]);
	*pointerZ = random_value_holder[id] * (maxZ - minZ) + minZ;
}
