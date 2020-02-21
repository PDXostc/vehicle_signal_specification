/**
* (C) 2020 Geotab Inc
* (C) 2018 Volvo Cars
*
* All files and artifacts in this repository are licensed under the
* provisions of the license provided by the LICENSE file in this repository.
*
* 
* Parser utilities for a native format VSS tree.
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include "vssparserutilities.h"
#include "nativeCnodeDef.h"

FILE* treeFp;
int currentDepth;
int maxTreeDepth;

void initTreeDepth() {
    currentDepth = 0;
    maxTreeDepth = 0;
}

void updateTreeDepth(int count) {
    if (count > 0) {
        currentDepth++;
        if (currentDepth > maxTreeDepth)
            maxTreeDepth++;
    } else {
        currentDepth--;
    }
}

void printTreeDepth() {
    printf("Max depth of VSS tree = %d\n", maxTreeDepth);
}

void readCommonPart(common_node_data_t* commonData, char** name, char**uuid, char** descr) {
    fread(commonData, sizeof(common_node_data_t), 1, treeFp);
    *name = (char*) malloc(sizeof(char)*(commonData->nameLen+1));
    *uuid = (char*) malloc(sizeof(char)*(commonData->uuidLen+1));
    *descr = (char*) malloc(sizeof(char)*(commonData->descrLen+1));
    fread(*name, sizeof(char)*commonData->nameLen, 1, treeFp);
    (*name)[commonData->nameLen] = '\0';
printf("Name = %s\n", *name);
    fread(*uuid, sizeof(char)*commonData->uuidLen, 1, treeFp);
    (*uuid)[commonData->uuidLen] = '\0';
printf("UUID = %s\n", *uuid);
    fread(*descr, sizeof(char)*commonData->descrLen, 1, treeFp);
    *((*descr)+commonData->descrLen) = '\0';
printf("Description = %s\n", *descr);
printf("Children = %d\n", commonData->children);
}

void copyData(node_t* node, common_node_data_t* commonData, char* name, char* uuid, char* descr) {
    node->nameLen = commonData->nameLen;
    node->name = (char*) malloc(sizeof(char)*(node->nameLen+1));
    strncpy(node->name, name, commonData->nameLen);
    node->name[commonData->nameLen] = '\0';
    node->type = commonData->type;
    node->uuidLen = commonData->uuidLen;
    node->uuid = (char*) malloc(sizeof(char)*(node->uuidLen+1));
    strncpy(node->uuid, uuid, commonData->uuidLen);
    node->uuid[commonData->uuidLen] = '\0'; 
    node->validate = commonData->validate;
    node->descrLen = commonData->descrLen;
    node->description = (char*) malloc(sizeof(char)*(node->descrLen+1));
    strncpy(node->description, descr, commonData->descrLen);
    node->description[commonData->descrLen] = '\0';
    node->children = commonData->children;
}

/*
  Data order on file: 
  - Common part
  - Name
  - Description
  if (node_t)
    - node_t specific
	* min/max/unit/enum
*/
struct node_t* traverseAndReadNode(struct node_t* parentPtr) {
if (parentPtr != NULL)
  printf("parent node name = %s\n", parentPtr->name);
    common_node_data_t* common_data = (common_node_data_t*)malloc(sizeof(common_node_data_t));
    if (common_data == NULL) {
        printf("traverseAndReadNode: 1st malloc failed\n");
        return NULL;
    }
    updateTreeDepth(1);
    char* name;
    char* uuid;
    char* descr;
    readCommonPart(common_data, &name, &uuid, &descr);
    node_t* node = NULL;
printf("Type=%d\n",common_data->type);

    node_t* node2 = (node_t*) malloc(sizeof(node_t));
    node2->parent = parentPtr;
    copyData((node_t*)node2, common_data, name, uuid, descr);
    if (node2->children > 0)
        node2->child = (node_t**) malloc(sizeof(node_t**)*node2->children);
    fread(&(node2->datatype), sizeof(int), 1, treeFp);
    fread(&(node2->min), sizeof(int), 1, treeFp);
    fread(&(node2->max), sizeof(int), 1, treeFp);
    fread(&(node2->unitLen), sizeof(int), 1, treeFp);
    node2->unit = NULL;
    if (node2->unitLen > 0) {
        node2->unit = (char*) malloc(sizeof(char)*(node2->unitLen+1));
        fread(node2->unit, sizeof(char)*node2->unitLen, 1, treeFp);
        node2->unit[node2->unitLen] = '\0';
    }
if (node2->unitLen > 0)
    printf("Unit = %s\n", node2->unit);

