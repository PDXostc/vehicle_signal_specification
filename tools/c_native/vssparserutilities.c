/**
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
#include "nativeCnodeDef.h"
#include "vssparserutilities.h"

void readCommonPart(common_node_data_t* commonData, char** name, char** descr) {
    fread(commonData, sizeof(common_node_data_t), 1, treeFp);
    *name = (char*) malloc(sizeof(char)*commonData->nameLen);
    *descr = (char*) malloc(sizeof(char)*commonData->descrLen);
    fread(*name, sizeof(char)*commonData->nameLen, 1, treeFp);
//(*name)[commonData->nameLen] = '\0';  //should not be needed
printf("Name = %s\n", *name);
    fread(*descr, sizeof(char)*commonData->descrLen, 1, treeFp);
//(*descr)[commonData->descrLen] = '\0';  //should not be needed
printf("Description length = %d\n", commonData->descrLen);
printf("Description = %s\n", *descr);
printf("Children = %d\n", commonData->children);
}

void copyData(node_t* node, common_node_data_t* commonData, char* name, char* descr) {
    node->nameLen = commonData->nameLen;
    node->name = (char*) malloc(sizeof(char)*node->nameLen);
    strncpy(node->name, name, commonData->nameLen);
    node->type = commonData->type;
    node->descrLen = commonData->descrLen;
    node->description = (char*) malloc(sizeof(char)*node->descrLen);
    strncpy(node->description, descr, commonData->descrLen);
    node->children = commonData->children;
}

int getObjectSize(objectTypes_t objectType) {
    switch (objectType) {
        case MEDIACOLLECTION:
            return sizeof(mediaCollectionObject_t);
        case MEDIAITEM:
            return sizeof(mediaItemObject_t);
    }
    return -1;
}

void readUniqueObjectRefs(objectTypes_t objectType, void* uniqueObject) {
    switch (objectType) {
        case MEDIACOLLECTION:
        {
            mediaCollectionObject_t* mediaCollectionObject = (mediaCollectionObject_t*) uniqueObject;
            mediaCollectionObject->items = (elementRef_t*) malloc(sizeof(elementRef_t)*mediaCollectionObject->numOfItems);
            fread(mediaCollectionObject->items, sizeof(elementRef_t)*mediaCollectionObject->numOfItems, 1, treeFp);
        }
        break;
        case MEDIAITEM:
        {
//            mediaItemObject_t* mediaItemObject = (mediaItemObject_t*) uniqueObject;
//            mediaCollectionObject->items = (elementRef_t*) malloc(sizeof(elementRef_t)*mediaCollectionObject->numOfItems);
//            fread(mediaCollectionObject->items, sizeof(elementRef_t)*mediaCollectionObject->numOfItems, 1, treeFp);
        }
        break;
        default:
            printf("readUniqueObjectRefs:unknown object type = %d\n", objectType);
        break;
    }
}

/*
  Data order on file: 
  - Common part
  - Name
  - Description
  if (node_t)
    - node_t specific
	* min/max/unit/enum
  if (rbranch_node_t)
    - rbranch specific
	* childType/numOfProperties/Properties
  if (element_node_t)
    - specific according to parent child type (mapped to struct def)
*/
struct node_t* traverseAndReadNode(struct node_t* parentPtr) {
if (parentPtr != NULL)
  printf("parent node name = %s\n", parentPtr->name);
    common_node_data_t* common_data = (common_node_data_t*)malloc(sizeof(common_node_data_t));
    if (common_data == NULL) {
        printf("traverseAndReadNode: 1st malloc failed\n");
        return NULL;
    }
    char* name;
    char* descr;
    readCommonPart(common_data, &name, &descr);
    node_t* node = NULL;
printf("Type=%d\n",common_data->type);
    switch (common_data->type) {
        case RBRANCH:
        {
            rbranch_node_t* node2 = (rbranch_node_t*) malloc(sizeof(rbranch_node_t));
            node2->parent = parentPtr;
            copyData((node_t*)node2, common_data, name, descr);
            if (common_data->children > 0)
                node2->child = (element_node_t**) malloc(sizeof(element_node_t**)*common_data->children);
            fread(&(node2->childTypeLen), sizeof(int), 1, treeFp);
            fread(&(node2->numOfProperties), sizeof(int), 1, treeFp);
            if (node2->numOfProperties > 0) {
                node2->propertyDefinition = (propertyDefinition_t*) malloc(sizeof(propertyDefinition_t*)*node2->numOfProperties);
                fread(node2->propertyDefinition, sizeof(propertyDefinition_t)*node2->numOfProperties, 1, treeFp);
            }
            node = (node_t*)node2;
        }
        break;
        case ELEMENT:
        {
            element_node_t* node2 = (element_node_t*) malloc(sizeof(element_node_t));
            node2->parent = parentPtr;
            copyData((node_t*)node2, common_data, name, descr);
            objectTypes_t objectType;
            fread(&objectType, sizeof(int), 1, treeFp);
            int objectSize = getObjectSize(objectType);
            if (objectSize > 0) {
                node2->uniqueObject = (void*) malloc(objectSize);
                *((int*)node2->uniqueObject) = objectType;
                fread(node2->uniqueObject+sizeof(int), objectSize-sizeof(int), 1, treeFp);
                readUniqueObjectRefs(objectType, node2->uniqueObject);
            }
            node = (node_t*)node2;
        }
        break;
        default:
        {
            node_t* node2 = (node_t*) malloc(sizeof(node_t));
            node2->parent = parentPtr;
            copyData((node_t*)node2, common_data, name, descr);
            if (node2->children > 0)
                node2->child = (node_t**) malloc(sizeof(node_t**)*node2->children);
            fread(&(node2->min), sizeof(int), 1, treeFp);
            fread(&(node2->max), sizeof(int), 1, treeFp);
            fread(node2->unit, sizeof(char)*MAXUNITLEN, 1, treeFp);
            fread(&(node2->numOfEnumElements), sizeof(int), 1, treeFp);
            if (node2->numOfEnumElements > 0) {
                node2->enumeration = (enum_t*) malloc(sizeof(enum_t)*node2->numOfEnumElements);
                fread(node2->enumeration, sizeof(enum_t)*node2->numOfEnumElements, 1, treeFp);
            }
for (int i = 0 ; i < node2->numOfEnumElements ; i++)
  printf("Enum[%d]=%s\n", i, (char*)node2->enumeration[i]);
            node = (node_t*)node2;
        }
        break;
    } //switch
    free(common_data);
    free(name);
    free(descr);
    int childNo = 0;
printf("node->children = %d\n", node->children);
    while(childNo < node->children) {
        node->child[childNo++] = traverseAndReadNode(node);
    }
    return node;
}

