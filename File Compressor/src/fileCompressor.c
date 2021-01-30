/*
 ============================================================================
 Name        : c.c
 Author      : Shaan Kalola
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<ctype.h>
//#include<sys/sysmacros.h>
//for opening directories with opendir()
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<libgen.h>

//for opening files w open()
#include <unistd.h>
#include<fcntl.h>

typedef struct _Stack
{
  char ** arr;
  int head;
  int size;
}Stack;

Stack * makeStack(int size){
  Stack * st = (Stack *) malloc(sizeof(Stack));
  st->arr = (char **) malloc(sizeof(char *) * size);
  int i = 0;
  while (i < size){
    st->arr[i] = (char *) malloc(sizeof(char) * 1024);
    i++;
  }
  st->head = -1;
  st->size = size;
  return st;
}

int stFull(Stack * st){
  if (st->head == (st->size - 1)){
    //is full, return 1
    return 1;
  }
  return 0;
}
int stEmpty(Stack * st){
  if (st->head == -1){
    return 1;
  }
  return 0;
}
void push(Stack * st, char * newName){
  printf("trying to push %s\n", newName);
  if (stFull(st) == 1)
    return;
  st->head++;
  strcpy(st->arr[st->head], newName);
  printf("just pushed %s\n", st->arr[st->head]);
}
char * pop (Stack * st){
  if (stEmpty(st) == 1)
    return NULL;
  char * name = st->arr[st->head - 1];
  st->head--;
  return name;
}

typedef struct _Node{
  struct _Node *left;
  struct _Node *right;
  char * str;
  int count;
  int height;
  char * bytestring;
}Node; //We store the number of times the word appears in count, and the string itself in str

char * idControlCode(char);
char * inverseIdControlCode(char *);
void bFlag(char *);
void cFlag(char *, char *);
void dFlag(char *, char *);
void cListAll(Node *, char*);
//void dListAll(char *, char*);
void compress(Node *, char *);

  /*
typedef struct _Node{
	struct _Node *left;
	struct _Node *right;
	char * str;
	int count;
	int height;
	char * bytestring;
}Node; //We store the number of times the word appears in count, and the string itself in str
*/

typedef struct _Heap{ //I haven't used this, not sure
	Node * array;
	int size;
}minHeap;

int Height (Node *  ptr){ //Calculate height of the node, used for the AVL rotation
	int left = 0;
	int right = 0;
	if(ptr == NULL){
		return 0;
	}
	if(ptr -> left != NULL){
		left = ptr->left->height;
	}
	if(ptr->right != NULL){
		right = ptr->right->height;
	}
	if(left < right){
		return right +1;
	}
	else{
		return left + 1;
	}
}

int Balance (Node * ptr){ // Calculate Balance for node
	return Height(ptr->right)-Height(ptr->left);

}

Node * leftRotation (Node * ptr){ //Rotates Left AVL
	Node * newRoot = ptr->right;
	Node * ptrRight = newRoot->left;
	ptr -> right = ptrRight;
	newRoot -> left =ptr;
	newRoot -> height = Height(newRoot);
	ptr->height = Height(ptr);
	return newRoot;
}

Node * rightRotation (Node * ptr){ // Rotates Right AVL
	Node * newRoot = ptr -> left;
	Node * ptrLeft = newRoot->right;
	ptr -> left = ptrLeft;
	newRoot -> right = ptr;
	newRoot -> height = Height(newRoot);
	ptr-> height = Height(ptr);
	return newRoot;

}

Node * insert (Node * ptr, Node * newInsert){ //AVL insertion, returns the root of the subtree, as a way to avoid storing the parent of AVL
  //  printf("new insert: %s\n", newInsert->str);
	if(ptr == NULL){
	    newInsert -> count =newInsert -> count + 1;
		  return newInsert;
	}
	else if(newInsert == NULL){
		return ptr;
	}

	else if (strcmp(ptr->str, newInsert->str) == 0){
		ptr->count = ptr->count + 1;
		//	printf("new insert: %s\n", newInsert->str);
		free(newInsert);
		return ptr;
	}
	else if (strcmp(newInsert->str,ptr->str) >0){
		ptr->right = insert(ptr->right,newInsert);
	}
	else if (strcmp(newInsert->str,ptr->str) < 0){
		ptr->left = insert(ptr->left,newInsert);
	}

	ptr->height = Height(ptr); //updates the height of the node

	int balance = Balance(ptr); // checks what rotation to do

	if(-1 <= balance && balance <= 1){
		return ptr;
	}

	else if(balance > 1){
	    char * temp1 = newInsert->str;
	    char * temp2 = ptr -> right ->str;
		if(strcmp(newInsert->str,ptr -> right -> str) >=1){
			return leftRotation(ptr);
		}
		else{
			ptr->right = rightRotation(ptr->right);
			return leftRotation(ptr);
		}
	}
	else if (balance < -1){
		if(strcmp(newInsert->str,ptr -> left ->str) <= -1){
			return rightRotation(ptr);
		}
		else{
			ptr->left = leftRotation(ptr->left);
			return rightRotation(ptr);
		}
	}

	return ptr;

}
void makeArray(Node ** array, int * index, Node * ptr){ //makes a heap with an array of Node pointers
	if (ptr == NULL){
		return;
	}
/*	array[index] = ptr;
	makeArray(array, 2*index + 1, ptr->left);
	makeArray(array, 2*index + 2, ptr->right);
*/
	makeArray(array, index, ptr->left);
        array[*index] = ptr;
        *index = *index + 1;
        makeArray(array, index,ptr->right);

}

