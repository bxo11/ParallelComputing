#include "MPIEnigmaBreaker.h"
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

int pow(int x, int n)
{
	int iloczyn = 1;
	for (int i = 0; i < n; i++)
		iloczyn *= x;
	return iloczyn;
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

void MPIEnigmaBreaker::crackMessage()
{
	uint rotorLargestSetting = enigma->getLargestRotorSetting();
	int rank, size;
	int found = 0;
	MPI_Request request, all_recv_stop;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Bcast(&expectedLength, 1, MPI_UNSIGNED, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD); // expected lenght
	MPI_Bcast(&messageLength, 1, MPI_UNSIGNED, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD);
	if (rank == MPI_ROOT_PROCESS_RANK)
	{
		MPI_Bcast(expectedBuffer, expectedLength, MPI_UNSIGNED, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD); // expected msg
		MPI_Bcast(messageToDecode, messageLength, MPI_UNSIGNED, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD);
	}
	else
	{
		expectedBuffer = new uint[expectedLength];
		messageToDecode = new uint[messageLength];
		MPI_Bcast(expectedBuffer, expectedLength, MPI_UNSIGNED, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD);

		comparator->setExpectedFragment(expectedBuffer, expectedLength);

		MPI_Bcast(messageToDecode, messageLength, MPI_UNSIGNED, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD);
		setMessageToDecode(messageToDecode, messageLength);
	}

	uint *rMax = new uint[MAX_ROTORS];
	for (uint rotor = 0; rotor < MAX_ROTORS; rotor++)
	{
		if (rotor < rotors)
			rMax[rotor] = rotorLargestSetting;
		else
			rMax[rotor] = 0;
	}
	uint *r = new uint[MAX_ROTORS];

	int *chunks = new int[size];
	for (int i = 0; i < size; i++)
	{
		chunks[i] = rotorLargestSetting / size;
	}

	int reszta = rotorLargestSetting % size;
	for (int i = 0; i < reszta; i++)
	{
		chunks[i]++;
	}

	int beg = 0;
	int end = -1;
	int myFirstRotorSetting = 0;
	int myMaxRotorSetting = 0;
	for (int i = 0; i < size; i++)
	{
		if (rank == i)
		{
			myFirstRotorSetting = beg;
			myMaxRotorSetting = end + chunks[i] + (rank == size - 1 ? 1 : 0);
			break;
		}
		beg += chunks[i];
		end += chunks[i];
	}

	uint *rotorsPositionMessage = new uint[rotors];

	if (rank == MPI_ROOT_PROCESS_RANK)
	{
		MPI_Irecv(rotorsPositionMessage, rotors, MPI_UNSIGNED, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &request);
	}
	else
	{
		MPI_Irecv(&found, 1, MPI_UNSIGNED, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &all_recv_stop);
	}

	int flag1 = 0;
	int flag2 = 0;
	for (r[0] = myFirstRotorSetting; r[0] <= myMaxRotorSetting; r[0]++)
		for (r[1] = 0; r[1] <= rMax[1]; r[1]++)
			for (r[2] = 0; r[2] <= rMax[2]; r[2]++)
				for (r[3] = 0; r[3] <= rMax[3]; r[3]++)
					for (r[4] = 0; r[4] <= rMax[4]; r[4]++)
						for (r[5] = 0; r[5] <= rMax[5]; r[5]++)
							for (r[6] = 0; r[6] <= rMax[6]; r[6]++)
								for (r[7] = 0; r[7] <= rMax[7]; r[7]++)
									for (r[8] = 0; r[8] <= rMax[8]; r[8]++)
										for (r[9] = 0; r[9] <= rMax[9]; r[9]++)
										{
											if (rank != MPI_ROOT_PROCESS_RANK)
											{
												MPI_Test(&all_recv_stop, &flag1, MPI_STATUS_IGNORE);
												if (flag1 || solutionFound(r))
													goto EXIT_ALL_LOOPS;
											}
											else
											{
												MPI_Test(&request, &flag2, MPI_STATUS_IGNORE);
												if (solutionFound(r) || flag2)
												{
													for (int i = 0; i < rotors; i++)
														rotorPositions[i] = rotorsPositionMessage[i];

													for (int i = 1; i < size; i++)
													{
														MPI_Isend(&found, 1, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &all_recv_stop);
													}
													goto EXIT_ALL_LOOPS;
												}
											}
										}

EXIT_ALL_LOOPS:
	delete[] rMax;
	delete[] r;
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

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (result && rank != MPI_ROOT_PROCESS_RANK)
	{
		uint *messageByRotors = new uint[rotors];
		for (int i = 0; i < rotors; i++)
		{
			messageByRotors[i] = rotorPositions[i];
		}
		MPI_Send(messageByRotors, rotors, MPI_UNSIGNED, MPI_ROOT_PROCESS_RANK, 0, MPI_COMM_WORLD);
		delete[] messageByRotors;
	}
	delete[] decodedMessage;

	return result;
}

void MPIEnigmaBreaker::setSampleToFind(uint *expected, uint expectedLength)
{
	MPIEnigmaBreaker::expectedLength = expectedLength;
	MPIEnigmaBreaker::expectedBuffer = new uint[expectedLength];

	for (int i = 0; i < MPIEnigmaBreaker::expectedLength; i++)
	{
		expectedBuffer[i] = expected[i];
	}

	comparator->setExpectedFragment(expected, MPIEnigmaBreaker::expectedLength);
}

void MPIEnigmaBreaker::setMessageToDecode(uint *message, uint messageLength)
{
	comparator->setMessageLength(messageLength);
	this->messageLength = messageLength;
	this->messageToDecode = message;
	messageProposal = new uint[messageLength];
}

void MPIEnigmaBreaker::getResult(uint *rotorPositions)
{
	for (uint rotor = 0; rotor < rotors; rotor++)
	{
		rotorPositions[rotor] = this->rotorPositions[rotor];
	}
}
