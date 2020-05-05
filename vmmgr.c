/*
Name: Alex Rigl
ID: 2300146
Assignment: Programming Assignment #6

Worked with:
- Filip Augustkowski

Project Description:
The objective of this assignment consists of writing a C/C++ program that translates logical to physical addresses for a virtual address space of size 216 = 65,536 bytes.
Your program will read from a file containing logical addresses and, using a TLB as well as a page table, will translate each logical address to it's corresponding
physical address and output the value of the byte stored at the translated physical address. The goal behind this project is to simulate the steps involved in translating
logical to physical addresses.

Run with:
./vmmgr addresses.txt

*/
//----VARIABLES AND PACKAGES-----------------------------------------------
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <semaphore.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <stdarg.h>

/* Structs */
typedef struct{
  int pageNo;
  int frameNo;
}PTentry;

typedef struct{
  unsigned int upper: 16;
  unsigned int pagno: 8;
  unsigned int offset: 8;
} BitAddr;

/* Command Line */
int reqArgs = 3;
int bsFile; /* backing store */
FILE *laFile; /* Logical Address File */

/* Memory Manager Specifications */
int entriesTLB = 16;
int numFrames = 256;
int entriesPT = 256;
/* In Bytes */
int pageSize = 256;
int frameSize = 256;
int physMemSize = 65536; /* this is because number of frames is 256, 256 * 256 frame size */

/* Page table */
PTentry *pageTable;

/* Backing store file information */
void *bsPtr;

/* Summary Information */
int pFaults;
float pFaultRate;
int tlbHits;
float tlbHitRate;




//----Method Headers-----------------------------------------------------------
void InitPageTable();
void BeginSim();
BitAddr ConvertLA(unsigned int addy);
void ToBinary(unsigned int num);
void InitPageTable();
int CheckInput(int argc, char** argv);





//----MAIN---------------------------------------------------------------------

int main(int argc, char** argv)
{

    /* From check input, we check input and open files */
    if (!CheckInput(argc, argv)) {
      printf("ERROR: Input incorrect");
      exit(EXIT_FAILURE);
    }

    /* Our next step is to NMAP the backing store file */
    bsPtr = mmap(0, physMemSize * sizeof(unsigned int), PROT_WRITE, MAP_PRIVATE, bsFile, 0); /* maybe MAP_SYNC */

    if (bsPtr == MAP_FAILED) {
  		perror("Map failed\n");
  		exit(EXIT_FAILURE);
  	}

    /* Initialize Page Table */
    InitPageTable();

    /* Initialize Summary Results */
    pFaults = 0;
    pFaultRate = 0;
    tlbHits = 0;
    tlbHitRate = 0;

    /* Begin Sim, which loops through address file */
    BeginSim();

    close(bsFile);
    fclose(laFile);

    return 1;
}






//----HELPER METHODS--------------------------------------------------------
void BeginSim()
{
    char fileLine[16];
    unsigned int newAddy;
    /* Get information from each line and put into tasks */
    while (fgets(fileLine, 16, laFile) != NULL) {
      sscanf(fileLine, "%d", &newAddy); /* scan each line for info */
      printf("The number %d converted to binary is \n", newAddy);
      ToBinary(newAddy);


    }
}

/* Convert logic address and create a bit address from it */
BitAddr ConvertLA(unsigned int addy)
{


}

//https://www.youtube.com/watch?time_continue=274&v=SdQ0VSEiLxY&feature=emb_logo
void ToBinary(unsigned int num)
{
  printf("I got this num in binary %d", num);
  // array to store binary number
  unsigned int binaryNum[32];

  int j;
  // counter for binary array
  int i = 0;
  while (num > 0) {

      // storing remainder in binary array
      binaryNum[i] = num % 2;
      num = num / 2;
      i++;
  }

  // printing binary array in reverse order
  for (j = i - 1; j >= 0; j--)
      printf("THIS: \n", binaryNum[j]);
}

void InitPageTable()
{
    pageTable = malloc(entriesPT * sizeof(PTentry));

    int i;
    for(i = 0; i < entriesPT; i++)
    {
        pageTable[i].pageNo = -1;
        pageTable[i].frameNo = -1;
    }
}


/* Method that checks input from command line */
int CheckInput(int argc, char** argv)
{
    /* Fetch file names and try to open them */
    if (argc == reqArgs) { /*Check number of args */

      bsFile = open(argv[1], O_RDWR);
      laFile = fopen(argv[2], "r");

       /* Check if files are opened */
      if(bsFile == -1) {
        printf("ERROR: Backing store file cannot be opened \n");
        exit(EXIT_FAILURE);
      }
      else if(laFile == NULL) {
        printf("ERROR: Address file file cannot be opened \n");
        exit(EXIT_FAILURE);
      }
      else { /* Input is correct */
       return 1;
     }
    }
    else {
      printf("ERROR: Not enough arguments \n");
      exit(EXIT_FAILURE);
    }
}
