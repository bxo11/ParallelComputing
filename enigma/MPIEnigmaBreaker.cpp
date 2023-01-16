#include "MPIEnigmaBreaker.h"
#include "mpi.h"

MPIEnigmaBreaker::MPIEnigmaBreaker(Enigma *enigma, MessageComparator *comparator) : EnigmaBreaker(enigma, comparator)
{
}

MPIEnigmaBreaker::~MPIEnigmaBreaker()
{
	delete[] rotorPositions;
}

void MPIEnigmaBreaker::crackMessage()
{
	uint rotorLargestSetting = enigma->getLargestRotorSetting();
	int rank, size;
	int found = 0;
	int received = 0;
	MPI_Request request, foundRequest;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Bcast(&expectedLength, 1, MPI_UNSIGNED, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD); // expected lenght
	MPI_Bcast(&messageLength, 1, MPI_UNSIGNED, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD);
	if (rank == MPI_ROOT_PROCESS_RANK)
	{
		MPI_Bcast(expectedBuffer, expectedLength, MPI_UNSIGNED, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD); // expected msg
		MPI_Bcast(messageToDecode, messageLength, MPI_UNSIGNED, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD);
	}

	if (rank != MPI_ROOT_PROCESS_RANK)
	{
		expectedBuffer = new uint[expectedLength];
		messageToDecode = new uint[messageLength];
		MPI_Bcast(expectedBuffer, expectedLength, MPI_UNSIGNED, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD);

		comparator->setExpectedFragment(expectedBuffer, expectedLength);
		
		MPI_Bcast(messageToDecode, messageLength, MPI_UNSIGNED, MPI_ROOT_PROCESS_RANK, MPI_COMM_WORLD);
		setMessageToDecode(messageToDecode, messageLength);
	}

	// teraz zastanowic sie nad swoim zakresem prac po rotorsetting i ilosci procesow
	// rank;
	// rotorLargestSetting

	int myFirstRotorSetting = 0;
	int myMaxRotorSetting = 0;

	// WYLICZANIE CZESCI DLA DANEGO PROCESU

	int proportions[size]; // dla kazdego procesu, ile przyjmie , pod jego index

	int forAll = rotorLargestSetting / size;

	for (int i = 0; i < size; i++)
	{
		proportions[i] = forAll;
	}

	int remainder = rotorLargestSetting % size;

	// tu beda zmiany dla rownej pracy
	while (remainder > 0)
	{
		for (int i = 0; i < size; i++)
		{
			if (remainder <= 0)
			{
				break;
			}
			proportions[i] += 1;
			remainder--;
		}
	}

	int beg = 0;
	int end = -1; // poczatek i koniec przedzialu krecenia pierwszym rotorem

	for (int i = 0; i < size; i++)
	{

		end += proportions[i]; // -1 cause index
		if (rank == i)
		{

			myFirstRotorSetting = beg;
			myMaxRotorSetting = end;

			if (rank == size - 1)
			{ // bo rotory leca od 0 - max, a nie MAX-1
				myMaxRotorSetting += 1;
			}

			break;
			// return rotorSettingsToCheck;
		}
		beg += proportions[i]; // if it was 0-17, then beg is 18 index
	}

	// KONIEC WYLICZANIA CZESCI DLA DANEGO PROCESU

	// cout <<"rank: " << rank <<" size: "<< size <<" first: "<< myFirstRotorSetting <<" max: " << myMaxRotorSetting <<endl;

	uint *rMax = new uint[MAX_ROTORS];
	for (uint rotor = 0; rotor < MAX_ROTORS; rotor++)
	{
		if (rotor < rotors)
			rMax[rotor] = rotorLargestSetting;
		else
			rMax[rotor] = 0;
	}

	uint *r = new uint[MAX_ROTORS];

	int ready = 0;									// used for root
	uint *rotorsPositionMessage = new uint[rotors]; // TO JEST W SOLUTION FOUND, pozmieniac MIEJSCE, PRZESYL CZY COS

	if (rank == MPI_ROOT_PROCESS_RANK)
	{
		MPI_Irecv(rotorsPositionMessage, rotors, MPI_UNSIGNED, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &request);
	}

	if (rank != MPI_ROOT_PROCESS_RANK)
	{
		MPI_Irecv(&found, 1, MPI_UNSIGNED, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &foundRequest);
	}

	for (r[0] = myFirstRotorSetting; r[0] <= myMaxRotorSetting; r[0]++)
	{

		for (r[1] = 0; r[1] <= rMax[1]; r[1]++)
		{

			for (r[2] = 0; r[2] <= rMax[2]; r[2]++)
			{

				for (r[3] = 0; r[3] <= rMax[3]; r[3]++)
				{

					for (r[4] = 0; r[4] <= rMax[4]; r[4]++)
					{

						for (r[5] = 0; r[5] <= rMax[5]; r[5]++)
						{

							for (r[6] = 0; r[6] <= rMax[6]; r[6]++)
							{
								
								for (r[7] = 0; r[7] <= rMax[7]; r[7]++)
								{
									
									for (r[8] = 0; r[8] <= rMax[8]; r[8]++)
									{

										for (r[9] = 0; r[9] <= rMax[9]; r[9]++)
										{

											if (rank == MPI_ROOT_PROCESS_RANK)
											{
												if (solutionFound(r))
												{
													cout << rank << " found" << endl;
													for (int i = 1; i < size; i++)
													{
														MPI_Isend(&found, 1, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &foundRequest);
													}
													goto EXIT_ALL_LOOPS;
												}
												MPI_Test(&request, &ready, MPI_STATUS_IGNORE);
												if (ready)
												{
													for (int i = 0; i < rotors; i++)
													{
														rotorPositions[i] = rotorsPositionMessage[i];
													}
													for (int i = 1; i < size; i++)
													{
														MPI_Isend(&found, 1, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &foundRequest);
													}

													goto EXIT_ALL_LOOPS;
												}
											}

											if (rank != MPI_ROOT_PROCESS_RANK)
											{
												MPI_Test(&foundRequest, &received, MPI_STATUS_IGNORE);
												if (received)
												{
													// cout << rank << "rec fin";
													goto EXIT_ALL_LOOPS;
												}

												if (solutionFound(r))
												{
													goto EXIT_ALL_LOOPS;
												}
											}

										} // 9
									}	  // 8
								}		  // 7
							}			  // 6
						}				  // 5
					}					  // 4
				}						  // 3
			}							  // 2
		}								  // 1
	}									  // 0

EXIT_ALL_LOOPS:
	delete[] rMax;
	delete[] r;
}

