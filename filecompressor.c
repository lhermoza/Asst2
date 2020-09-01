#include "fileCompressor.h"

/* AVL Tree implementation */
// creates node for AVL tree
avlNode * createAVLNode(char * token, char * code) {
	avlNode * node = (avlNode *) malloc(sizeof(avlNode));
	node->token = token;
	node->left = NULL;
	node->right = NULL;
	node->code = code;
	node->freq = 1;
	node->height = 1;
	return node;
};

// returns the max of two integers
int max(int a, int b) {
	if (a > b) {
		return a;
	} else {
		return b;
	}
};

// returns height of an avl tree node
int height (avlNode * node) {
	if (node == NULL) { // node does not exist
		return 0;
	}
	return node->height;
};

// returns balance factor of an avl tree node
int getBalance (avlNode * node) {
	if (node == NULL) { // node does not exist
		return 0;
	}
	return height(node->left)-height(node->right);
};

// rotates avl right around node
avlNode * rightRotate (avlNode * node) {
	// performing rotation
	avlNode * nodeL = node->left;
	avlNode * nodeLR = nodeL->right;
	nodeL->right = node;
	node->left = nodeLR;
	//updating heights
	node->height = max(height(node->left), height(node->right))+1;
	nodeL->height = max(height(nodeL->left), height(nodeL->right))+1;
	return nodeL;
};

// rotates avl left around node
avlNode * leftRotate (avlNode * node) {
	// performing rotation
	avlNode * nodeR = node->right;
	avlNode * nodeRL = nodeR->left;
	nodeR->left = node;
	node->right = nodeRL;
	// updating heights
	node->height = max(height(node->left), height(node->right))+1;
	nodeR->height = max(height(nodeR->left), height(nodeR->right))+1;
	return nodeR;
};

avlNode * avlInsert(avlNode * node, char * token, char * code) {
	// recursive bst insertion
	if (node == NULL) {
		return createAVLNode(token, code);
	} else if (strcmp(token, node->token) < 0) {
		node->left = avlInsert(node->left, token, code);
	} else if (strcmp(token, node->token) > 0) {
		node->right = avlInsert(node->right, token, code);
	} else {
		++node->freq; // if token is already in tree, increase frequency
		return node;
	}
	
	// updating height
	node->height = 1+max(height(node->left), height(node->right));
	
	// getting balance factor to check rotation case
	int balance = getBalance(node);
	// right right case
	if (balance > 1 && strcmp(token, node->left->token) < 0) {
		return rightRotate(node);
	}
	// left left case
	if (balance < -1 && strcmp(token, node->right->token) > 0) {
		return leftRotate(node);
	}
	// left right case
	if (balance > 1 && strcmp(token, node->left->token) > 0) {
		node->left = leftRotate(node->left);
		return rightRotate(node);
	}
	// right left case
	if (balance < -1 && strcmp(token, node->right->token) < 0) {
		node->right = rightRotate(node->right);
		return leftRotate(node);
	}

	return node;
};

void avlPrint(avlNode * root) {
	if (root != NULL) {
		printf("%s %d %s\n", root->token, root->freq, root->code);
		avlPrint(root->left);
		avlPrint(root->right);
	}
};

/* min heap implementation */
bNode * createBNode(char * token, int freq) {
	bNode * node = (bNode *) malloc(sizeof(bNode));
	node->token = token;
	node->freq = freq;
	node->left = NULL;
	node->right = NULL;
	return node;
};

int countNodes(avlNode * root) {
	int numNodes = 1;
	if (root == NULL) {
		return 0;
	}
	numNodes = numNodes + countNodes(root->left);
	numNodes = numNodes + countNodes(root->right);
	return numNodes;
};

int avlToArray(avlNode * node, bNode ** arr, int i) {
	if (node == NULL) {
		return i;
	}
	if (node->left != NULL) {
		i = avlToArray(node->left, arr, i);
	}
	arr[i] = createBNode(node->token, node->freq);
	i++;
	if (node->right != NULL) {
		i = avlToArray(node->right, arr, i);
	}
	return i;
};

void arrToMinHeap(bNode ** arr, int n, int i) {
	int p = i;
	int l = 2*i+1;
	int r = 2*i+2;
	
	if (l<n && arr[l]->freq < arr[p]->freq) {
		p = l;
	}
	if (r<n && arr[r]->freq < arr[p]->freq) {
		p = r;
	}
	if (p != i) {
		bNode * temp = arr[i];
		arr[i] = arr[p];
		arr[p] = temp;
		arrToMinHeap(arr, n, p);
	}
};

minHeap * createMinHeap(bNode ** arr, int size) {
	minHeap * heap = (minHeap *) malloc(sizeof(minHeap));
	heap->arr = arr;
	heap->size = size;
	return heap;
};