void shiftDown (Node ** array, int index, int size){
  int left = (index*2)+1;
  int right = (index*2)+2;
	int smallest = index;
	if(left <= size && array[index]->count > array[left]->count){
		smallest = left;
	}
	if (right <= size && array[smallest]->count > array[right]->count){
		smallest = right;
	}

	if(smallest != index){
		Node *const small = array[smallest];
		Node *const  toSwitch = array[index];
		array[index] = small;
		array[smallest] = toSwitch;
		shiftDown(array,smallest,size);
	}

}

void shiftUp (Node ** array, int index, int size){
	double i = (index-1)/2;
	int parent =(int) (i);
	Node * parentNode = array[parent];
	Node * ptr = array[index];
	if(index !=0 && parentNode->count >  ptr ->count){
		array[parent] =ptr;
		array[index] =parentNode;
		shiftUp(array,parent,size);
	}



}

Node * extractMin (Node ** array, int * size){ //gets the min from the heap and removes it
	if(*size == 0){
		printf("Error, Empty Heap");
		return NULL;
	}
	Node * min = array[0];
	array[0] = array[*size -1];
	*size = *size - 1;
	shiftDown(array,0,*size);
	return min;

}

Node * insertHeap (Node ** array, int * size, Node * ptr){
	*size = *size + 1;
	array[*size-1] = ptr;
	shiftUp(array,*size - 1,*size);
}

void buildHeap (Node ** array, int size){
	double i = ((size/2) - 1);
	int index = (int)i;
	while (index >= 0){
		shiftDown(array,index,size);
		index= index-1;
	}
}
void makeHuff (Node ** array, int * size){ //This will make the huffman tree and store in array[0]
	while (*size > 1){
		Node * min1 = extractMin(array,size);
		Node * min2 = extractMin(array,size);
		Node * newNode =(Node*) malloc(sizeof(Node));
		newNode -> count = min1-> count + min2 -> count;
		newNode -> left = min1;
		newNode -> right = min2;
		newNode -> str = NULL;
		newNode -> height = 0;
		insertHeap(array,size,newNode);
	}

}

void PreOrder (Node * ptr){
	if(ptr ==  NULL){

	}
	else{
	  //	  printf("str: %s\tcount: %d\theight: %d\n",ptr->str, ptr->count, ptr->height);
	  //	  if (ptr->str != NULL) printf("Count : %d\t\tString : %s\t\tHeight: %d\n",ptr->count, ptr->str, ptr->height);
	  //  else printf("Count : %d\t\tString : NULL\t\tHeight: %d\n",ptr->count, Height(ptr));
	  PreOrder (ptr->left);
	  PreOrder (ptr->right);
	}
}
void PreOrderHeap (Node ** array, int * size){
	int index = 0;
	while (index < *size){
	  //		printf("Index : %d Count : %d, String :  %s\n",index, array[index]-> count, array[index] -> str);
		index = index +1;
	}
}
void PreOrderHuff (Node * ptr){
	if(ptr ==  NULL){

	}
	else if (ptr -> str != NULL){
	  //printf("Count : %d\t\tString : %s\t\tHeight: %d\n",ptr->count, ptr->str, ptr->height);
	}
	else{
	  //printf("Count : %d\t\tString : NULL\t\tHeight: %d\n",ptr->count, Height(ptr));
		PreOrderHuff (ptr->left);
		PreOrderHuff (ptr->right);
	}
}


void freeEverything (Node * ptr){
	if(ptr == NULL){

	}
	else{
		freeEverything(ptr->left);
		freeEverything(ptr->right);
		free(ptr);
	}

}

void inOrder (Node * ptr, int * size){

	if(ptr ==  NULL){

	}
	else{
		*size = *size + 1;
		int temp = *size;
		inOrder (ptr->left, size);
		inOrder (ptr->right, size);
	}
}

Node ** mallocArray (int size){
    Node ** array = malloc(sizeof(Node**)*size);
    return array;
}


Node * buildCodetree (){
	int fd = open("HuffmanCodebook", O_RDONLY | O_CREAT, 00600);

}







void buildCodebook(Node * tree, char * bytestring, int bs_head){
  //creates a codebook.txt file representing tree’s properly formatted codebook
  //assume tree to be a balanced AVL tree of Node structs where Node-> represents frequency of a Node->str token
  if (tree == NULL) return;
  if (tree->left){
    bytestring[bs_head] = '0';
    buildCodebook(tree->left, bytestring, bs_head + 1);
  }
  if (tree->right){
    bytestring[bs_head] = '1';
    buildCodebook(tree->right, bytestring, bs_head + 1);
  }
  //if (tree->left == NULL && tree->right == NULL){
  if (tree->str != NULL){
    //is a leaf node: write <ASCII bytestring><tab><token><newline> to codebook.txt
    int fd = open("HuffmanCodebook", O_WRONLY | O_CREAT | O_APPEND, 00600);
    int wr, num, bytesToWrite;
    if (iscntrl(tree->str[0]) != 0 && strlen(tree->str) == 1){
      char * d = idControlCode(tree->str[0]);
      bytesToWrite = (strlen(bytestring) + strlen(d) + 3);
      char str[bytesToWrite];
      num = sprintf(str, "%s\t%s\n", bytestring, d);
      bytesToWrite = strlen(str);
      //printf("%s", str);
      wr = write(fd, str, bytesToWrite);
      close(fd);
    }
    else{
      bytesToWrite = (strlen(bytestring) + strlen(tree->str) + 3);
      char str[bytesToWrite];
      num = sprintf(str, "%s\t%s\n", bytestring, tree->str);
      bytesToWrite = strlen(str);
      //printf("%s", str);
      wr = write(fd, str, bytesToWrite);
      close(fd);
    }
    if (wr != bytesToWrite){
      //error in writing to file
      printf("Error writing codebook file.\n");
    }
  }
}