struct node_t* VSSReadTree() {
    return traverseAndReadNode(NULL);
}

char tmpNodeName[MAXNAMELEN];
char* getNodeName(int stepNo, char* path) {
printf("getNodeName:step=%d\n", stepNo);
    tmpNodeName[0] = '\0';
    char* ptr = strchr(path, '.');
    if (ptr != NULL) {
        if (stepNo == 0) {
            strncpy(tmpNodeName, path, (int)(ptr-path));
            tmpNodeName[(int)(ptr-path)] = '\0';
            return tmpNodeName;
        }
        char* front;
        for (int i = 0 ; i < stepNo ; i++) {
            front = ptr+1;
            ptr = strchr(ptr+1, '.');
            if (ptr == NULL) {
                if (i == stepNo-1) {
                    ptr =&path[strlen(path)];
                    break;
                } else
                    return tmpNodeName;
            }
        }
        strncpy(tmpNodeName, front, (int)(ptr-front));
        tmpNodeName[(int)(ptr-front)] = '\0';
    } else {
        if (stepNo == 0)
            strcpy(tmpNodeName, path);
    }
printf("getNodeName:step=%d, name=%s\n", stepNo, tmpNodeName);
    return tmpNodeName;
}

int getNumOfPathSteps(char* path) {
    int numofelements = 0;
    char* ptr= strchr(path, '.');
    if (ptr == NULL) {
        return 1;
    } else {
        numofelements++;
        while (ptr != NULL) {
            numofelements++;
            ptr= strchr(ptr+1, '.');
        }
printf("getNumOfPathSteps=%d\n", numofelements);
        return numofelements;
    }
}

