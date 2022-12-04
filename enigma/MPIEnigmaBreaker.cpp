
#include "MPIEnigmaBreaker.h"
#include "cmath"
#include "mpi.h"

const uint EXPECTED_MESSAGE_LENGTH = 50;
const uint MESSAGE_LENGTH = 3000;
void incrementTab(uint *tab, int lenght, int max, int increase)
{
	// cout<<"rotors: "<<lenght<<", maxValue: "<<max<<endl;
	tab[lenght - 1] += increase;
	for (int i = lenght - 1; i > 0; i--)
	{
		while (tab[i] > max)
		{
			tab[i - 1]++;
			tab[i] -= max + 1;
		}
		// cout<<tab[i]<<" ";
	}
	// cout<<endl;
}

void dekodowanie(int value, uint *r, int PODSTAWA, int ROTORS)
{
	int stanRotora;
	int x_do_n;
	// cout<<"rotors: "<<ROTORS<<", maxValue: "<<PODSTAWA<<endl;
	for (int i = ROTORS - 1; i >= 0; i--)
	{
		x_do_n = pow(PODSTAWA, i);
		stanRotora = value / x_do_n;
		value -= stanRotora * x_do_n;
		r[i] = stanRotora;
	}
}

int pow(int x, int n)
{
	int iloczyn = 1;
	for (int i = 0; i < n; i++)
		iloczyn *= x;
	return iloczyn;
}

void pp(uint *r, int size)
{
	for (int i = size; i >= 0; i--)
	{
		cout << r[i];
	}
	cout << endl;
}
MPIEnigmaBreaker::MPIEnigmaBreaker(Enigma *enigma, MessageComparator *comparator) : EnigmaBreaker(enigma, comparator)
{
}

MPIEnigmaBreaker::~MPIEnigmaBreaker()
{
	delete[] rotorPositions;
}

int forwardStopSignal(MPI_Request *root_recv_stop, MPI_Request *all_recv_stop, int foundCount)
{
	int flag;
	MPI_Status status;
	if (foundCount == 0)
	{
		MPI_Test(root_recv_stop, &flag, &status);
	}
	else
	{
		MPI_Wait(root_recv_stop, &status);
		flag = 1;
	}
	if (flag)
	{
		printf("Forwarding stop signal from %d\n", status.MPI_SOURCE);
		MPI_Ibarrier(MPI_COMM_WORLD, all_recv_stop);
		MPI_Wait(all_recv_stop, MPI_STATUS_IGNORE);
		// if multiple ranks found something at the same time
		MPI_Reduce(MPI_IN_PLACE, &foundCount, 1, MPI_INT, MPI_SUM, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD);
		for (foundCount--; foundCount > 0; foundCount--)
		{
			MPI_Recv(NULL, 0, MPI_CHAR, MPI_ANY_SOURCE, 999, MPI_COMM_WORLD, &status);
			cout << "Additional stop from: " << status.MPI_SOURCE << endl;
		}
		return 1;
	}
	return 0;
}

void copy2(uint *source, uint sourceLength, uint *destination, uint destinationLength)
{
	uint shift = sourceLength - destinationLength;

	for (uint pos = 0; pos < destinationLength; pos++)
		destination[pos] = source[pos + shift];
}

