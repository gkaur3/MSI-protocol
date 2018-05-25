// MSI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "iostream"
#include "stdio.h"

/* states--> 1= invalid
2= shared
3=modified */

using namespace std;
enum states {
	invalid = 1,
	shared = 2,
	modified = 3,
};
states stateofProcessors[4] = { invalid, invalid, invalid, invalid };


int cachesOfProcessors[4][4];
int memoryArray[4] = { 10,11,12,13 };
int BusValues[4];

bool placeInvalidateOnBus()
{
	for (int i = 0; i < 4; i++)
	{
		if (stateofProcessors[i] == shared)
		{
			stateofProcessors[i] = invalid;
		}
		else if (stateofProcessors[i] == modified)
		{
			memcpy(BusValues, cachesOfProcessors[i], 4 * sizeof(int));
			memcpy(memoryArray, BusValues, 4 * sizeof(int));
			stateofProcessors[i] = invalid;
			return true;
		}
	}
	return false;
}

void readMissOnBus(int memLoc)
{
	/*for invalid, no case*/

	/*with Read Miss request
	if block in shared state read from read block from bus or memory

	if block in modified state, write back block to memory from present cache and requested processor will access block from memory or from bus
	*/
	for (int i = 0; i < 4; i++)
	{
		if (stateofProcessors[i] == invalid)
		{
			continue;
		}
		else if (stateofProcessors[i] == shared)
		{
			//return cachesOfProcessors[i][memLoc]; it will return one mem location
			memcpy(BusValues, cachesOfProcessors[i], 4 * sizeof(int));
			return;
		}
		else if (stateofProcessors[i] == modified)
		{
			memcpy(BusValues, cachesOfProcessors[i], 4 * sizeof(int));
			memcpy(memoryArray, BusValues, 4 * sizeof(int));
			stateofProcessors[i] = shared;
			return;
		}
	}
	memcpy(BusValues, memoryArray, 4 * sizeof(int));

	/*with Write Miss request
	if block in shared state, invalidate it (requested processor will read it from bus)

	if block in exclusive state, writeback block to memory and make it invalid, requested processor will read it from memory
	*/
}
void readByProcessor(int processorNumber , int locationToRead)
{
	if (stateofProcessors[processorNumber] == invalid)
	{

		//cachesOfProcessors[processorNumber][locationToRead] = 
		readMissOnBus(locationToRead);
		memcpy(cachesOfProcessors[processorNumber], BusValues, 4 * sizeof(int));
		stateofProcessors[processorNumber] = shared;
	}

	else if (stateofProcessors[processorNumber] == shared)
	{

	}


	/* Read Miss
	for processor block itself in invalid state , becomes shared (after placing read miss on bus)
		 check if other in modified state
	for processor block itself in modified state, becomes shared (write back, but this case is not considered as we are dealing with just one block)
	*/

}

void writeByProcessor(int valueToWrite, int processorNumber, int cacheLocation)
{
	/* write Miss
	for processor itself in invalid state check if other has modified access to block (after placing write miss on bus)
	*/

	if (stateofProcessors[processorNumber] == invalid)
	{
		readMissOnBus(cacheLocation);
		memcpy(cachesOfProcessors[processorNumber], BusValues, 4 * sizeof(int));
		cachesOfProcessors[processorNumber][cacheLocation] = valueToWrite;
		placeInvalidateOnBus();
		stateofProcessors[processorNumber] = modified;
	}

	else if (stateofProcessors[processorNumber] == shared)
	{
		if (placeInvalidateOnBus())
		{
			memcpy(memoryArray, BusValues, 4 * sizeof(int));
		}
		cachesOfProcessors[processorNumber][cacheLocation] = valueToWrite;
		placeInvalidateOnBus();
		stateofProcessors[processorNumber] = modified;
	}
	
	else if (stateofProcessors[processorNumber] == modified)
	{
		cachesOfProcessors[processorNumber][cacheLocation] = valueToWrite;
	}

}


void Print()
{
	for (int i = 0; i < 4; i++)
	{
		printf("Processor[%d]: Status[%d] Values[%d, %d, %d, %d]\n", i, stateofProcessors[i], cachesOfProcessors[i][0], cachesOfProcessors[i][1], cachesOfProcessors[i][2], cachesOfProcessors[i][3]);

	}
	printf("memory contents are:");
	for (int i = 0; i < 4; i++)
	{
		printf(" %d ", memoryArray[i]);
	}
	printf("\n\n");
}

int main()
{
	readByProcessor(2, 3);
	Print();

	writeByProcessor(10, 2, 1);
	Print();

	writeByProcessor(20, 0, 3);
	Print();

	readByProcessor(0, 3);
	Print();

	readByProcessor(3, 2);
	Print();

	return 0;
}