bNode * getMin(minHeap * heap) {
	bNode * min = heap->arr[0];
	heap->arr[0] = heap->arr[heap->size-1];
	--heap->size;
	int k;
	for (k=(heap->size)/2-1; k>=0; k--) {
		arrToMinHeap(heap->arr, heap->size, k);
	}
	return min;
}

bNode * buildHuffmanTree(minHeap * heap) {
	bNode * internal;
	bNode * left;
	bNode * right;
	
	while (heap->size > 1) {
		left = getMin(heap);
		right = getMin(heap);
		int size = left->freq+right->freq;
		internal = createBNode("INTERNAL!!!", size);
		internal->left = left;
		internal->right = right;
		
		heap->arr[heap->size] = internal;
		++heap->size;
		
		int k;
		for (k=(heap->size)/2-1; k>=0; k--) {
			arrToMinHeap(heap->arr, heap->size, k);
		}
	}

	return getMin(heap);
};

int isLeaf(bNode * root) {
	return !(root->left) && !(root->right);
};

void writeArr(int arr[], int n, int fd) {
	int i;
	for (i=0; i<n; i++) {
		char c = arr[i]+'0';
		char str[2];
		str[0] = c;
		str[1] = '\0';
		write(fd, str, 1);
	}
};

void outputCodes (bNode * root, int arr[], int top, int fd) {
	if (root->left) {
		arr[top] = 0;
		outputCodes(root->left, arr, top+1, fd);
	}
	if (root->right) {
		arr[top] = 1;
		outputCodes(root->right, arr, top+1, fd);
	}
	if (isLeaf(root)) {
		writeArr(arr, top, fd);
		write(fd, "\t", 1);
		write(fd, root->token, strlen(root->token));
		write(fd, "\n", 1);
	}
};

void printLevelOrder(bNode * node) {
	int h = height2(node);
	int i;
	for (i=1; i<=h; i++) {
		printf("level: %d\n", i);
		printGivenLevel(node, i);
	}
};

void printGivenLevel(bNode * root, int level) {
	if (root == NULL) {
		printf("empty\n");
		return;
	}
	if (level == 1) {
		printf("%s %d\n", root->token, root->freq);
	} else if (level > 1) {
		printGivenLevel(root->left, level-1);
		printGivenLevel(root->right, level-1);
	}
};

int height2 (bNode * node) {
	if (node == NULL) {
		return 0;
	} else {
		int lheight = height2(node->left);
		int rheight = height2(node->right);
		if (lheight > rheight) {
			return lheight+1;
		} else {
			return rheight+1;
		}
	}
};

char * escapeCode (char c) {
	if (c == '\n')
		return ("$n");
	if (c == '\t')
		return ("$t");
	if (c == '\v')
		return ("$v");
	if (c == '\a')
		return ("$a");
	if (c == '\b')
		return ("$b");
	if (c == '\f')
		return ("$f");
	if (c == '\r')
		return ("$r");
	if (c == '\'')
		return ("$\'");
	if (c == '\"')
		return ("$\"");
};

char escapeSeq (char * c) {
	if (strcmp(c, "$n") == 0)
		return ('\n');
	if (strcmp(c, "$t") == 0)
		return ('\t');
	if (strcmp(c, "$v") == 0)
		return ('\v');
	if (strcmp(c, "$a") == 0)
		return ('\a');
	if (strcmp(c, "$b") == 0)
		return ('\b');
	if (strcmp(c, "$f") == 0)
		return ('\f');
	if (strcmp(c, "$r") == 0)
		return ('\r');
	if (strcmp(c, "$\'") == 0)
		return ('\'');
	if (strcmp(c, "$\"") == 0)
		return ('\"');
	else
		return 0;
}

void listdir(const char *name, char mode, avlNode * root) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            if (mode == 'b') {
            	listdir(path, 'b', huffTree);
            }
            if (mode == 'c') {
            	listdir(path, 'c', root);
            }
            if (mode == 'd') {
            	listdir(path, 'd', root);
            }
        } else {
        	char hcz[1024];
        	char path[1024];
        	snprintf(path, sizeof(path), "%s/%s", name, entry->d_name); // path to file
        	snprintf(hcz, sizeof(hcz), "%s/%s.hcz", name, entry->d_name); // hcz file name
		
        	if (mode == 'b') {
        		huffTree = tokenizer(path, huffTree);
        	}
        	if (mode == 'c') {
        		printf("%s\n", path);
			compress(root, path);
		}
		if (mode == 'd') {
			printf("%s\n", path);
			decompress(root, path);
		}
        }
    }
    closedir(dir);
};