void MPIEnigmaBreaker::crackMessage()
{
	uint rotorLargestSetting = enigma->getLargestRotorSetting();
	int rank, size;
	int repeats = 0;
	int stopCond = 0;
	MPI_Request sendRequest, root_recv_stop, all_recv_stop;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// MPI_Bcast(&messageLength, 1, MPI_INT, 0, MPI_COMM_WORLD);

	uint *messageToDecode2 = new uint[MESSAGE_LENGTH];
	uint *expectedMessage2 = new uint[EXPECTED_MESSAGE_LENGTH];

	if (rank == MPI_ROOT_PROCESS_RANK)
	{
		for (int i = 0; i < messageLength; i++)
		{
			messageToDecode2[i] = messageToDecode[i];
		}
	}

	MPI_Bcast(messageToDecode2, MESSAGE_LENGTH, MPI_INT, 0, MPI_COMM_WORLD);

	// pp(messageToDecode,EXPECTED_MESSAGE_LENGTH);

	setMessageToDecode(messageToDecode2, MESSAGE_LENGTH);
	copy2(messageToDecode2, MESSAGE_LENGTH, expectedMessage2, EXPECTED_MESSAGE_LENGTH);
	setSampleToFind(expectedMessage2, EXPECTED_MESSAGE_LENGTH);

	// pp(messageToDecode2,EXPECTED_MESSAGE_LENGTH);

	uint *r = new uint[MAX_ROTORS];
	for (int j = 0; j < rotors; j++)
	{
		r[j] = 0;
	}
	r[rotors - 1] = rank;

	if (rank == MPI_ROOT_PROCESS_RANK)
	{
		MPI_Irecv(NULL, 0, MPI_CHAR, MPI_ANY_SOURCE, 999, MPI_COMM_WORLD, &root_recv_stop);
	}
	else
	{
		MPI_Ibarrier(MPI_COMM_WORLD, &all_recv_stop);
	}

	for (int i = rank; i < pow(rotorLargestSetting+1, rotors); i += size)
	{
		dekodowanie(i, r, rotorLargestSetting, rotors);
		// incrementTab(r, rotors, rotorLargestSetting, size);
		// pp(r,rotors);
		repeats++;
		if (solutionFound(r)==true)
		{
			stopCond = 1;
			// sending solution to main thread
			pp(r,rotors);
			MPI_Isend(r, rotors, MPI_INT, MPI_ROOT_PROCESS_RANK, 0, MPI_COMM_WORLD, &sendRequest);
			
		}
		int found = stopCond;
		if (found)
		{
			cout << rank << ": found solution" << endl;
			MPI_Request req;
			MPI_Isend(NULL, 0, MPI_CHAR, MPI_ROOT_PROCESS_RANK, 999, MPI_COMM_WORLD, &req);
			if (rank != MPI_ROOT_PROCESS_RANK)
			{
				MPI_Wait(&all_recv_stop, MPI_STATUS_IGNORE);
				MPI_Reduce(&found, NULL, 1, MPI_INT, MPI_SUM, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD);
				MPI_Wait(&req, MPI_STATUS_IGNORE);
				break;
			}
			MPI_Wait(&req, MPI_STATUS_IGNORE);
		}
		if (rank == MPI_ROOT_PROCESS_RANK)
		{
			if (forwardStopSignal(&root_recv_stop, &all_recv_stop, found))
			{
				break;
			}
		}
		else
		{
			int stop_signal;
			MPI_Test(&all_recv_stop, &stop_signal, MPI_STATUS_IGNORE);
			if (stop_signal)
			{
				MPI_Reduce(&found, NULL, 1, MPI_INT, MPI_SUM, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD);
				cout << rank << ": stopping" << endl;
				break;
			}
		}
	};

	if (rank == MPI_ROOT_PROCESS_RANK)
	{
		MPI_Status status;
		uint *res = new uint[rotors];

		cout << "czekam na wynik" << endl;
		MPI_Recv(res, rotors, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		cout << "Result from: " << status.MPI_SOURCE << endl;
		for (uint rotor = 0; rotor < rotors; rotor++)
		{
			rotorPositions[rotor] = res[rotor];
		}
		enigma->setRotorPositions(rotorPositions);
		uint *result2 = new uint[rotors];
		getResult(result2);
		showUint(rotorPositions, rotors);
	}

	cout << rank << ": " << repeats << " iterations" << endl;

	delete[] r;
}

void setRootResutl()
{
}

bool MPIEnigmaBreaker::solutionFound(uint *rotorSettingsProposal)
{
	for (uint rotor = 0; rotor < rotors; rotor++)
	{
		rotorPositions[rotor] = rotorSettingsProposal[rotor];
	}
	enigma->setRotorPositions(rotorPositions);
	uint *decodedMessage = new uint[messageLength];

	for (uint messagePosition = 0; messagePosition < messageLength; messagePosition++)
	{
		decodedMessage[messagePosition] = enigma->code(messageToDecode[messagePosition]);
	}

	bool result = comparator->messageDecoded(decodedMessage);

	delete[] decodedMessage;

	return result;
}

void MPIEnigmaBreaker::getResult(uint *rotorPositions)
{
	for (uint rotor = 0; rotor < rotors; rotor++)
	{
		rotorPositions[rotor] = this->rotorPositions[rotor];
	}
}