void writeDeCom(char * fileName,Node * ptr){
  if (ptr == NULL) printf("ptr is NULL\n");
  char * newflName = (char *) malloc(sizeof(char) * (strlen(fileName)-4));
  strncpy(newflName, fileName, (strlen(fileName)-4));
  //printf("in writeDeCom: newflName: %s\n", newflName);
  int fd = open(newflName, O_WRONLY | O_CREAT | O_APPEND, 00600);
	int ogfl = open(fileName, O_RDONLY);
	char buffer[1];
	Node * temp = ptr;
	//printf("ptr->str: %s\n", ptr->str);
	while(read(ogfl,buffer,1)>0){
	  //	  printf("buffer[0]: %c\n", buffer[0]);
	  if (buffer[0] == '0'){
 	    temp = temp -> left;
	    //printf("found a 0, heading to the left\n");
	    if(temp->str != NULL && temp->left == NULL && temp->right == NULL){
	      int wr;
	      //printf("temp->str: %s\n", temp->str);
	      //if (strlen(temp->str) > 1) printf("temp->str[1]: %c\n", temp->str[1]);
	      if (strcmp(inverseIdControlCode(temp->str), " ")==0 && temp->str[0] != ' '){
		//printf("found 0, writing %s\n", temp->str);
		wr = write(fd,temp->str,strlen(temp->str));
	      }
	      else{
		//char * c = inverseIdControlCode(temp->str);
		//printf("found 0, writing %s\n", c);
		//wr = write(fd,temp->str,strlen(c));
		//		printf("found a control code\n");
		if (strcmp(temp->str, "\\a")==0){
		  wr = write(fd,"\a",1);
		}
		else if (strcmp(temp->str, "\\b")==0){
		  wr = write(fd,"\b",1);
		}
		else if (strcmp(temp->str, "\\e")==0){
		  wr = write(fd,"\e",1);
		}
		else if (strcmp(temp->str, "\\f")==0){
		  wr = write(fd,"\f",1);
		}
		else if (strcmp(temp->str, "\\n")==0){
		  wr = write(fd,"\n",1);
		}
		else if (strcmp(temp->str, "\\r")==0){
		  wr = write(fd,"\r",1);
		}
		else if (strcmp(temp->str, "\\t")==0){
		  wr = write(fd,"\t",1);
		}
		else if (strcmp(temp->str, "\\v")==0){
		  wr = write(fd,"\v",1);
		}
		else if (strcmp(temp->str, "\\?")==0){
		  wr = write(fd,"\?",1);
		}
		else if (strcmp(temp->str, "\\'")==0){
		  wr = write(fd,"\'",1);
		}
		else if (strcmp(temp->str, "\\\"")==0){
		  wr = write(fd,"\"",1);
		}
		else{
		  wr = write(fd," ",1);
		}
	      }
	      //printf("%d bytes written\n", wr);
	      temp = ptr;
	    }
	  }
	  else {
	    temp = temp -> right;
	    //printf("found a %c, heading to the right\n", buffer[0]);
	    if (temp -> str!=NULL && temp->left == NULL && temp->right == NULL){
	      //	      printf("found 1, writing %s\n", temp->str);
	      int wr;
	      //printf("temp->str: %s\n", temp->str);
	      //if (strlen(temp->str) > 1) printf("temp->str[1]: %c\n", temp->str[1]);
	      if (strcmp(inverseIdControlCode(temp->str), " ")==0 && temp->str[0] != ' '){
		//printf("found 1, writing %s\n", temp->str);
                wr = write(fd,temp->str,strlen(temp->str));
	      }
	      else{
		//char * c = inverseIdControlCode(temp->str);
		//printf("found a control code\n");
		if (strcmp(temp->str, "\\a")==0){
                  wr = write(fd,"\a",1);
                }
                else if (strcmp(temp->str, "\\b")==0){
                  wr = write(fd,"\b",1);
                }
                else if (strcmp(temp->str, "\\e")==0){
                  wr = write(fd,"\e",1);
                }
                else if (strcmp(temp->str, "\\f")==0){
                  wr = write(fd,"\f",1);
                }
                else if (strcmp(temp->str, "\\n")==0){
                  wr = write(fd,"\n",1);
                }
                else if (strcmp(temp->str, "\\r")==0){
                  wr = write(fd,"\r",1);
                }
                else if (strcmp(temp->str, "\\t")==0){
                  wr = write(fd,"\t",1);
                }
                else if (strcmp(temp->str, "\\v")==0){
                  wr = write(fd,"\v",1);
                }
                else if (strcmp(temp->str, "\\?")==0){
                  wr = write(fd,"\?",1);
                }
                else if (strcmp(temp->str, "\\'")==0){
                  wr = write(fd,"\'",1);
                }
                else if (strcmp(temp->str, "\\\"")==0){
                  wr = write(fd,"\"",1);
                }
                else{
                  wr = write(fd," ",1);
                }
	      }
	      //printf("%d bytes written\n", wr);
	      temp = ptr;
	    }
	  }
	}
	close(fd);
}

