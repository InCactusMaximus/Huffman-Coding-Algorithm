#include "Tree.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctype.h>
#include <vector>

using namespace std;


int FindSmallest(vector<int>& freq) { // Returns the index of the smallest number in a vector, if there are several numbers tied for smallest, it picks the last one

    int smallest = freq[0];
    int index = 0;

    for (int i = 0; i < freq.size(); i++) {

        if (freq[i] <= smallest) {
            smallest = freq[i];
            index = i;
        } 
    }

    return index;
}

string LookUpChar(char c, vector<CharCode> table) { // Finds a given character in the table and returns its code
    
    for (int i = 0; i < table.size(); i++) {
        if (table[i].character == c) {
            return table[i].code;
        }
    }

    return "?"; // Returns ? as a flag if not found
}

string Encode(string input, vector<CharCode> table) {
    
    bool error = false;
    string code;

    for (int i = 0; i < input.length(); i++) {
        if (LookUpChar(input[i], table) == "?") {
            error = true;
        }
        else {
            code.append(LookUpChar(input[i], table)); // Look up and append all code to the result
        }
    }

    if (error) { // If there was an error, append a ? at the end so the user can be warned that the message was not encoded fully
        code.append("?");
    }

    return code;
}

string Decode(string input, Tree<char> code_tree) {

    TreeNode<char>* n = code_tree.root;
    string msg;

    for (int i = 0; i < input.length(); i++) { // For each character in the input, if it is a 0, traverse left, if it is a 1, traverse right
        if (input[i] == '0') {
            n = n->left;
        }
        else if (input[i] == '1') {
            n = n->right;
        }

        if (n->char_freq > 0) { // Once a leaf node is found, append the character in the node to the output, then move back to the root and start over
            msg.push_back(n->key);
            n = code_tree.root;
        }
    }

    return msg;
}

void FindCharCode(TreeNode<char>* n, vector<CharCode>& table, string code) { // Uses a recursive post-order traversal of the code tree to generate the code for each character

    if (n->char_freq == 0) { // If n is not a leaf node

        FindCharCode(n->left, table, (code + "0")); // Traverse left and add a 0 to the code
        FindCharCode(n->right, table, (code + "1")); // Traverse right and add a 1 to the code
    }
    else {
        table.emplace_back(CharCode(n->key, code)); // If a leaf is found, place the character along with the generated code in the lookup table
    }
}

vector<CharCode> GetLookupTable(Tree<char>& code_tree, vector<char>& characters) { // Builds the lookup table where characters are mapped to their corresponding code

    cout << "\nGenerating character lookup table...\n";

    vector<CharCode> look_table;
    FindCharCode(code_tree.root, look_table, "");

    cout << "...Done!\n\nTable:\n\n";

    for (int i = 0; i < look_table.size(); i++) { // Printing table
        look_table[i].Print();
    }

    return look_table;
}

Tree<char> GetHuffTree(vector<char> characters, vector<int> freq) { // Returns the code tree given a vector of characters and a corresponding frequency vector

    cout << "\nBuilding Binary Code Tree...\n"; 

    vector<Tree<char>> tree_vec; // Holds all the trees which will be combined together to create the code tree
    int smallest; // Holds index of smallest frequency
    int weight; // Weight holds the combined frequency of all leaves in a tree. When two trees are combined, their frequencies are added together to get the new tree's weight
    TreeNode<char>* temp; // Temp node pointer used for creation of new tree

    tree_vec.resize(characters.size()); // Make room for all characters

    for (int i = 0; i < characters.size(); i++) {

        tree_vec[i].root = new TreeNode<char>(characters[i]); // Creates a new tree of one node for each character in the character vector, initializing the root to the corresponding character and frequency
        tree_vec[i].root->char_freq = freq[i];
    }

    while (tree_vec.size() != 1) { // Combines two trees until there is only one tree left

        smallest = FindSmallest(freq);                  // Find the first tree, the one with the overall smallest frequency
        weight = freq[smallest];                        // Set the weight equal to the frequency
        freq.erase(freq.begin() + (smallest));          // Remove that frequency from the frequency vector

        temp = new TreeNode<char>('~');                 // Create a new node, which will be the root node of the new tree
        temp->right = tree_vec[smallest].CopyTree();    // Set the new tree's right subtree equal to the entirety of the first tree
        tree_vec.erase(tree_vec.begin() + smallest);    // Delete the first tree from the tree vector

        smallest = FindSmallest(freq);                  // Find the second tree, which would have been the second smallest in the vector before the first tree was deleted
        weight += freq[smallest];                       // Sum the frequencies of the first and second tree
        freq.erase(freq.begin() + smallest);            // Delete the frequency of the second tree from the freq vector
        temp->left = tree_vec[smallest].CopyTree();     // Copy the entirety of the second tree into the new tree's left subtree

        tree_vec.resize(tree_vec.size() + 1);           // Make room for one more tree in the tree vector
        tree_vec.back().root = temp;                    // The back of the vector is the new tree. Set the back tree's root equal to the root of the new tree which was just created
        tree_vec.erase(tree_vec.begin() + smallest);    // Delete the second tree from the tree vector
        freq.push_back(weight);                         // Add the frequency of the new tree to the frequency vector
        
    }

    cout << "...Done!\n";
    cout << "\nPrinted code tree: (higher frequencies should be closer to the left, and therefore closer to the root node)\n\n";

    tree_vec[0].PrintTree();

    return tree_vec[0];
}

