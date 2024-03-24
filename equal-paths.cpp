#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool equalPathsHelper(Node* root, int depth, int& expectedDepth){
    if(root == NULL){
        return true;
    }
    if(root->left == NULL && root->right == NULL){
        if(expectedDepth == -1){
            expectedDepth = depth;
        }
        else if(depth != expectedDepth){
            return false;
        }
    }

    return equalPathsHelper(root->left, depth+1, expectedDepth) && equalPathsHelper(root->right, depth+1, expectedDepth);
}

bool equalPaths(Node * root)
{
    // Add your code below
    int expectedDepth = -1;
    return equalPathsHelper(root, 0, expectedDepth);
}

