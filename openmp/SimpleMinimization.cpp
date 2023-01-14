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
	struct drand48_data lBuffer;
	int lRseed = 0;
	double randValue = 0;
	srand48_r (lRseed, &lBuffer);

	// generateRandomPosition(&bestX, &bestY, &bestZ, lBuffer, lRseed, randValue);
	// bestV = function->value(bestX, bestY, bestZ);
	bestV = 0;
}

SimpleMinimization::~SimpleMinimization()
{
}

void SimpleMinimization::find(double dr_ini, double dr_fin,
							  int idleStepsLimit)
{
	#pragma omp parallel private(rseed, buffer)
	{
		int idleSteps = 0; // liczba krokow, ktore nie poprawily lokalizacji
		int id = omp_get_thread_num();
		int rseed = omp_get_thread_num();
		double randValue = 0;
		srand48_r (rseed, &buffer);
		double v, xnew, ynew, znew, vnew, dr;
		double localX, localY, localZ;
		double localBestX, localBestY, localBestZ, localBestV =0;

		// generateRandomPosition(&localBestX, &localBestY, &localBestZ, buffer, rseed, randValue);
		// localBestV = function->value(localBestX, localBestY, localBestZ);
		// #pragma omp critical
		// 	{
		// functionCalls++;
		// 	}

		// #pragma omp critical
		// 		{
		// 			std::cout << "Init position - " << id << ": " << localBestX << ", " << localBestY << ", " << localBestZ
		// 					  << " value = " << localBestV << std::endl;
		// 		}

		while (hasTimeToContinue())
		{
			// inicjujemy losowo polozenie startowe w obrebie kwadratu o bokach od min do max
			generateRandomPosition(&localX, &localY, &localZ, buffer, rseed, randValue);

			v = function->value(localX, localY, localZ); // wartosc funkcji w punkcie startowym

			idleSteps = 0;
			dr = dr_ini;

			while ((dr > dr_fin) && (idleSteps < idleStepsLimit))
			{
				drand48_r(&buffer, &randValue);
				xnew = localX + (randValue - 0.5) * dr;
				drand48_r(&buffer, &randValue);
				ynew = localY + (randValue - 0.5) * dr;
				drand48_r(&buffer, &randValue);
				znew = localZ + (randValue - 0.5) * dr;

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

void SimpleMinimization::generateRandomPosition(double *pointerX, double *pointerY, double *pointerZ, drand48_data buffer, int rseed, double randValue)
{
	drand48_r(&buffer, &randValue);
	*pointerX = randValue * (maxX - minX) + minX;
	drand48_r(&buffer, &randValue);
	*pointerY = randValue * (maxY - minY) + minY;
	drand48_r(&buffer, &randValue);
	*pointerZ = randValue * (maxZ - minZ) + minZ;
}