avlNode * tokenizer(char * filename, avlNode * root) {
	int fd = open(filename, O_RDONLY);
	int size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
		
	char buffer;
	char * input = (char *) malloc(size*sizeof(char));
	int inputIndex = 0;
	int tokLen = 0;
	int i;
	for (i=0; i<size; i++) {
		read(fd, &buffer, sizeof(buffer));
		if (buffer == ' ' || iscntrl(buffer)) {
			if (tokLen > 0) {
				char * tok = (char *) malloc(tokLen*sizeof(char)+1);
				memcpy(tok, input, tokLen);
				tok[tokLen] = '\0';
				root = avlInsert(root, tok, "");
				tokLen = 0;
			}
			char * delim = (char *) malloc(sizeof(char)+2);
			if (buffer == ' ') {
				delim = "$sp";
				root = avlInsert(root, delim, "");
			} else if (i+1 != size) {
				delim = escapeCode(buffer);
				root = avlInsert(root, delim, "");
			}
		} else {
			if (i+1 == size && tokLen > 0) {
				char * tok = (char *) malloc(tokLen*sizeof(char)+1);
				memcpy(tok, input, tokLen);
				tok[tokLen] = '\0';
				root = avlInsert(root, tok, "");
				break;
			}
			input[tokLen] = buffer;
			tokLen++;
		}
	}
	close(fd);
	return root;
};

char * getCode(avlNode * root, char * token) {
	if (root == NULL) {
		return "";
	}
	if (strcmp(root->token, token) == 0)
		return root->code;

	if (strcmp(root->token, token) < 0) 
		return getCode(root->right, token);
	
	return getCode(root->left, token);
};

void getToken(avlNode * root, char * code, char ** token, int * isCode) {
	if (root == NULL)
		return;
	
	if(strcmp(root->code, code) == 0) {
		//printf("%s\n", root->token);
		*token = root->token;
		*isCode = 1;
	}
	
	getToken(root->left, code, token, isCode);
	getToken(root->right, code, token, isCode);
};

avlNode * codebookToTree(char * codebookName) {
	int fd = open(codebookName, O_RDWR, S_IWUSR | S_IRUSR);
	int size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	
	avlNode * tree = NULL;
	
	char esc, buffer;
	read(fd, &esc, sizeof(esc));
	read(fd, &buffer, sizeof(buffer));
	char * input = (char *) malloc(size*sizeof(char));
	char * code = (char *) malloc(size*sizeof(char));
	int inputIndex = 0;
	int codeLen = 0;
	int tokLen = 0;
	int isCode = 1;
	int i;
	
	for (i=sizeof(esc)+sizeof(buffer); i<size; i++) {
		read(fd, &buffer, sizeof(buffer));
		if (buffer == '\t') {
			isCode = 0;
		} else if (buffer == '\n') {
			if (tokLen > 0) {
				char * co = (char *) malloc(codeLen*sizeof(char)+1);
				memcpy(co, code, codeLen);
				co[codeLen] = '\0'; 
				char * tok = (char *) malloc(tokLen*sizeof(char)+1);
				memcpy(tok, input, tokLen);
				tok[tokLen] = '\0';
				tree = avlInsert(tree, tok, co);
				tokLen = 0;
				codeLen = 0;
				isCode = 1;
			}
		} else {
			if (isCode == 1) {
				code[codeLen] = buffer;
				codeLen++;
			} else {
				input[tokLen] = buffer;
				tokLen++;
			}
		}
	}
	close(fd);
	return tree;
};

