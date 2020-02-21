/**
* (C) 2020 Geotab Inc
* (C) 2018 Volvo Cars
*
* All files and artifacts in this repository are licensed under the
* provisions of the license provided by the LICENSE file in this repository.
*
* 
* Definition of native format.
**/

#define MAXNAMELEN 28
#define MAXPROPNAMELEN 30
#define MAXPROPDESCRLEN 50
#define MAXPROPFORMATLEN 15
#define MAXPROPUNITLEN 15
#define MAXPROPVALUELEN 50


/**
* There are three different node structures defined below: 
 * node_t, rbranch_node_t, element_node_t
 * They have a common part, and a unique part.
 * The common part consists of:
 *   int nameLen;
 *   char* name;
 *   nodeTypes_t type;
 *   int descrLen;
 *   char* description;
 *   int children;
 *   struct node_t* parent;
 *   struct node_t** child;
 * This common part must be identical in all three structures, and be positioned before any unique parts.
 * When reading/writing the tree from/to file, the common part is read/written first, 
 * then from analysis of its data, further reading/writing is done.
 * The size of the common part is defined by sizeof(common_node_data_t), which contains the parameters above, except the pointers.
 * Data pointed to by the name and description pointers, with size defined by the associated length parameters, 
 * is read/written following the common part.
**/

typedef struct node_t {
    int nameLen;
    char* name;
    nodeTypes_t type;
    int uuidLen;
    char* uuid;
    int validate;
    int descrLen;
    char* description;
    int children;
    struct node_t* parent;
    struct node_t** child;
    nodeTypes_t datatype;
    int max;
    int min;
    int unitLen;
    char* unit;
    int numOfEnumElements;
    enum_t* enumeration;
    int functionLen;
    char* function;
} node_t;

typedef struct {
    int nameLen;
    nodeTypes_t type;
    int uuidLen;
    int validate;
    int descrLen;
    int children;
} common_node_data_t;