    fread(&(node2->numOfEnumElements), sizeof(int), 1, treeFp);
    if (node2->numOfEnumElements > 0) {
        node2->enumeration = (enum_t*) malloc(sizeof(enum_t)*node2->numOfEnumElements);
        fread(node2->enumeration, sizeof(enum_t)*node2->numOfEnumElements, 1, treeFp);
    }
for (int i = 0 ; i < node2->numOfEnumElements ; i++)
  printf("Enum[%d]=%s\n", i, (char*)node2->enumeration[i]);

    fread(&(node2->functionLen), sizeof(int), 1, treeFp);
    node2->function = NULL;
    if (node2->functionLen > 0) {
        node2->function = (char*) malloc(sizeof(char)*(node2->functionLen+1));
        fread(node2->function, sizeof(char)*node2->functionLen, 1, treeFp);
        node2->function[node2->functionLen] = '\0';
    }
if (node2->functionLen > 0)
    printf("Function = %s\n", node2->function);
            node = (node_t*)node2;

    free(common_data);
    free(name);
    free(uuid);
    free(descr);
printf("node->children = %d\n", node->children);
    int childNo = 0;
    while(childNo < node->children) {
        node->child[childNo++] = traverseAndReadNode(node);
    }
    updateTreeDepth(-1);
    return node;
}

long VSSReadTree(char* filePath) {
    treeFp = fopen(filePath, "r");
    if (treeFp == NULL) {
        printf("Could not open file for reading tree data\n");
        return 0;
    }
    initTreeDepth();
    intptr_t root = (intptr_t)traverseAndReadNode(NULL);
    printTreeDepth();
    fclose(treeFp);
    return (long)root;
}

typedef struct SearchContext_t {
    long rootNode;
    int maxFound;
    bool leafNodesOnly;
    int maxDepth;
    char* searchPath;
    path_t matchPath;
    int currentDepth;  // depth in tree from rootNode, and also depth (in segments) in searchPath
    int speculationIndex;  // inc/dec when pathsegment in focus is wildcard
    int speculativeMatches[20];  // inc when matching node is saved
    int maxValidation;
    int numOfMatches;
    searchData_t* searchData;
} SearchContext_t;

void pushPathSegment(char* name, SearchContext_t* context) {
    if (context->currentDepth > 0) {
        strcat(context->matchPath, ".");
    }
    strcat(context->matchPath, name);
}

void popPathSegment(SearchContext_t* context) {
    char* delim = strrchr(context->matchPath, '.');
    if (delim == NULL) {
        context->matchPath[0] = 0;
    } else {
        *delim = 0;
    }
}

void incDepth(long thisNode, SearchContext_t* context) {
//printf("incDepth()\n");
    pushPathSegment(getName(thisNode), context);
    context->currentDepth++;
}

char getPathSegmentBuf[100];  // modified by getPathSegment() only
char* getPathSegment(int offset, SearchContext_t* context) {
    char* frontDelimiter = &(context->searchPath[0]);
    char* endDelimiter;
    for (int i = 1 ; i < context->currentDepth + offset ; i++) {
        frontDelimiter = strchr(frontDelimiter+1, '.');
        if (frontDelimiter == NULL) {
            if (context->searchPath[strlen(context->searchPath)-1] == '*' && context->currentDepth < context->maxDepth) {
                return "*";
            } else {
                return "";
            }
        }
    }
    endDelimiter = strchr(frontDelimiter+1, '.');
    if (endDelimiter == NULL) {
        endDelimiter = &(context->searchPath[strlen(context->searchPath)]);
    }
    if (frontDelimiter[0] == '.') {
        frontDelimiter++;
    }
    strncpy(getPathSegmentBuf, frontDelimiter, (int)(endDelimiter-frontDelimiter));
    getPathSegmentBuf[(int)(endDelimiter-frontDelimiter)] = 0;
    return getPathSegmentBuf;
}

int countSegments(char* path) {
    int i;
    if (strlen(path) == 0) {
        return 0;
    }
    char* delim = strchr(path, '.');
    for (i = 0 ; i < 100 ; i++) {
        if (delim == NULL) {
            break;
        }
        delim = strchr(delim+1, '.');
    }
    return i+1;
}

bool compareNodeName(char* nodeName, char* pathName) {
//printf("compareNodeName(): nodeName=%s, pathName=%s\n", nodeName, pathName);
    if (strcmp(nodeName, pathName) == 0 || strcmp(pathName, "*") == 0) {
        return true;
    }
    return false;
}

