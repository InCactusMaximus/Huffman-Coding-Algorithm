#ifndef TREE_H_ADTREE
#define TREE_H_ADTREE
#include <iostream>

using namespace std;

template <class T>
class TreeNode {
public:
    T key; 
    int char_freq;
    TreeNode* left;
    TreeNode* right;
    TreeNode(T val);
    void Print(int count);
};

template<class T>
class Tree {
public:
    TreeNode<T>* root;
    void PrintTree();
    TreeNode<T>* Copy(TreeNode<T>* n);
    TreeNode<T>* CopyTree();
};

class CharCode { // Holds a character along with its corresponding Huffman binary code
public:
    char character;
    string code;
    CharCode(char c, string binary);
    void Print();
};

CharCode::CharCode(char c, string binary) {
    character = c;
    code = binary;
}

void CharCode::Print() {

    if (character == '\n') { // Special case for \n and \t as they will mess up formatting if displayed normally
        cout << "char: \'\\n";
    }
    else if (character == '\t') {
        cout << "char: \'\\t";
    }
    else {
        cout << "char: \' " << character;
    }
    cout << "\'|_ code: " << code << endl;
}

template <class T>
TreeNode<T>::TreeNode(T val) {
    key = val;
    left = NULL;
    right = NULL;
    char_freq = 0; // Char frequency is initialized to 0 by default, indicating an empty space in a tree. Only characters (leaf nodes) have a non-zero frequency
}

template <class T>
void TreeNode<T>::Print(int count) { // Recursive print tree function based off of the one from sillyTree.cpp on Canvas
    for (int i = 0; i < count; i++) {
        cout << "\t";
    }

    if (count == 0) { // If function is currently on the root
        cout << "ROOT" << endl;
    }
    else if (key == '\n') { // Special case for \n and \t as they will mess up formatting if displayed normally
        cout << "\"\\n\": " << char_freq << endl;
    }
    else if (key == '\t') {
        cout << "\"\\t\": " << char_freq << endl;
    }
    else if (char_freq > 0) { // If the node is a character and not just an empty space, print the key
        cout << "\"" << key << "\": " << char_freq << endl;
    }
    else {
        cout << "~" << endl; // If the node is empty, ignore the key and print out a "~" instead
    }
    
    count++;
    if (left != NULL) {
        left->Print(count);
    }
    if (right != NULL) {
        right->Print(count);
    }
}

template <class T>
void Tree<T>::PrintTree() {
    root->Print(0);
}

template <class T>
TreeNode<T>* Tree<T>::Copy(TreeNode<T>* n) { // Recursive copy function which exists to construct a new tree so that the old one can be safely deleted without causing any pointer issues.
    TreeNode<T>* new_node = new TreeNode<T>(n->key); // Starts on the root node of the tree, uses preorder traversal
    new_node->char_freq = n->char_freq;

    if (n->left != NULL) {
        new_node->left = Copy(n->left);
    }    
    if (n->right != NULL) {
        new_node->right = Copy(n->right);
    }

    return new_node;
}

template <class T>
TreeNode<T>* Tree<T>::CopyTree() { // Copy function is called on a tree from here. It returns a pointer to the root node of the copied tree
    return Copy(root);
}

#endif
