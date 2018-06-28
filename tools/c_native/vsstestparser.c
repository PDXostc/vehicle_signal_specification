/**
* (C) 2018 Volvo Cars
*
* All files and artifacts in this repository are licensed under the
* provisions of the license provided by the LICENSE file in this repository.
*
* 
* Example of parser for a native format VSS tree.
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include "nativeCnodeDef.h"
#include "vssparserutilities.h"

#define MAXTREEDEPTH 8
#define MAXFOUNDNODES 150


struct NodeHandle_t parserHandle;
struct NodeHandle_t* currentNode = &parserHandle;
struct NodeHandle_t rootNode;

char* getTypeName(nodeTypes_t type) {
    switch (type) { 
        case BRANCH:
                return "BRANCH";
        case BOOLEAN:
                return "BOOLEAN";
        case UINT8:
                return "UINT8";
        case INT8:
                return "INT8";
        case UINT16:
                return "UINT16";
        case INT16:
                return "INT16";
        case UINT32:
                return "UINT32";
        case INT32:
                return "INT32";
        case DOUBLE:
            return "DOUBLE";
        case FLOAT:
            return "FLOAT";
        case STRING:
            return "STRING";
        case RBRANCH:
            return "RBRANCH";
        case ELEMENT:
            return "ELEMENT";
        default:
            printf("getTypeName: unknown type\n");
            return "unknown";
        break;
    } // switch
}

void updateHandle(struct NodeHandle_t* currentNode, node_t* nodePtr) {
    currentNode->nodePtr = nodePtr;
    currentNode->nodeType = nodePtr->type;
}

int main(void) {

    treeFp = fopen("../../vss_rel_1.0.cnative", "r");
    if (treeFp == NULL) {
        printf("Could not open file for reading tree data\n");
        return 0;
    }
    rootNode.nodePtr = (void*)VSSReadTree();
    rootNode.nodeType = ((node_t*)rootNode.nodePtr)->type;
    fclose(treeFp);


    char traverse[10];
    printf("\nTo traverse the tree: 'u'(p)/'d'(own)/'l'(eft)/'r'(ight)/g(et)/h(elp), or any other to quit\n");
    memcpy(currentNode, &rootNode, sizeof(NodeHandle_t));
    int currentChild = 0;
    while (true) {
        printf("\n'u'/'d'/'l'/'r'/'g'/'h', or any other to quit: ");
        scanf("%s", traverse);
        switch (traverse[0]) {
            case 'u':  //up
                if (((node_t*)currentNode->nodePtr)->parent != NULL) {
                    updateHandle(currentNode, ((node_t*)currentNode->nodePtr)->parent);
                    currentChild = 0;
                }
            break;
            case 'd':  //down
                if (((node_t*)currentNode->nodePtr)->child[currentChild] != NULL) {
                    updateHandle(currentNode, ((node_t*)currentNode->nodePtr)->child[currentChild]);
                    currentChild = 0;
                }
            break;
            case 'l':  //left
                if (currentChild > 0)
                    currentChild--;
            break;
            case 'r':  //right
                if (currentChild < ((node_t*)currentNode->nodePtr)->children-1)
                    currentChild++;
            break;
            case 'g':  //get nodes matching path
            {
                char searchPath[MAXCHARSPATH];
                printf("\nPath to resource(s): ");
                scanf("%s", searchPath);
                path_t responsePaths[MAXFOUNDNODES];
                struct NodeHandle_t foundNodes[MAXFOUNDNODES];
                int foundResponses = VSSGetNodes(searchPath, &rootNode, MAXFOUNDNODES, responsePaths, foundNodes);
                printf("\nNumber of elements found=%d\n", foundResponses);
                for (int i = 0 ; i < foundResponses ; i++) {
                    printf("Found node name=%s\n", ((node_t*)foundNodes[i].nodePtr)->name);
                    printf("Found path=%s\n", responsePaths[i]);
                }
            }
            break;
            case 'h':  //help
                printf("\nTo traverse the tree, 'u'(p)p/'d'(own)/'l'(eft)/'r'(ight)/g(et)/h(elp), or any other to quit\n");
            break;
            default:
                return 0;
        }  //switch
        printf("\nNode name = %s, Node type = %s, Node children = %d\nNode description = %s\n", ((node_t*)currentNode->nodePtr)->name, getTypeName(((node_t*)currentNode->nodePtr)->type), ((node_t*)currentNode->nodePtr)->children, ((node_t*)currentNode->nodePtr)->description);
        if (((node_t*)currentNode->nodePtr)->children > 0)
            printf("Node child[%d]=%s\n", currentChild, ((node_t*)currentNode->nodePtr)->child[currentChild]->name);
        if (((node_t*)currentNode->nodePtr)->type == ELEMENT) {
            // as all objectdefinitions start with objectType, this is ok. But only for reading the objectType
            mediaCollectionObject_t* ptr2 = (mediaCollectionObject_t*)((element_node_t*)currentNode->nodePtr)->uniqueObject; 
            printf("Node object type=%d\n", ptr2->objectType);
            if (ptr2->objectType == MEDIACOLLECTION) {
                for (int i = 0 ; i < ptr2->numOfItems ; i++) {
                    printf("Items ref[%d]=%s\n", i, ptr2->items[i]);
                }
            }
        }
    } //while


    return 0;
} // main