Node * buildDeComtree (Node * ptr, char * byte, char * word){
		if (ptr == NULL){
		  //		  printf("He's NULL, Jim.\n");
			ptr = (Node*) malloc (sizeof (Node));
			ptr -> bytestring = NULL;
			ptr -> right = NULL;
			ptr -> left  = NULL;
		}

		Node * temp = ptr;
		int index = 0;
		int len = strlen(byte);
		while (index < len){
		  //		  printf("line 386: byte: %s\t\tindex: %d\n", byte, index);
			if (byte[index] == '0'){
				if(temp -> left == NULL){
					Node * newN  = (Node*) malloc(sizeof(Node));
					newN -> bytestring = NULL;
					newN -> right = NULL;
					newN -> left  = NULL;
					index = index + 1;
					temp->left = newN;
					temp = temp->left;
				}
				else {
					index = index + 1;
					temp = temp -> left;
				}
			}
			else {
				if(temp -> right == NULL){
				  Node * newN  = (Node*) malloc(sizeof(Node));
				  newN -> bytestring = NULL;
				  newN -> right = NULL;
				  newN -> left  = NULL;
				  index = index + 1;
				  temp->right = newN;
				  temp = temp->right;
				}
				else {
					index = index + 1;
					temp = temp -> right;
				}
			}
		}
		//printf("exited loop\n");
		temp -> bytestring = malloc(strlen(byte)* sizeof(char));
		temp -> bytestring = byte;
		temp -> str = malloc(strlen(word)* sizeof(char));
		temp -> str = word;
		//printf("end of call to buildDeComtree\n");
		//printf("temp->str: %s\t\tptr->str: %s\n", temp->str, ptr->str);
		return ptr;
}


char * idControlCode (char buffer){
	if (buffer == '\a'){
		char * str = (char*) malloc(sizeof(char) * 3);
		str = "\\a";
		return str;
	}
	else if (buffer == '\b'){
		char * str = (char*) malloc(sizeof(char) * 3);
		str = "\\b";
		return str;
	}
	else if (buffer == '\e'){
		char * str = (char*) malloc(sizeof(char) * 3);
		str = "\\e";
		return str;
	}
	else if (buffer == '\f'){
		char * str = (char*) malloc(sizeof(char) * 3);
		str = "\\f";
		return str;
	}
	else if (buffer == '\n'){
 		char * str = (char*) malloc(sizeof(char) * 3);
 		str = "\\n";
 		return str;
 	}
	else if (buffer == '\r'){
		char * str = (char*) malloc(sizeof(char) * 3);
		str = "\\n";
		return str;
	}
	else if (buffer == '\t'){
		char * str = (char*) malloc(sizeof(char) * 3);
		str = "\\t";
		return str;
	}
	else if (buffer == '\v'){
		char * str = (char*) malloc(sizeof(char) * 3);
		str = "\\v";
		return str;
	}
	else if (buffer == '\\'){
		char * str = (char*) malloc(sizeof(char) * 3);
		str = "\\";
		return str;
	}
	else if (buffer == '\''){
		char * str = (char*) malloc(sizeof(char) * 3);
		str = "\\'";
		return str;
	}
	else if (buffer == '\"'){
		char * str = (char*) malloc(sizeof(char) * 3);
		str = "\\\"";
		return str;
	}
	else if (buffer == '\?'){
		char * str = (char*) malloc(sizeof(char) * 3);
		str = "\\?";
		return str;
	}
	return "*";
	/*	else if (buffer == '\x'){
		char * str = (char*) malloc(sizeof(char));
		str = "\\x";
		return str;

	}
	*/
}


char * inverseIdControlCode (char * buffer){
  if (strcmp(buffer, "\\a")==0){
    return "\a";
  }
  if (strcmp(buffer, "\\b")==0){
    return "\b";
  }
  if (strcmp(buffer, "\\e")==0){
    return "\e";
  }
  if (strcmp(buffer, "\\f")==0){
    return "\f";
  }
  if (strcmp(buffer, "\\n")==0){
    return "\n";
  }
  if (strcmp(buffer, "\\r")==0){
    return "\r";
  }
  if (strcmp(buffer, "\\t")==0){
    return "\t";
  }
  if (strcmp(buffer, "\\v")==0){
    return "\v";
  }
  if (strcmp(buffer, "\\?")==0){
    return "\?";
  }
  if (strcmp(buffer, "\\'")==0){
    return "\'";
  }
  if (strcmp(buffer, "\\\"")==0){
    return "\"";
  }
  return " ";
}


