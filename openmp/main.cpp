#include<sys/times.h>
#include<sys/time.h>
#include<omp.h>
#include<iostream>
#include "Minimization.h"
#include "Function.h"
#include "SimpleTestFunction.h"
#include "Point3D.h"
#include "SimpleMinimization.h"

const double CZAS_PRACY = 10000; // w msec
const bool SHOW_HISTORY = false;

double getMsecTime() {
	struct timeval tf;
	gettimeofday(&tf, NULL);
	return tf.tv_sec * 1000 + tf.tv_usec * 0.001;
}

tms getTimes() {
	struct tms current;
	times(&current);
	return current;
}

void showTimers(const struct tms &stopTMS, const struct tms &startTMS,
		double stop, double start) {
	std::cout << "PMO: User time           = "
			<< 10 * (stopTMS.tms_utime - startTMS.tms_utime) << " msec"
			<< std::endl;
	std::cout << "PMO: User / real time    = "
			<< (stopTMS.tms_utime - startTMS.tms_utime) * 1000.0
					/ (stop - start) << "%" << std::endl;
	std::cout << "PMO: System time         = "
			<< (stopTMS.tms_stime - startTMS.tms_stime) * 10 << " msec"
			<< std::endl;
}

void showHistory(Minimization *m) {
	int size = m->historySize();
	std::vector<Point3D*> history = m->getHistory();
	for (int i = 0; i < size; i++) {
		cout << i << " ---- " << (*history[i]) << endl;
	}
}

void showBestResultFound(Minimization *m) {
	Point3D *bestResult = m->getBestPosition();
	std::cout << "PMO: Best x     = " << bestResult->getX() << std::endl;
	std::cout << "PMO: Best y     = " << bestResult->getY() << std::endl;
	std::cout << "PMO: Best z     = " << bestResult->getZ() << std::endl;
	std::cout << "PMO: Best value = " << m->getBestValueFound() << std::endl;
}

int main(int ac, char **av) {

	Function *function = new SimpleTestFunction();
	Minimization *minimization = new SimpleMinimization(function, CZAS_PRACY);

	struct tms startTMS = getTimes();
	double start = getMsecTime();
	minimization->find(0.1, 0.00001, 1500 );
	double stop = getMsecTime();
	struct tms stopTMS = getTimes();

	showBestResultFound(minimization);
	showTimers(stopTMS, startTMS, stop, start);

	if ( SHOW_HISTORY )
		showHistory(minimization);

	return 0;
}
