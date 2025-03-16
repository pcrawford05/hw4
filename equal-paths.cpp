#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

//Finds the maximum depth of a root given that they have the same distance to their children
void maxDepth(Node * root, int& depth){
    if(root == NULL)
        return;
    depth++;
    if(root->left == NULL){
        if(root->right == NULL){
            return;
        } else {
            maxDepth(root->right, depth);
        }
    } else { //Left is an option
        maxDepth(root->left, depth);
    }
}

bool equalPaths(Node * root)
{
    if(root == NULL)
        return true;
    if(root->left == NULL && root->right == NULL)
        return true;

    // If only one child then recurse down that side
    if(root->left == NULL && root->right != NULL)
        return equalPaths(root->right);
    if(root->left != NULL && root->right == NULL)
        return equalPaths(root->left);
    
    // If has both children we check if there's equalpaths to their children 
    // but also if they are the same depth
    int dL = 0; int dR = 0;
    maxDepth(root->left, dL); maxDepth(root->right, dR);
    return (equalPaths(root->left) && equalPaths(root->right) && dL == dR);
}