Node* Parser(char * fileName){
  //printf("got to Parser\n");
  int size = 0;
  int first = 0;
  int total = 0;
  Node * treeName = NULL;
  int fd = open(fileName, O_RDONLY);
  if (fd < 0){
    printf("Error: file does not exist.\n");
    return NULL;
  }
  char buffer[1];
  char buffer2[1];
  int rd = read(fd, buffer, 1);
  while (rd > 0){
    if (iscntrl(buffer[0]) == 0 && buffer[0] != ' '){
      //      printf("non-delim character: _%c_\n", buffer[0]);
      size++;
      rd = read(fd, buffer, 1);
    }
    else{
      //printf("delim character: _%c_\n", buffer[0]);
  	if(size != 0){
	      //make a Node for the current token and insert
	      char * tkn = (char *) malloc(sizeof(char) * size);
	      int fd2 = open(fileName, O_RDONLY);
	      int moved = lseek(fd2, total, 0);
	      //	      printf("fd2 moved %d bytes\n", moved);
	      //printf("current size is %d and total is %d\n", size, total);
	      read(fd2, tkn, size);
	      Node * newTkn = (Node *) malloc(sizeof(Node));
	      newTkn->str = tkn;
	      treeName = insert(treeName, newTkn);
	      //  printf("newtkn: _%s_\n", tkn);
	}
	Node * newDelim = (Node *) malloc(sizeof(Node));
	char * deliminterStr;
	if (buffer[0] != ' '){
	  //	  printf("buffer[0] should not be a space here. It is: _%c_\n", buffer[0]);
	  //insert another Node containing the delimiter
	  //	  Node * newDelim = (Node *) malloc(sizeof(Node));
	  deliminterStr =(char *) malloc(sizeof(char)*2);
	  //	  deliminterStr[0] = '\\';
	  deliminterStr[0] = buffer[0];
	  deliminterStr[1] = '\0';
	}
	else{
	  //Node * newDelim = (Node *) malloc(sizeof(Node));
	  deliminterStr =(char *) malloc(sizeof(char)*2);
	  deliminterStr[0] = ' ';
	}
	//printf("newdelim: _%s_\n", deliminterStr);
	newDelim->str = deliminterStr;
	newDelim -> left = NULL;
	newDelim -> right = NULL;
	newDelim -> count = 0;
	treeName = insert(treeName, newDelim);
	//printf("_%s_\n", buffer);
	total = total + size + 1;
	size = 0;
	rd = read(fd, buffer, 1);
    }
  }
	close(fd);
	return treeName;
}

Node * ParCompress(char * codebookPath){
  Node * ptr = NULL;
  int size = 0;
  int first = 0;
  int total = 0;
  int fd = open(codebookPath, O_RDONLY);
  if (fd < 0){
    printf("Error: file does not exist.\n");
    return NULL;
  }
  char buffer[1];
  char buffer2[1];
  int rd = read(fd, buffer, 1);
  if (buffer[0] == '\\'){ //will skip control character in codebook
	rd = read(fd, buffer, 1);
	total = 1;
	}
  if (buffer[0] == '\n'){
	rd = read(fd, buffer,1);
	total = 2;
	}
  while (rd > 0){
    if (iscntrl(buffer[0]) == 0){
      size++;
      rd = read(fd, buffer, 1);
    }
    else{
			//make a Node for the current token and insert
			char * byte = (char *) malloc(sizeof(char) * size);
			int fd2 = open(codebookPath, O_RDONLY);
			int moved = lseek(fd2, total, 0);
			//printf("fd2 moved %d bytes\n", moved);
			//printf("current size is %d and total is %d\n", size, total);
			read(fd2, byte, size);
			//Node * newTkn = (Node *) malloc(sizeof(Node));
			//newTkn->bytestring = byte;
			total = total + size + 1;
			size = 0;
			close(fd2);
			//printf("_%s_\n", tkn);
			rd = read(fd, buffer,1);
			while (iscntrl(buffer[0]) == 0){
				size++;
				rd = read(fd, buffer, 1);
			}
			char * string = (char *) malloc(sizeof(char) * size);
			int fd3 = open(codebookPath, O_RDONLY);
			int moved3 = lseek(fd2, total, 0);
			//printf("fd2 moved %d bytes\n", moved3);
			//printf("current size is %d and total is %d\n", size, total);
			read(fd3, string, size);
			Node * toInsert = (Node*)(malloc(sizeof(Node)));
			toInsert->str = string;
			toInsert->bytestring = byte;
			toInsert->count = 0;
			//newTkn -> str = string;
			//newTkn -> left = NULL;
			//newTkn -> right = NULL;
			//newTkn -> count = 0;
			ptr = insert(ptr,toInsert);
			//insert another Node containing the delimiter
			//Node * newDelim = (Node *) malloc(sizeof(Node));
			//newDelim->str = idControlCode((char *) buffer);
			//insert(treeName, newDelim);
			//printf("_%s_\n", buffer);
			total = total + size + 1;
			size = 0;
			rd = read(fd, buffer, 1);
			close(fd3);
    }
  }
  return ptr;
}