int saveMatchingNode(long thisNode, SearchContext_t* context, bool* done) {
    if (strcmp(getPathSegment(0, context), "*") == 0) {
        context->speculationIndex++;
    }
    if (getValidation(thisNode) > context->maxValidation) {
        context->maxValidation = getValidation(thisNode);  // TODO handle speculative setting
    }
    if (getType(thisNode) != BRANCH || context->leafNodesOnly == false) {
        strcpy(context->searchData[context->numOfMatches].responsePaths, context->matchPath);
        context->searchData[context->numOfMatches].foundNodeHandles = thisNode;
        context->numOfMatches++;
        if (context->speculationIndex >= 0) {
            context->speculativeMatches[context->speculationIndex]++;
        }
    }
    if (getNumOfChildren(thisNode) == 0 || context->currentDepth == context->maxDepth) {
        *done = true;
    } else {
        *done = false;
    }
    if (context->speculationIndex >= 0 && ((getNumOfChildren(thisNode) == 0 && context->currentDepth >= countSegments(context->searchPath)) || context->currentDepth == context->maxDepth)) {
        return 1;
    }
    return 0;
}

/**
* decDepth() shall reverse speculative wildcard matches that have failed, and also decrement currentDepth.
**/
void decDepth(int speculationSucceded, SearchContext_t* context) {
//printf("decDepth():speculationSucceded=%d\n", speculationSucceded);
    if (context->speculationIndex >= 0 && context->speculativeMatches[context->speculationIndex] > 0) {
        if (speculationSucceded == 0) {  // it failed so remove a saved match
            context->numOfMatches--;
            context->speculativeMatches[context->speculationIndex]--;
        }
    }
    if (strcmp(getPathSegment(0, context), "*") == 0) {
        context->speculationIndex--;
    }
    popPathSegment(context);
    context->currentDepth--;
}

int traverseNode(long thisNode, SearchContext_t* context) {
    int speculationSucceded = 0;

    incDepth(thisNode, context);
    if (compareNodeName(getName(thisNode), getPathSegment(0, context)) == true) {
//printf("after compareNodeName()\n");
        bool done;
        speculationSucceded = saveMatchingNode(thisNode, context, &done);
        if (done == false) {
            int numOfChildren = getNumOfChildren(thisNode);
            char* childPathName = getPathSegment(1, context);
            for (int i = 0 ; i < numOfChildren ; i++) {
                if (compareNodeName(getName(getChild(thisNode, i)), childPathName) == true) {
                    speculationSucceded += traverseNode(getChild(thisNode, i), context);
                }
            }
        }
    }
    decDepth(speculationSucceded, context);
    return speculationSucceded;
}

void initContext(SearchContext_t* context, char* searchPath, long rootNode, int maxFound, searchData_t* searchData, bool anyDepth, bool leafNodesOnly, int* validation) {
    context->searchPath = searchPath;
/*    if (anyDepth == true && context->searchPath[strlen(context->searchPath)-1] != '*') {
        strcat(context->searchPath, ".*");
    } */
    context->rootNode = rootNode;
    context->maxFound = maxFound;
    context->searchData = searchData;
    if (anyDepth == true) {
        context->maxDepth = 100;  //jan 2020 max tree depth = 8
    } else {
        context->maxDepth = countSegments(context->searchPath);
    }
    context->leafNodesOnly = leafNodesOnly;
    context->maxValidation = 0;
    context->currentDepth = 0;
    context->matchPath[0] = 0;
    context->numOfMatches = 0;
    context->speculationIndex = -1;
    for (int i = 0 ; i < 20 ; i++) {
        context->speculativeMatches[i] = 0;
    }
}

int VSSSearchNodes(char* searchPath, long rootNode, int maxFound, searchData_t* searchData, bool anyDepth, bool leafNodesOnly, int* validation) {
//    intptr_t root = (intptr_t)rootNode;
    struct SearchContext_t searchContext;
    struct SearchContext_t* context = &searchContext;

    initContext(context, searchPath, rootNode, maxFound, searchData, anyDepth, leafNodesOnly, validation);
    traverseNode(rootNode, context);
    if (validation != NULL) {
        *validation = context->maxValidation;
    }
    return context->numOfMatches;
}

