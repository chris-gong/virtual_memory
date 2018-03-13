#include "my_malloc.h"
#include "my_pthread_t.h"

#define MEM_SIZE (1024 * 1024 * 8)
#define MAX_MEM (1024 * 4)
#define METADATA_SIZE 0xc

/*
STATE YOUR ASSUMPTIONS:
  - left-most bit will be free-bit
  - 12 bits will be used for size
  - max allocation size will be 4kB (page size)
    + requires 12 bits


*/

char PHYS_MEMORY[MEM_SIZE];
int isSet;

void* myallocate(size_t size_req, char *fileName, int line, tcb* src)
{
  //TODO: search for free block: (assume first-fit)
  printf("Size Request: %zu\n",size_req);
  int i = 0;
  unsigned int meta; //2 bytes required to hold all metadata
  unsigned int isFree;
  unsigned int blockSize;
  int total_alloc;

  if(!isSet)
  {
    //fill in metadata
    int totalSize = MEM_SIZE - (sizeof(char) * 3);
    //(PHYS_MEMORY[i] >> 3) &= 0x1;
    totalSize &= 0x7fffff;
    char freeBit = 0x80; //1 means free?
    
    PHYS_MEMORY[i] = freeBit | ((totalSize >> 16) & 0x7f);
    PHYS_MEMORY[i+1] = (totalSize >> 8) & 0xff;
    PHYS_MEMORY[i+2] = totalSize & 0xff;
/*
    PHYS_MEMORY[i] = (freeBit >> ) | (totalSize >> 8);
    PHYS_MEMORY[i+1] = totalSize & 0xff;
*/
    isSet = 1;
  }

  if(src == NULL)
  {
    //is a library thread
    total_alloc = -1;

    if(size_req > MEM_SIZE)
    {
      printf("Reurn NULL 1\n");
      return NULL;
    }
  }
  else
  {
    //is a user thread
    total_alloc = src->alloc + size_req + (sizeof(char) * 3);
  }

  if(total_alloc > MAX_MEM)
  {
    //block attempting to allocate more than max
    printf("Reurn NULL 2\n");
    return NULL;
  }

  while(i+2 < MEM_SIZE)
  {
    //printf("Check index: %d\n",i);
    meta = (PHYS_MEMORY[i] << 16) | (PHYS_MEMORY[i+1] << 8) | (PHYS_MEMORY[i+2]);
    /*
    meta = (PHYS_MEMORY[i] & 0x7f0000);
    meta |= (PHYS_MEMORY[i+1] & 0xff00);
    meta |= (PHYS_MEMORY[i+2] & 0xff);
    */


    printf("META: %d\n",meta);
    isFree = meta & 0x800000;
    blockSize = meta & 0x7fffff/*0x7ff8*/;


    if (i < 30)
    {
       //printf("Block size: %d\ti: %d\tisFree: %d\n",blockSize,i,isFree);
    }

    if(isFree && blockSize >= size_req)
    {
      //valid block found
      printf("Block size currently is: %d\tMEM_SIZE was: %d\n",blockSize,MEM_SIZE);
      //note: 0 out header
      PHYS_MEMORY[i] &= 0x00;
      PHYS_MEMORY[i+1] &= 0x00;
      PHYS_MEMORY[i+2] &= 0x00;

      //for i: get leftmost 4 bits from size_req
      //for i + 1 0 out all except rightmost 8
      size_req &= 0x7fffff;
      printf("Registered request: %zu\n",size_req);
      //10001111 | 11111111 
      //free bit then three blank bits then the size bits
      //so the above the is block is free and has a lot of memory
      PHYS_MEMORY[i] = (size_req >> 16) & 0x7f;//((isFree >> 16) & 0x0) | 
      //printf("%d\n", isFree >> 16);
      PHYS_MEMORY[i+1] = (size_req >> 8) & 0xff;
      PHYS_MEMORY[i+2] = (size_req & 0xff);

      if(src != NULL)
      {
        src->alloc += total_alloc;
      }
      //if there isn't enough data in the new block after splitting, then don't split at all
      if(blockSize - size_req - (sizeof(char) * 3) < 0)
      {
	//printf("?\n");
      }
      else
      {
        int freeBit = 0x800000;
        int remainingSize = blockSize - size_req - (sizeof(char)*3);
	//printf("Remaining Size: %d\n",remainingSize);
        PHYS_MEMORY[i+(sizeof(char) * 3)+size_req] = freeBit | ((remainingSize >> 16) & 0x7f);
        PHYS_MEMORY[i+(sizeof(char) * 3)+size_req+1] = (remainingSize >> 8) & 0xff;
        PHYS_MEMORY[i+(sizeof(char) * 3)+size_req+2] = remainingSize & 0xff;
      }
      /*if(i + (sizeof(char) * 3) + size_req > MAX_MEM)
      {
        return &PHYS_MEMORY[i] + (sizeof(char) * 3);
      }*/

      printf("Inserted request of %zu at %p\n", size_req,&PHYS_MEMORY[i] + (sizeof(char) * 3));

      return &PHYS_MEMORY[i] + (sizeof(char) * 3);
   }
  
    else
    {
      i += (blockSize + (sizeof(char) * 3));

    }

  }

  printf("NO VALID BLOCK\n");
  //if reached, no valid block	
  return NULL;
}