Node *  ParDecompress(char * codebookPath){
  int size = 0;
  int first = 0;
  int total = 0;
  Node * ptr = NULL;
  int fd = open(codebookPath, O_RDONLY);
  if (fd < 0){
    printf("Error: file does not exist.\n");
    return NULL;
  }
  char buffer[1];
  char buffer2[1];
  int rd = read(fd, buffer, 1);
  if (buffer[0] == '\\'){ //will skip control character in codebook
    rd = read(fd, buffer, 1);
    total++;
  }
  if (buffer[0] == '\n'){ //will skip newline character in codebook
    rd = read(fd, buffer, 1);
    total++;
  }
  while (rd > 0){
    //printf("next buffer[0] is %c\n", buffer[0]);
    if (iscntrl(buffer[0]) == 0 && buffer[0] != ' '){
      //printf("regular char: -%c-\n", buffer[0]);
      size++;
      rd = read(fd, buffer, 1);
      //printf("rd value: %d\n", rd);
    }
    else{
      //printf("delim char line: -%c-\n", buffer[0]);
      //make a Node for the current token and insert
      char * byte = (char *) malloc(sizeof(char) * size);
      int fd2 = open(codebookPath, O_RDONLY);
      int moved = lseek(fd2, total, 0);
      //      printf("fd2 moved %d bytes\n", moved);
      if (size == 0) size++;
      //printf("current size is %d and total is %d\n", size, total);
      read(fd2, byte, size);
      //Node * newTkn = (Node *) malloc(sizeof(Node));
      //newTkn->bytestring = byte;
      total = total + size + 1;
      size = 0;
      //printf("_%s_\n", tkn);
			rd = read(fd, buffer,1);
      while ((iscntrl(buffer[0]) == 0) && rd > 0){
	      size++;
	      rd = read(fd, buffer, 1);
	    }
      if (size == 0) size++;
      //printf("current size: %d\t\tcurrent total: %d\n", size, total);
      char * string = (char *) malloc(sizeof(char) * size);
      int fd3 = open(codebookPath, O_RDONLY);
      int moved3 = lseek(fd3, total, 0);
      //printf("fd2 moved %d bytes\n", moved3);
      //printf("current size is %d and total is %d\n", size, total);
      read(fd3, string, size);
      //newTkn -> str = string;
      //newTkn -> left = NULL;
      //newTkn -> right = NULL;
      //newTkn -> count = 0;
      //printf("about to call buildDeComtree: byte: %s\tstring:-%s-\n", byte, string);
      ptr = buildDeComtree (ptr, byte,string);
      //printf("done with buildDeComptree\n");
      //insert another Node containing the delimiter
      //Node * newDelim = (Node *) malloc(sizeof(Node));
      //newDelim->str = idControlCode((char *) buffer);
      //insert(treeName, newDelim);
      //printf("_%s_\n", buffer);
      total = total + size + 1;
      size = 0;
      rd = read(fd, buffer, 1);
      //rd = read(fd, buffer, 1);
    }
    //printf("done with ParDecompress\n");
  }
	close(fd);
  return ptr;
}



void Rec_dflag(char*f1, char * codebook){
	Node*filename = ParDecompress(codebook);
	cListAll(filename, f1);
}

void dListAll(Node * filetree, char * dirName){ // char * codebook

  DIR * dir;
  struct dirent * entry;
  if(!(dir = opendir(dirName))){
    return;
  }
  //printf("before while\n");
  while ((entry = readdir(dir)) != NULL) {
    //printf("while loop\n");
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
    char * fileName = (char *) malloc(sizeof(char) * (strlen(entry->d_name) + strlen(dirName) + 2));
    strcpy(fileName, dirName);
    strcat(fileName, "/");
    strcat(fileName, entry->d_name);
    strcat(fileName, "\0");
    if(entry == NULL){
	printf("NULL\n");
    }
    if (entry->d_type == DT_DIR) {
      //printf("entry->d_name : %s fileName: %s\n", entry->d_name, fileName);
      dListAll(filetree, fileName);
    }
    else if (entry->d_type == DT_REG) {
      //printf("entry->d_name : %s fileName : %s\n", entry -> d_name, fileName);
      // compress(filetree, fileName);
	  //printf("file\n");
	  //	  push(st, fileName);
      writeDeCom(fileName, filetree);
    }
    else{
      printf("Error: %s is not a file nor a directory.\n", dirName);
    }
    free(fileName);
  }
  closedir(dir);
}

void Rec_cflag(char*f1, char * codebook){
	Node*filename = ParCompress(codebook);
	//int space = 0;
	//int * treeSize = &space;
	//inOrder(filename, treeSize);
	//Stack * st = makeStack(*treeSize);
	char * flName = (char *) malloc(sizeof(char) * (strlen(f1) + 6));
	strcpy(flName, f1);
	strcat(flName, "~.hcz");
	DIR * dir;
	struct dirent * entry;
	dir = opendir(flName);
	if(dir == NULL){
	  cListAll(filename, f1);
	}
	//cListAll(filename, f1);
	//while(stEmpty(st) != 1){
	//  char * fl = pop(st);
	// printf("file from stack: %s\n", fl);
	//compress(filename, fl);
	//	}
}

void cListAll(Node * filetree, char * dirName){ // char * codebook

  DIR * dir;
  struct dirent * entry;
  if(!(dir = opendir(dirName))){
    return;
  }
  //printf("before while\n");
  while ((entry = readdir(dir)) != NULL) {
    //printf("while loop\n");
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
    char * fileName = (char *) malloc(sizeof(char) * (strlen(entry->d_name) + strlen(dirName) + 2));
    strcpy(fileName, dirName);
    strcat(fileName, "/");
    strcat(fileName, entry->d_name);
    strcat(fileName, "\0");
    //char * fileNameWithDot = (char *) malloc(sizeof(char) * 2);
    //fileNameWithDot[0] = '.';
    //fileNameWithDot[1] = '/';
    //strcat(fileNameWithDot, fileName);
    if(entry == NULL){
	printf("NULL\n");
    }
    if (entry->d_type == DT_DIR) {
      //printf("entry->d_name : %s fileName: %s\n", entry->d_name, fileName);
      cListAll(filetree, fileName);
    }
    else if (entry->d_type == DT_REG) {
	  printf("entry->d_name : %s fileName : %s\n", entry -> d_name, fileName);
	  compress(filetree, fileName);
	  //printf("file\n");
	  //	  push(st, fileName);
    }
    else{
      printf("Error: %s is not a file nor a directory.\n", dirName);
    }
    free(fileName);
  }
  closedir(dir);
}