void compress (avlNode * tree, char * filename) {
	int fd = open(filename, O_RDWR, S_IWUSR | S_IRUSR);
	int size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	
	char * hcz = (char *) malloc(strlen(filename)*sizeof(char)+5);
	memcpy(hcz, filename, strlen(filename));
	hcz[strlen(filename)] = '.';
	hcz[strlen(filename)+1] = 'h';
	hcz[strlen(filename)+2] = 'c';
	hcz[strlen(filename)+3] = 'z';
	hcz[strlen(filename)+4] = '\0';
	
	int hfd = open(hcz, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
	
	char buffer;
	char * input = (char *) malloc(size*sizeof(char));
	int inputIndex = 0;
	int tokLen = 0;
	int i;
	for (i=0; i<size; i++) {
		read(fd, &buffer, sizeof(buffer));
		if (buffer == ' ' || iscntrl(buffer)) {
			if (tokLen > 0) {
				char * tok = (char *) malloc(tokLen*sizeof(char)+1);
				memcpy(tok, input, tokLen);
				tok[tokLen] = '\0';
				char * code = getCode(tree, tok);
				write(hfd, code, strlen(code));
				tokLen = 0;
			}
			char * delim = (char *) malloc(sizeof(char)+2);
			if (buffer == ' ') {
				delim = "$sp";
				char * code = getCode(tree, delim);
				write(hfd, code, strlen(code));
			} else if (i+1 != size) {
				delim = escapeCode(buffer);
				char * code = getCode(tree, delim);
				write(hfd, code, strlen(code));
			}
		} else {
			input[tokLen] = buffer;
			tokLen++;
			if (i+1 == size && tokLen > 0) {
				char * tok = (char *) malloc(tokLen*sizeof(char)+1);
				memcpy(tok, input, tokLen);
				tok[tokLen] = '\0';
				char * code = getCode(tree, tok);
				write(hfd, code, strlen(code));
				break;
			}
		}
	}
	close(hfd);
	close(fd);
};

void decompress (avlNode * tree, char * filename) {
	int fd = open(filename, O_RDWR, S_IWUSR | S_IRUSR);
	int size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	
	char * dFile = (char *) malloc((strlen(filename)-4)*sizeof(char)+1);
	memcpy(dFile, filename, strlen(filename)-4);
	dFile[strlen(dFile)] = '\0';
	
	int nfd = open(dFile, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
	
	char buffer;
	char * input = (char *) malloc(size*sizeof(char));
	int inputIndex = 0;
	int codeLen = 0;
	int i;
	for (i=0; i<size; i++) {
		read(fd, &buffer, sizeof(buffer));
		input[codeLen] = buffer;
		codeLen++;
		char * currCode = (char *) malloc(codeLen*sizeof(char)+1);
		memcpy(currCode, input, codeLen);
		currCode[codeLen] = '\0';
		int a = 0;
		char * tokn;
		getToken(tree, currCode, &tokn, &a);
		if (a == 1) {
			char c = escapeSeq(tokn);
			if (strcmp(tokn, "$sp") == 0) {
				write(nfd, " ", 1);
			} else if (c == 0) {
				write(nfd, tokn, strlen(tokn));
			} else {
				write(nfd, &c, sizeof(c));
			}
			codeLen = 0;
		}
	}
	close(nfd);
	close(fd);
};
	
int main (int argc, char ** argv) {
	if (argc < 3) {
		printf("Error: invalid argument\n");
		return 0;
	}
	avlNode * root = NULL;
	if (strcmp(argv[1], "-b") == 0 || strcmp(argv[2], "-b") == 0) {
		if (strcmp(argv[1], "-R") == 0 || strcmp(argv[2], "-R") == 0) {
			if (argc != 4) {
				printf("Error: invalid argument\n");
				return 0;
			}
			listdir(argv[3], 'b', root);
			root = huffTree;
		} else {
			if (argc != 3) {
				printf("Error: invalid argument\n");
			}
			root = tokenizer(argv[2], root);
		}
		bNode ** heapArray = (bNode **) malloc(countNodes(root) * sizeof(bNode *));
		int a = avlToArray(root, heapArray, 0);
		int k;
		for (k=(countNodes(root)/2-1); k>=0; k--) {
			arrToMinHeap(heapArray, countNodes(root), k);
		}
		minHeap * minHeap = createMinHeap(heapArray, countNodes(root));
		bNode * huffmanRoot = buildHuffmanTree(minHeap);
		int arr[100], c=0;
		
		int cfd = open("./HuffmanCodebook", O_RDWR | O_APPEND | O_CREAT, S_IWUSR | S_IRUSR);
		write(cfd, "$\n", 2);
		outputCodes(huffmanRoot, arr, c, cfd);
		write(cfd, "\n", 1);
		close(cfd);
	} else if (strcmp(argv[1], "-c") == 0 || strcmp(argv[2], "-c") == 0) {
		if (strcmp(argv[1], "-R") == 0 || strcmp(argv[2], "-R") == 0) {
			if (argc != 5) {
				printf("Error: invalid argument\n");
				return 0;
			}
			avlNode * tr = codebookToTree(argv[4]);
			listdir(argv[3], 'c', tr);
		} else {
			if (argc != 4) {
				printf("Error: invalid argument\n");
				return 0;
			}
			avlNode * tr = codebookToTree(argv[3]);
			compress(tr, argv[2]);
		}
	} else if (strcmp(argv[1], "-d") == 0 || strcmp(argv[2], "-d") == 0) {
		if (strcmp(argv[1], "-R") == 0 || strcmp(argv[2], "-R") == 0) {
			if (argc != 5) {
				printf("Error: invalid argument\n");
				return 0;
			}
			avlNode * tr = codebookToTree(argv[4]);
			listdir(argv[3], 'd', tr);
		} else {
			if (argc != 4) {
				printf("Error: invalid argument\n");
				return 0;
			}
			avlNode * tr = codebookToTree(argv[3]);
			decompress(tr, argv[2]);
		}
	}
	return 0;
}