void writeCommonPart(struct node_t* node) {
    common_node_data_t* commonData = (common_node_data_t*)malloc(sizeof(common_node_data_t));
    commonData->nameLen = node->nameLen;
    commonData->type = node->type;
    commonData->uuidLen = node->uuidLen;
    commonData->validate = node->validate;
    commonData->descrLen = node->descrLen;
    commonData->children = node->children;
    fwrite(commonData, sizeof(common_node_data_t), 1, treeFp);
    free(commonData);
    fwrite(node->name, sizeof(char)*node->nameLen, 1, treeFp);
    fwrite(node->uuid, sizeof(char)*node->uuidLen, 1, treeFp);
    fwrite(node->description, sizeof(char)*node->descrLen, 1, treeFp);
}

void traverseAndWriteNode(struct node_t* node) {
    if (node == NULL) //not needed?
        return;
    printf("Node name = %s, type=%d\n", node->name, node->type);
    writeCommonPart(node);
    fwrite(&(node->datatype), sizeof(int), 1, treeFp);
    fwrite(&(node->min), sizeof(int), 1, treeFp);
    fwrite(&(node->max), sizeof(int), 1, treeFp);
    fwrite(&(node->unitLen), sizeof(int), 1, treeFp);
    if (node->unitLen > 0)
        fwrite(node->unit, sizeof(char)*node->unitLen, 1, treeFp);
    fwrite(&(node->numOfEnumElements), sizeof(int), 1, treeFp);
    if (node->numOfEnumElements > 0) {
        fwrite(node->enumeration, sizeof(enum_t)*node->numOfEnumElements, 1, treeFp);
    }
    fwrite(&(node->functionLen), sizeof(int), 1, treeFp);
    if (node->functionLen > 0)
        fwrite(node->function, sizeof(char)*node->functionLen, 1, treeFp);

//printf("numOfEnumElements=%d, unitlen=%d, functionLen=%d\n", node->numOfEnumElements, node->unitLen, node->functionLen);
for (int i = 0 ; i < node->numOfEnumElements ; i++)
  printf("Enum[%d]=%s\n", i, (char*)node->enumeration[i]);
    int childNo = 0;
printf("node->children = %d\n", node->children);
    while(childNo < node->children) {
         traverseAndWriteNode(node->child[childNo++]);
    }
}

void VSSWriteTree(char* filePath, int rootHandle) {
    treeFp = fopen(filePath, "w");
    if (treeFp == NULL) {
        printf("Could not open file for writing tree data\n");
        return;
    }
    traverseAndWriteNode((struct node_t*)((intptr_t)rootHandle));
    fclose(treeFp);
}


// the intptr_t castings below are needed to avoid compiler warnings

long getParent(long nodeHandle) {
    return (long)((intptr_t)((node_t*)((intptr_t)nodeHandle))->parent);
}

int getNumOfChildren(long nodeHandle) {
    return (int)((intptr_t)((node_t*)((intptr_t)nodeHandle))->children);
}

long getChild(long nodeHandle, int childNo) {
    if (getNumOfChildren(nodeHandle) > childNo)
        return (long)((intptr_t)((node_t*)((intptr_t)nodeHandle))->child[childNo]);
    return 0;
}

nodeTypes_t getType(long nodeHandle) {
    return ((node_t*)((intptr_t)nodeHandle))->type;
}

nodeTypes_t getDatatype(long nodeHandle) {
    nodeTypes_t type = getType(nodeHandle);
    if (type != BRANCH)
        return ((node_t*)((intptr_t)nodeHandle))->datatype;
    return -1;
}

char* getName(long nodeHandle) {
    return ((node_t*)((intptr_t)nodeHandle))->name;
}

char* getUUID(long nodeHandle) {
    return ((node_t*)((intptr_t)nodeHandle))->uuid;
}

int getValidation(long nodeHandle) {
    return (int)((intptr_t)((node_t*)((intptr_t)nodeHandle))->validate);
}

char* getDescr(long nodeHandle) {
    return ((node_t*)((intptr_t)nodeHandle))->description;
}

int getNumOfEnumElements(long nodeHandle) {
    nodeTypes_t type = getType(nodeHandle);
    if (type != BRANCH)
        return ((node_t*)((intptr_t)nodeHandle))->numOfEnumElements;
    return 0;
}

char* getEnumElement(long nodeHandle, int index) {
    return ((node_t*)((intptr_t)nodeHandle))->enumeration[index];
}

char* getUnit(long nodeHandle) {
    nodeTypes_t type = getType(nodeHandle);
    if (type != BRANCH)
        return ((node_t*)((intptr_t)nodeHandle))->unit;
    return NULL;
}

char* getFunction(long nodeHandle) {
    nodeTypes_t type = getType(nodeHandle);
    if (type != BRANCH)
        return ((node_t*)((intptr_t)nodeHandle))->function;
    return NULL;
}

