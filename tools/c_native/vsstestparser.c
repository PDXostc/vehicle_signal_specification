/**
* (C) 2020 Geotab Inc
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
#include "vssparserutilities.h"

long currentNode;
long rootNode;
char* vspecfile;

char* getTypeName(nodeTypes_t type) {
    switch (type) { 
        case INT8:
                return "INT8";
        case UINT8:
                return "UINT8";
        case INT16:
                return "INT16";
        case UINT16:
                return "UINT16";
        case INT32:
                return "INT32";
        case UINT32:
                return "UINT32";
        case DOUBLE:
            return "DOUBLE";
        case FLOAT:
            return "FLOAT";
        case BOOLEAN:
            return "BOOLEAN";
        case STRING:
            return "STRING";
        case SENSOR:
            return "SENSOR";
        case ACTUATOR:
            return "ACTUATOR";
        case STREAM:
            return "STREAM";
        case ATTRIBUTE:
            return "ATTRIBUTE";
        case BRANCH:
            return "BRANCH";
        default:
            printf("getTypeName: unknown type\n");
            return "unknown";
        break;
    } // switch
}

void showNodeData(long currentNode, int currentChild) {
        printf("\nNode: name = %s, type = %s, uuid = %s, validate = %d, children = %d,\ndescription = %s\n", getName(currentNode), getTypeName(getType(currentNode)), getUUID(currentNode), getValidation(currentNode), getNumOfChildren(currentNode), getDescr(currentNode));
        if (getNumOfChildren(currentNode) > 0)
            printf("Node child[%d]=%s\n", currentChild, getName(getChild(currentNode, currentChild)));
//        for (int i = 0 ; i < getNumOfEnumElements(currentNode) ; i++)
//            printf("Enum[%d]=%s\n", i, getEnumElement(currentNode, i));
printf("#enums=%d\n", getNumOfEnumElements(currentNode));
        nodeTypes_t dtype = getDatatype(currentNode);
        if (dtype != -1)
            printf("Datatype = %d\n", dtype);
        char* tmp = getUnit(currentNode);
        if (tmp != NULL)
            printf("Unit = %s\n", tmp);
        tmp = getFunction(currentNode);
        if (tmp != NULL)
            printf("Function = %s\n", tmp);
}

int main(int argc, char** argv) {

    vspecfile = argv[1];
    rootNode = VSSReadTree(vspecfile);

    char traverse[10];
    printf("\nTo traverse the tree, 'u'(p)p/'d'(own)/'l'(eft)/'r'(ight)/g(et)/m(etadata subtree)/w(rite to file)/h(elp), or any other to quit\n");
    currentNode = rootNode;
    int currentChild = 0;
    while (true) {
        printf("\n'u'/'d'/'l'/'r'/'g'/'w'/'h', or any other to quit: ");
        scanf("%s", traverse);
        switch (traverse[0]) {
            case 'u':  //up
                if (getParent(currentNode) != 0) {
                    currentNode = getParent(currentNode);
                    currentChild = 0;
                }
                showNodeData(currentNode, currentChild);
            break;
            case 'd':  //down
                if (getChild(currentNode, currentChild) != 0) {
                    currentNode = getChild(currentNode, currentChild);
                    currentChild = 0;
                }
                showNodeData(currentNode, currentChild);
            break;
            case 'l':  //left
                if (currentChild > 0) {
                    currentChild--;
                }
                showNodeData(currentNode, currentChild);
            break;
            case 'r':  //right
                if (currentChild < getNumOfChildren(currentNode)-1) {
                    currentChild++;
                }
                showNodeData(currentNode, currentChild);
            break;
            case 'g':  //get nodes matching path
            {
                char searchPath[MAXCHARSPATH];
                printf("\nPath to resource(s): ");
                scanf("%s", searchPath);
                searchData_t searchData[MAXFOUNDNODES];
                int foundResponses = VSSSearchNodes(searchPath, rootNode, MAXFOUNDNODES, searchData, true, true, NULL);
                printf("\nNumber of elements found=%d\n", foundResponses);
                for (int i = 0 ; i < foundResponses ; i++) {
                    printf("Found node type=%s\n", getTypeName(getType((long)(&(searchData[i]))->foundNodeHandles)));
                    printf("Found path=%s\n", (char*)(&(searchData[i]))->responsePaths);
                }
            }
            break;
            case 'm':  //subtree metadata
            {
                char subTreePath[MAXCHARSPATH];
                printf("\nPath to subtree node: ");
                scanf("%s", subTreePath);
                int depth;
                printf("\nSubtree depth: ");
                scanf("%d", &depth);
                searchData_t searchData[MAXFOUNDNODES];
                int foundResponses = VSSSearchNodes(subTreePath, rootNode, MAXFOUNDNODES, searchData, false, false, NULL);
                long subtreeNode = (long)(&(searchData[foundResponses-1]))->foundNodeHandles;
                char subTreeRootName[MAXCHARSPATH];
                strcpy(subTreeRootName, getName((long)(&(searchData[foundResponses-1]))->foundNodeHandles));
                for (int i = 1 ; i < depth ; i++) {
                    strcat(subTreeRootName, ".*");
                }
                foundResponses = VSSSearchNodes(subTreeRootName, subtreeNode, MAXFOUNDNODES, searchData, false, false, NULL);
                printf("\nNumber of elements found=%d\n", foundResponses);
                for (int i = 0 ; i < foundResponses ; i++) {
                    printf("Node type=%s\n", getTypeName(getType((long)(&(searchData[i]))->foundNodeHandles)));
                    printf("Node path=%s\n", (char*)(&(searchData[i]))->responsePaths);
                    printf("Node validation=%d\n", getValidation((long)(&(searchData[i]))->foundNodeHandles));
                }
            }
            break;
            case 'h':  //help
                printf("\nTo traverse the tree, 'u'(p)p/'d'(own)/'l'(eft)/'r'(ight)/g(et)/m(etadata subtree)/w(rite to file)/h(elp), or any other to quit\n");
            break;
            case 'w':  //write to file
                VSSWriteTree(vspecfile, rootNode);
            break;
            default:
                return 0;
        }  //switch
    } //while


    return 0;
} // main