int FindCharIndex(vector<char> &characters, char c) { // Searches a vector for a given character, returning the index of the character if it is found

    for (int i = 0; i < characters.size(); i++) {
    
        if (characters[i] == c) {
            return i;
        }
    }

    return -1; // Return -1 if character is not found in the vector
}

void GetFrequency(vector<char> &characters, vector<int> &freq) { // Reads the file and initializes the character and frequency vectors

    cout << "\nReading file and determining frequencies of each character...\n";

    FILE* original_text;
    original_text = fopen("war_and_peace.txt", "r"); // Text input file which is "war and peace" from my Homework 5 program. Although, this program should work with any text file.

    unsigned char cur_char = 0;
    int index;

    while (!feof(original_text)) { // Reads the input file character by character until it reaches the end

        cur_char = getc(original_text);
        index = FindCharIndex(characters, cur_char); // Search the vector for the character

        if (index == -1) { // If it is a new character, add it to the vector and set the frequency to 1 
            characters.push_back(cur_char);
            freq.push_back(1);
        }
        else {
            freq[index]++; // Adds 1 to the frequency of the character if it is already found in the vector
        }       
    }

    fclose(original_text);

    for (int i = 0; i < characters.size(); i++) { // Display all the characters and their frequencies

        if (characters[i] == '\n') { // Special case for \n and \t as they will mess up formatting if displayed normally
            cout << "char: " << "\\n";
        }
        else if (characters[i] == '\t') {
            cout << "char: " << "\\t";
        }
        else {
            cout << "char: " << characters[i];
        }

        cout << " freq: " << freq[i] << endl;
    }
    
    cout << "...Done!\n";
}

int main() {

    vector<char> char_vec; // Stores all characters found in the file
    vector<int> freq_vec; // Stores the frequency of each character in the file. Indices correspond to char_vec, so the frequency of the character at char_vec[i] can be found at freq_vec[i]
    Tree<char> code_tree; // Encoding tree
    vector<CharCode> code_table; // Lookup table for all characters and their respective codes
    string user_input, code_msg;

    GetFrequency(char_vec, freq_vec);
    code_tree = GetHuffTree(char_vec, freq_vec);
    code_table = GetLookupTable(code_tree, char_vec);

    cout << "\n**Scroll up for more details, including a graph of the generated code tree**\n\n";
    cout << "Please type the message which you would like to encode: \n";

    getline(cin, user_input);

    while (user_input != ":q:") { 

        cout << "\nYour message: " << user_input;
        code_msg = Encode(user_input, code_table);
        
        if (code_msg[code_msg.length() - 1] == '?') { // If the encoding runs into an error due to the user entering a symbol which did not exist in the source text file
            cout << "\nSome of the symbols you used in your message do not exist in the code tree. This is because the source text file did not include them. These symbols have been skipped over.";
        }

        cout << "\nEncoded: " << code_msg;
        cout << "\nDecoded: " << Decode(code_msg, code_tree) << "\n\nIf you would like to try another message, type it into the terminal, otherwise, type :q: to quit\n";

        getline(cin, user_input);
    }
    
    cout << "\nGoodbye.";
    return 0;
}