void testListAll(char * dirName){
  printf("start of testListAll: %s\n", dirName);
  //  struct dirent * dirEntry;
  //  struct dirent * entry;
  DIR * dir;
  struct dirent * entry;
  if(!(dir = opendir(dirName))){
    return;
  }
  while ((entry = readdir(dir)) != NULL) {
    printf("dirName: %s\t\tentry->d_name: %s\n", dirName, entry->d_name);
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

    char * fileName = (char *) malloc(sizeof(char) * (strlen(entry->d_name) + strlen(dirName) + 2));
    strcpy(fileName, dirName);
    strcat(fileName, "/");
    strcat(fileName, entry->d_name);

    if (entry->d_type == DT_DIR) { //dirName is a directory name
      printf("found a directory: %s\n", fileName);
      testListAll(fileName);
    }
    else if (entry->d_type == DT_REG) { //dirName is a file name	\
      printf("found a file: %s\n",entry->d_name);
      //Parser(entry->d_name);
      //dFlag(entry->d_name, huffCodebook);

      printf("file: %s\n", entry->d_name);

    }
    else{
      printf("Error: %s is not a file nor a directory.\n", dirName);
    }
      //entry = readdir(dir);
      //}
  }
  //  else{
  //  printf("Error: %s is not a directory.\n", dirName);
  //}
  closedir(dir);
}

char * getBytestring(Node * tree, char * tok){
  if (tree == NULL)
    return NULL;
  if (strcmp(tok, tree->str) == 0){
    return tree->bytestring;
  }
  if (strcmp(tok, tree->str) < 0){ //tok is less than current node
    if (tree->left == NULL){
      printf("Error: tree does not contain token.\n");
      return NULL;
    }
    return getBytestring(tree->left, tok);
  }
  else{ //tok is greater than current node
    if (tree->right == NULL){
      printf("Error: tree does not contain token.\n");
      return NULL;
    }
    return getBytestring(tree->right, tok);
  }
  return NULL;
}

int getSizeOfBytestring(Node * tree, char * tok){
  if (tree == NULL)
    return -1;
  if (strcmp(tok, tree->str) == 0){
    return strlen(tree->bytestring);
  }
  if (strcmp(tok, tree->str) < 0){ //tok is less than current node
    if (tree->left == NULL){
      printf("Error: tree does not contain token.\n");
      return -1;
    }
    return strlen(getBytestring(tree->left, tok));
  }
  else{ //tok is greater than current node
    if (tree->right == NULL){
      printf("Error: tree does not contain token.\n");
      return -1;
    }
    return strlen(getBytestring(tree->right, tok));
  }
  return -1;
}


void compress(Node * fileTree, char * fileName){
  int size = 0;
  int first = 0;
  int total = 0;
  char * newflname = (char *) malloc(sizeof(char) * (strlen(fileName) + 5));
  int n = sprintf(newflname, "%s.hcz", fileName);
  int returnfl = open(newflname, O_WRONLY | O_CREAT | O_APPEND, 00600);
  int fd = open(fileName, O_RDONLY);
  if (fd < 0){
    printf("Error: file does not exist.\n");
    return;
  }
  char buffer[1];
  char buffer2[1];
  int rd = read(fd, buffer, 1);
  int tknWritten = 1;
  while (rd > 0 || tknWritten == 0){
    //printf("buffer[0]: %c\n", buffer[0]);
    if ((iscntrl(buffer[0]) == 0 && buffer[0] != ' ') && rd != 0){
      tknWritten = 0;
      size++;
      rd = read(fd, buffer, 1);
    }
    else if(size == 0){
      char * deliminterStr;
      if (buffer[0] == ' '){
        deliminterStr = (char *) malloc(sizeof(char));
	deliminterStr = " ";
      }
      else{
        deliminterStr =(char *) malloc(sizeof(char)*3);
        /*      deliminterStr[0] = 47;
        deliminterStr[1] = buffer[0];
        deliminterStr[2] = '\0';
        */
        deliminterStr = idControlCode(buffer[0]);
        //deliminterStr = buffer;
      }
      //printf("allocate for delim\n");
      printf("get bytestring: dbs\n");
      char * dbs = (char *) malloc(sizeof(char) * getSizeOfBytestring(fileTree, deliminterStr));
      dbs = getBytestring(fileTree, deliminterStr);
      int dwr = write(returnfl, dbs, getSizeOfBytestring(fileTree, deliminterStr));
      //printf("line 996\n");
      printf("get bytestring: dbs\n");
      total = total + 1;
      rd = read(fd, buffer, 1);
    }
    else{
      char * tkn = (char *) malloc(sizeof(char) * (size + 1));
      int fd2 = open(fileName,O_CREAT | O_RDONLY | O_APPEND);
      int moved = lseek(fd2, total, 0);
      int bytesRead = read(fd2, tkn, size);
      char *tknNew = (char *) malloc(sizeof(char) * (size+1));
      tknNew = tkn;
      //      strlcpy(tknNew, tkn, 1);
      /*
      if (size == strlen(tkn)) printf("size == strlen(tkn)\n");
      else printf("they're not equal\n");
      int z = 0;
      while (z < size){
	//	printf("z is %d\n", z);
	printf("ascii of tkn: %d\tascii of a: %d\n", tkn[z], 'a');
	tknNew[z] = tkn[z];
	z++;
      }
      */
      int z = size;
      while (z < strlen(tkn)){
	tkn[z] = '\0';
	z++;
      }
      printf("get bytestring: fileName: %s\ttkn: %s\tsize: %d\tbytesRead: %d\tmoved: %d\ttotal: %d\n", fileName, tkn, size, bytesRead, moved, total);
      printf("size: %d\tsize of tkn: %d\ttknNew: %s\tsize of tknNew: %d\n", size, strlen(tkn), tknNew, strlen(tknNew));
      char * bs = (char *) malloc(sizeof(char) * strlen(getBytestring(fileTree, tkn)));
      printf("get bytestring: got through get size of\n");
      bs = getBytestring(fileTree, tkn);
      printf("get bytestring: bs\n");
      int wr = write(returnfl, bs, strlen(bs));
      tknWritten = 1;
      //printf("wr = %d\n", wr);
      //      if (twoDelimsInARow != 0){
      /*
      char * deliminterStr;
      if (buffer[0] == ' '){
	deliminterStr = (char *) malloc(sizeof(char));
				deliminterStr = " ";
      }
      else{

      	deliminterStr =(char *) malloc(sizeof(char)*3);
	/*	deliminterStr[0] = 47;
      	deliminterStr[1] = buffer[0];
      	deliminterStr[2] = '\0';

	deliminterStr = idControlCode(buffer[0]);
	//deliminterStr = buffer;
      }
      printf("allocate for delim\n");
      char * dbs = (char *) malloc(sizeof(char) * getSizeOfBytestring(fileTree, deliminterStr));
      dbs = getBytestring(fileTree, deliminterStr);
      int dwr = write(returnfl, dbs, getSizeOfBytestring(fileTree, deliminterStr));
      printf("line 996\n");
      */
      close (fd2);
      total = total + size;
      size = 0;
    }
    //printf("rd = %d\n", rd);
  }
	close(fd);
}