void mydeallocate(void *ptr, char *fileName, int line, tcb *src)
{
  char *location = (char*)ptr - (sizeof(char) * 3); //address of meta block of inputted block
  int searchI = 0; //index of block in array

  printf("Deallocating %p from MEM %p\n", location, PHYS_MEMORY);

  int index = (location - PHYS_MEMORY);
  int meta, isFree, prevBlockSize, blockSize, nextBlockSize, searchBSize, totalBlockSize = 0, firstBlockSize;
  int firstBlockIndex = index;

  if(index < 0 || index >= MAX_MEM)
  {
    //fail case
    //TODO: can we just return here, or do we have to do some error checking?
    printf("Out of bounds error\tIndex: %d\n", index);
    return;
  }
  
  meta = (PHYS_MEMORY[index] << 16) | PHYS_MEMORY[index+1] << 8 | PHYS_MEMORY[index+2];
  /*
  meta = (PHYS_MEMORY[index] & 0x7f0000);
  meta |= (PHYS_MEMORY[index+1] & 0xff00);
  meta |= (PHYS_MEMORY[index+2] & 0xff);
  */
  isFree = meta & 0x800000;
  blockSize = meta & 0x7fffff;

  //checking current block
  if(isFree)
  {
    //TODO: handle double free
    printf("Attempted double free\n");
    return;
  }

  //check first element
  if(location == PHYS_MEMORY)
  {
    firstBlockIndex = index;
  }
  else
  {
    while(location - &PHYS_MEMORY[searchI] != 0 && searchI < MEM_SIZE)
    {
      searchBSize = ((PHYS_MEMORY[searchI] << 16) | (PHYS_MEMORY[searchI+1] << 8) | (PHYS_MEMORY[searchI+2])) & 0x7fffff; //size of the previous block
      searchI += (searchBSize + (sizeof(char) * 3)); //address of current meta block we are on
    }

    //Probably won't happen, but just in case
    if(searchI > MEM_SIZE)
    {
      printf("That thing that shouldn't happen actually happened\n");
      return;
    }

   printf("Removed at %li\n", &PHYS_MEMORY[searchI] - PHYS_MEMORY);

  /*
    PHYS_MEMORY[searchI] &= 0x0;
    PHYS_MEMORY[searchI+1] &= 0x0;
  */

    searchI -= (searchBSize + (sizeof(char) * 3));
    meta = (PHYS_MEMORY[searchI] << 16) | (PHYS_MEMORY[searchI+1] << 8) | (PHYS_MEMORY[searchI+2]);
    /*
    meta = (PHYS_MEMORY[searchI] & 0x7f0000);
    meta |= (PHYS_MEMORY[searchI+1] & 0xff00);
    meta |= (PHYS_MEMORY[searchI+2] & 0xff);
    */
    isFree = meta & 0x800000;
    prevBlockSize = meta & 0x7fffff; //ignore the name, cuz technically this is the new block we're on

    //store current blocksize in total
    totalBlockSize += blockSize;

    //checks previous block
    if(isFree)
    {
      //bzero(&PHYS_MEMORY[searchI], blockSize + (sizeof(char) * 3));
      firstBlockIndex = searchI;
      totalBlockSize += (prevBlockSize + (sizeof(char) * 3));
    }

  }
  //check if last element
  if(index + blockSize + (sizeof(char) * 3) >= MEM_SIZE)
  {
    
  }
  else
  {
    searchI += blockSize + (sizeof(char) * 3);
    meta = (PHYS_MEMORY[searchI] << 16) | (PHYS_MEMORY[searchI+1] << 8) | (PHYS_MEMORY[searchI+2]);
    /*
    meta = (PHYS_MEMORY[searchI] & 0x7f0000);
    meta |= (PHYS_MEMORY[searchI+1] & 0xff00);
    meta |= (PHYS_MEMORY[searchI+2] & 0xff);
    */
    isFree = meta & 0x800000;
    nextBlockSize = meta & 0x7fffff; 
    if(isFree)
    {
      totalBlockSize += nextBlockSize + (sizeof(char) * 3);
    }
  }
  //change the block size of the first block (leftmost block)
  meta = (PHYS_MEMORY[firstBlockIndex] << 16) | PHYS_MEMORY[firstBlockIndex+1] << 8 | PHYS_MEMORY[firstBlockIndex+2];
  /*
  meta = (PHYS_MEMORY[firstBlockIndex] & 0x7f0000);
  meta |= (PHYS_MEMORY[firstBlockIndex+1] & 0xff00);
  meta |= (PHYS_MEMORY[firstBlockIndex+2] & 0xff);
  */
  isFree = meta & 0x800000;
  firstBlockSize = meta & 0x7fffff; 
  totalBlockSize += firstBlockSize;
  
  //Right check
  int right = index + blockSize + (sizeof(char) * 3);
  meta = (PHYS_MEMORY[right] << 16) | PHYS_MEMORY[right+1] << 8 | PHYS_MEMORY[right+2];
  /*
  meta = (PHYS_MEMORY[right] & 0x7f0000);
  meta |= (PHYS_MEMORY[right+1] & 0xff00);
  meta |= (PHYS_MEMORY[right+2] & 0xff);
  */
  isFree = meta & 0x800000;
  
  if (isFree)
  {
    totalBlockSize += meta & 0x7fffff;
  }


  PHYS_MEMORY[firstBlockIndex] = 0x80 | ((totalBlockSize >> 16) & 0x7f);
  PHYS_MEMORY[firstBlockIndex+1] = (totalBlockSize >> 8) & 0xff;
  PHYS_MEMORY[firstBlockIndex+2] = (totalBlockSize & 0xff);


  //TODO: maybe, possibly, probably not zero out block
  
  if(src != NULL)
  {
    src->alloc -= blockSize;
  }
  printf("Removed at %li\n", &PHYS_MEMORY[searchI] - PHYS_MEMORY);
  printf("Success!\n");
}
