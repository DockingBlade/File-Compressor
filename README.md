# File-Compressor



The fileCompressor program creates, opens, reads, and writes files, using file descriptors and
open(). The program also builds and traverses various trees and lists, utilizing Huffman coding to
compress and decompress files.
This program takes three flags from users, each indicating a distinct function:

-b : “build codebook”

-c : “compress”

-d : “decompress”

The recursive flag will perform one of the first three operations on either a single file provided
by the user, or in the case of an -R flag, all files which are below the directory pathway given by
the user.

b flag:
The b flag will take a file name and feed it to the Parser method. The Parser will read bits one by
one until a delimiter is read, and it will store all tokens and delimiters in an AVL tree using the
insert() method. This method will insert in the tree, performing rotations as needed to keep the
tree balanced, storing the counts for each token and keeping insertion to log(n) for each word
(for a total O(nlogn)). Imbalanced nodes are detected by comparing node heights, which are
calculated with the Height() function. Then the inOrder() method is used to calculate the number
of the nodes in the tree (size) by doing an inOrder traversal of the AVL tree and incrementing the
value stored in a int pointer. Using the size, we allocate space for an array of Node pointers,
which will be used to represent a heap. makeArray() then assigns a index to each Node in the
AVL by doing an inOrder traversal. Then buildHeap() heapifies the array, by performing
shiftDown() on the appropriate half of the nodes in the heap to heapify the array. Subsequently,
the makeHuff() method builds the Huffman tree by using the extractMin() method twice, then
creating a new Node with no string, with a count equal to the sum of the two counts of the
extracted nodes. makeHuff() then calls on the insertHeap() method, which the places the new
Node at the end of the array, and then uses the shiftUp() operation to place the Node in the
correct spot in the array. We then open the “HuffmanCodebook”, and write the escape character.
Then we use the removeConnections() on the first Node on the Heap, which traverses the tree,
and sets the left, and right pointers with a non-NULL string to left and right to NULL. This is
done to make sure that the nodes with strings are leaves (these nodes have children from the
AVL tree, as they were originally nodes in the AVL tree, and had children). Finally, the
buildCodebook method uses inOrder traversal to set the byteStrings of each Node by setting the
last char in the byteString to 0 if it is the left child and 1 if it is the right child. If the node has a
non-NULL string, the method writes the byteString and the string to the file.

-c
Compress will take the fileName, and the path to the HuffmanCodebook. It feeds the
HuffmanCodebook to the ParCompress, which is a special Parser that takes the codebook and
makes an AVL tree which contains the byteStrings, and the Strings they map to. Then the
compress() method takes in the filename, and the head of the AVL tree. Compress then separates
words and delimiters, and then searches for each in the tree, and then writes the corresponding
byteString to the file.

-d
dflag takes a .hcz file, and a path to the HuffmanCodebook. It feeds the path to the codebook,
and parses the codeBook and inserts the ByteStrings, and inserts the corresponding strings into a
pseudo-Huffman Tree using the buildDeComtree() method. It does this by taking the first node
of the tree, and traveling left if the next bit in the byteString is zero, and travel right if the next
bit is one, creating empty nodes along the path if needed. Then the node is inserted after the last
byte is read. Finally, dFlag takes the head of the tree built, and the filename name and passes
them to writeDeCom(). This method starts at the head and goes to the Nodes left if 0, and right if 1. This process continues until a leaf node is hit, at which point the leaf’s byteString is written, and the next bit in the file starts the process again at the head, until the file ends.