void removeConnections(Node * ptr){
  if(ptr==NULL){


  }
  else if(ptr->str !=NULL){
    ptr->left = NULL;
    ptr->right=NULL;
  }
  else{
    removeConnections(ptr->left);
    removeConnections(ptr->right);
  }


}

void bFlag(char * fl){
  Node * fileTree = Parser(fl);
  PreOrder(fileTree);
  int space = 0;
  int * treeSize = &space;
  char * bs = (char *) malloc(sizeof(char) * (*treeSize + 1));
  inOrder(fileTree, treeSize);
  Node ** array = mallocArray(*treeSize);
  int in = 0;
  int * index = &in;
  makeArray(array, index, fileTree);
  buildHeap(array,*treeSize-1);
  PreOrderHeap(array,treeSize);
  makeHuff(array,treeSize);
  PreOrderHuff(array[0]);
  PreOrder(array[0]);
  int hcfl = open("HuffmanCodebook", O_WRONLY | O_CREAT, 00600);
  char t[3] = {'\\', '\n', '\0'};
  int sz = strlen((char *) t);
  write(hcfl, t, sz);
  removeConnections(array[0]);
  buildCodebook(array[0], bs, 0);
  close(hcfl);
}

void cFlag(char * fl, char*codebook){
  printf("cFlag: %s\n", fl);
  Node * fileTree = ParCompress(codebook);
  compress(fileTree, fl);
}

void dFlag (char * fileName, char * codebook){
	Node * ptr = ParDecompress(codebook);
	//printf("finished making tree\n");
	if (ptr == NULL) printf("ptr is NULL\n");
	writeDeCom(fileName, ptr);
}

int main(int argc, char **argv){

  /*
   if (argc < 3){
    printf("Error: insufficient arguments.\n");
    return 0;
   }

   int i = 0;
   while (i < argc){
     if (strcmp(argv[i], "-b") == 0){
        bFlag(argv[i+1]);
     }
     else if (strcmp(argv[i], "-c") == 0){
       cFlag(argv[i+1],argv[i+2]);
     }
     else if (strcmp(argv[i], "-d") == 0){
       dFlag(argv[i+1], argv[i+2]);
     }
     i++;
   }
  */

  //  testListAll("tests");
  //printf("end of test\n\n");

  printf("argv[4]: %s\n", argv[4]);

  if (argc < 3){
    printf("Error: insufficient arguments.\n");
    return 0;
  }
	if (argc > 5){
		printf("Error: invalid arguments.\n");
    return 0;
	}

   int i = 0;
	 int r = 0;

	 while (i < argc){
		 if (strcmp(argv[i], "-R") == 0){
				 r =1 ;
		 }
		 i++;
	}

	i =0;
	char arg2 = ' ';
	 while (i < argc){
		 if (strcmp(argv[i], "-b") == 0){
				arg2 = 'b';
				break;
		 }
		 else if (strcmp(argv[i], "-c") == 0){
			 	arg2 = 'c';
				break;
		 }
		 else if (strcmp(argv[i], "-d") == 0){
			 	arg2 = 'd';
				break;
		 }
		 i++;
	}

	if (r){
		if (arg2 == 'b'){
		  //bFlag(argv[3]);
		}
		else if (arg2 == 'c' ){
			Rec_cflag(argv[3], argv[4]);
		}
		else if(arg2 == 'd'){
			dListAll(argv[3],argv[4]);
		}
	}
	else{
		if (arg2 == 'b'){
			bFlag(argv[2]);
		}
		else if (arg2 == 'c' ){
			cFlag(argv[2],argv[3]);
		}
		else if(arg2 == 'd'){
			dFlag(argv[2], argv[3]);
		}
}

   return 0;

}
