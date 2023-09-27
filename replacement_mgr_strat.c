#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "buffer_mgr.h"
#include "replacement_mgr_strat.h"
#include "storage_mgr.h"


void FIFO(BM_BufferPool *const bm, Frame *page, int rearIndex, int maxBufferSize, int noOfPagesWrite)
{
	Frame *pageFrame = (Frame *) bm->mgmtData;
	
	int i, frontIndex;
	frontIndex = rearIndex % maxBufferSize;

	for(i = 0; i < maxBufferSize; i++)
	{
		if(pageFrame[frontIndex].fixCount == 0)
		{
			if(pageFrame[frontIndex].dirtyCount == 1)
			{
				SM_FileHandle *fHandle;
				openPageFile(bm->pageFile, fHandle);
				writeBlock(pageFrame[frontIndex].bm_PageHandle.pageNum, fHandle, pageFrame[frontIndex].smp);
				
				noOfPagesWrite++;
			}
			
			pageFrame[frontIndex].smp = page->smp;
			pageFrame[frontIndex].bm_PageHandle.pageNum = page->bm_PageHandle.pageNum;
			pageFrame[frontIndex].dirtyCount = page->dirtyCount;
			pageFrame[frontIndex].fixCount = page->fixCount;
			break;
		}
		else
		{
			frontIndex++;
            if (frontIndex % maxBufferSize == 0){
                frontIndex = 0;
            }
		}
	}
}


void LRU (BM_PageHandle *const page){

};

// Define a structure to represent a page frame
typedef struct Frame {
    int pageNum;
    int fixCount;
    int dirtyCount;
    bool referenceBit; // Add a reference bit for CLOCK
    // Add other fields as needed
} Frame;

// Initialize your buffer pool and related data structures here

// Function to find the page to replace using CLOCK
int findClockPage(Frame* frames, int numFrames, int* clockHand) {
    while (1) {
        if (!frames[*clockHand].referenceBit) {
            // This page has a reference bit of false and can be replaced
            return *clockHand;
        } else {
            // Give the page a "second chance" by resetting the reference bit
            frames[*clockHand].referenceBit = false;
            *clockHand = (*clockHand + 1) % numFrames;
        }
    }
}

// Function to simulate page replacement using CLOCK
void replacePageClock(Frame* frames, int numFrames, int pageNum, int* clockHand) {
    int emptySlot = -1;

    // Check if the page is already in memory
    for (int i = 0; i < numFrames; i++) {
        if (frames[i].pageNum == pageNum) {
            // Page is already in memory, so just update the fix count and set the reference bit
            frames[i].fixCount++;
            frames[i].referenceBit = true;
            return;
        }
        if (frames[i].fixCount == 0 && emptySlot == -1) {
            // Found an empty slot to place a new page (fix count is zero)
            emptySlot = i;
        }
    }

    // If no empty slot is found, evict a page using the CLOCK algorithm
    if (emptySlot == -1) {
        emptySlot = findClockPage(frames, numFrames, clockHand);
    }

    // Replace the selected page with the new page
    frames[emptySlot].pageNum = pageNum;
    frames[emptySlot].fixCount = 1;
    frames[emptySlot].dirtyCount = 0;
    frames[emptySlot].referenceBit = true; // Set the reference bit to true for the new page
}

// Example usage:
int main() {
    int numFrames = 4; // Number of frames in memory
    Frame frames[4];  // Memory buffer with 4 frames
    int clockHand = 0; // Initialize the CLOCK hand to the beginning of the buffer

    // Initialize frames with page numbers, fix count, dirty count, and reference bit
    for (int i = 0; i < numFrames; i++) {
        frames[i].pageNum = -1; // Initialize as empty
        frames[i].fixCount = 0; // Fix count initially zero
        frames[i].dirtyCount = 0; // Dirty count initially zero
        frames[i].referenceBit = false; // Reference bit initially false for CLOCK
    }

    // Simulate page requests
    int pageRequests[] = {1, 2, 3, 1, 4, 2, 5 ,4 ,3 ,5};
    int numRequests = sizeof(pageRequests) / sizeof(pageRequests[0]);

    for (int i = 0; i < numRequests; i++) {
        int pageNum = pageRequests[i];
        replacePageClock(frames, numFrames, pageNum, &clockHand);
        // You can add code here to handle the loaded page as needed
    }

    // Print the frames currently in memory
    printf("Frames in memory after all requests:\n");
    for (int i = 0; i < numFrames; i++) {
        printf("Frame %d (Page: %d, Fix Count: %d, Dirty Count: %d, Reference Bit: %d)\n",
            i, frames[i].pageNum, frames[i].fixCount, frames[i].dirtyCount, frames[i].referenceBit);
    }

    return 0;
}

void LFU (BM_PageHandle *const page){

};

void LRU_k (BM_PageHandle *const page){

};