void copySteps(char* newPath, char* oldPath, int stepNo) {
    char* ptr = strchr(oldPath, '.');
    for (int i = 0 ; i < stepNo-1 ; i++) {
        if (ptr != NULL)
            ptr = strchr(ptr+1, '.');
    }
    if (ptr != NULL) {
        strncpy(newPath, oldPath, (int)(ptr - oldPath));
        newPath[(int)(ptr - oldPath)] = '\0';
    }
}

struct node_t* stepToNextNode(struct node_t* ptr, int stepNo, char* searchPath, int maxFound, int* foundResponses, path_t* responsePaths, struct NodeHandle_t* foundNodePtrs) {
printf("ptr->name=%s, stepNo=%d, responsePaths[%d]=%s\n",ptr->name, stepNo, *foundResponses, responsePaths[*foundResponses]);
    if (*foundResponses >= maxFound-1)
        return NULL; // found buffers full
    char pathNodeName[MAXNAMELEN];
    strcpy(pathNodeName, getNodeName(stepNo, searchPath));
    if (stepNo == getNumOfPathSteps(searchPath)-1) { // at leave node, so save ptr and return success
        foundNodePtrs[*foundResponses].nodePtr = (void*)ptr;
        foundNodePtrs[*foundResponses].nodeType = ptr->type;
        return ptr;
    }
    strcpy(pathNodeName, getNodeName(stepNo+1, searchPath));  // get name of next step in path
    if (strcmp(pathNodeName, "*") != 0) {  // try to match with one of the children
        for (int i = 0 ; i < ptr->children ; i++) {
printf("ptr->child[i]->name=%s\n", ptr->child[i]->name);
            if (strcmp(pathNodeName, ptr->child[i]->name) == 0) {
                if (strlen(responsePaths[*foundResponses]) > 0) // always true?
                    strcat(responsePaths[*foundResponses], ".");
                strcat(responsePaths[*foundResponses], pathNodeName);
                return stepToNextNode(ptr->child[i], stepNo+1, searchPath, maxFound, foundResponses, responsePaths,  foundNodePtrs);
            }
        }
        return NULL;
    } else {  // wildcard, try to match with all children
        struct node_t* responsePtr = NULL;
        for (int i = 0 ; (i < ptr->children) && (*foundResponses < maxFound) ; i++) {
printf("Wildcard:ptr->child[%d]->name=%s\n", i, ptr->child[i]->name);
            strcat(responsePaths[*foundResponses], ".");
            strcat(responsePaths[*foundResponses], ptr->child[i]->name);
            struct node_t* ptr2 = stepToNextNode(ptr->child[i], stepNo+1, searchPath, maxFound, foundResponses, responsePaths,  foundNodePtrs);
            if (ptr2 == NULL) {
                copySteps(responsePaths[*foundResponses], responsePaths[*foundResponses], stepNo+1);
            } else {
                if (i < ptr->children && foundNodePtrs[*foundResponses].nodePtr != NULL) {
                    (*foundResponses)++;
                    copySteps(responsePaths[*foundResponses], responsePaths[*foundResponses-1], stepNo+1);
                } else
                    copySteps(responsePaths[*foundResponses], responsePaths[*foundResponses], stepNo+1);
                responsePtr = ptr2;
            }
        }
        return responsePtr;
    }
}


int VSSGetNodes(char* searchPath, struct NodeHandle_t* rootNode, int maxFound, path_t* responsePaths, struct NodeHandle_t* foundNodePtrs) {
    struct node_t* ptr = (node_t*)rootNode->nodePtr;
    int stepNo = 0;
    int foundResponses = 0;
    /* 
     * This is a workaround to the fact that with X multiple wildcards, 
     * there are "(X-1)*numberofrealresults" bogus results added.
     * It seems stepToNextNode returns non-NULL when it should not in those cases?
     * See NULL check in wildcard code in stepToNextNode. 
     */
    for (int i = 0 ; i < maxFound ; i++)
        foundNodePtrs[i].nodePtr = NULL;

    strcpy(responsePaths[0], ptr->name); // root node name needs to be written initially
    stepToNextNode(ptr, stepNo, searchPath, maxFound, &foundResponses, responsePaths, foundNodePtrs);

    if (strchr(searchPath, '*') == NULL)
        foundResponses++;

    return foundResponses;
}