// TO PRZEROBIC

bool MPIEnigmaBreaker::solutionFound(uint *rotorSettingsProposal)
{
	for (uint rotor = 0; rotor < rotors; rotor++)
		rotorPositions[rotor] = rotorSettingsProposal[rotor];

	enigma->setRotorPositions(rotorPositions);
	uint *decodedMessage = new uint[messageLength];

	for (uint messagePosition = 0; messagePosition < messageLength; messagePosition++)
	{
		decodedMessage[messagePosition] = enigma->code(messageToDecode[messagePosition]);
	}

	bool result = comparator->messageDecoded(decodedMessage);
	if (result)
	{ // found, if not in root, then send it
		int rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		uint *rotorsPositionMessage = new uint[rotors];
		if (rank != MPI_ROOT_PROCESS_RANK)
		{
			for (int i = 0; i < rotors; i++)
			{
				rotorsPositionMessage[i] = rotorPositions[i];
			}
			MPI_Send(rotorsPositionMessage, rotors, MPI_UNSIGNED, MPI_ROOT_PROCESS_RANK, 0, MPI_COMM_WORLD);
		}
	}

	delete[] decodedMessage;

	return result;
}

void MPIEnigmaBreaker::setSampleToFind(uint *expected, uint expectedLength)
{
	MPIEnigmaBreaker::expectedLength = expectedLength;

	MPIEnigmaBreaker::expectedBuffer = new uint[expectedLength];

	for (uint i = 0; i < expectedLength; i++)
	{
		expectedBuffer[i] = expected[i];
	}

	comparator->setExpectedFragment(expected, expectedLength);
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
