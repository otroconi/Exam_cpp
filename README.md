#Binary search tree (bst)####

## To compile:

######First option:

```
c++ bst_otroconi.cpp -o bst_otroconi.x -Wall -Wextra -g -std=c++14

```

######Second option: using the Makefile

```
make

```
## To run: (valgrind is optional: only if you want to check if there is no memory leaks)

```
valgrind ./bst_otroconi.x

```

## Comments:

- There are many comments (//) in the code some of them I used to debug (and realize where were the problem when I had complains from the compiler). Others coments are just for myself in order to remember some things that I understand thanks to the complains of the compiler. As well I comment to myself about some things that we studied during the videos lectures and I wrote in my notes.

- At this moment I am not sure why the find() function is not working, i.e. if I write tree.find(value) it compiles and run, but if I try to access the key or value of that key that I am sure is in the tree, I have segmentation fault.

- At the beggining I wrote the class node having the key as a member, but then remembering the lecture about the class animal example, I realized it was better to write the class node similar to the linked list but with left, right and parent. As well, it is easier to have only one template in the class node. I started with two templates, but then it was difficult for me to manage it inside the bst class.

- One thing that was hard to understand for me was the syntaxis when a function were declared private inside the class but then, I needed to define outside, hence I needed to template very well and with a fully qualified name.

- Hence with this implementation the class node has the members: value, right, left and parent (this help us to looking for who is the right and left). The class iterator recommended to be declared outside the classes nodes and bst, has a current as a member, been current a node (he will help us to iterate or "travel, walk" inside the class bst). Finally the class bst has as members a root (which is a node, and hence has the characteristics of the class node) and an operator.


 
