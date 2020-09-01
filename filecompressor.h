#ifndef FILECOMPRESSOR_H
#define FILECOMPRESSOR_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stddef.h>
#include<string.h>
#include<dirent.h>
#include<errno.h>
#include<sys/param.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<libgen.h>

typedef struct avlNode {
	struct avlNode * left;
	struct avlNode * right;
	char * token;
	int freq;
	int height;
	char * code;
} avlNode;

typedef struct bNode {
	char * token;
	int freq;
	struct bNode * left;
	struct bNode * right;
} bNode;

typedef struct minHeap {
	bNode ** arr;
	int size;
} minHeap;

avlNode * huffTree = NULL;

avlNode * createAVLNode(char *, char *);
int max (int, int);
int height (avlNode *);
int getBalance (avlNode *);
avlNode * rightRotate (avlNode *);
avlNode * leftRotate (avlNode *);
avlNode * avlInsert(avlNode *, char *, char *);
void avlPrint(avlNode *);
void printGivenLevel(bNode *, int);
avlNode * tokenizer(char *, avlNode *);
bNode * createBNode(char *, int);
int countNodes(avlNode *);
int avlToArray(avlNode *, bNode **, int);
void arrToMinHeap(bNode **, int, int);
minHeap * createMinHeap(bNode **, int);
bNode * getMin(minHeap *);
bNode * buildHuffmanTree(minHeap *);
int isLeaf(bNode *);
void writeArr(int[], int, int);
void outputCodes (bNode *, int[], int, int);
void printLevelOrder(bNode *);
void printGivenLevel(bNode *, int);
int height2 (bNode *);
char * escapeCode (char);
void listdir(const char *, char, avlNode *);
avlNode * tokenizer(char *, avlNode *);
void compress (avlNode *, char *);
void decompress (avlNode *, char *);

#endif
