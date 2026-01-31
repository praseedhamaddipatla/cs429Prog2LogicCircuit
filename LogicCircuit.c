#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// dynamic array
typedef struct Array {
    int* data;
    int size;
    int capacity;
} Array;

Array newArray() {
    Array arr;
    arr.data = NULL;
    arr.size = 0;
    arr.capacity = 0;
    return arr;
}

void addToArray(Array* arr, int value) {
    int newCap;
    //grow when needed
    if (arr->size >= arr->capacity) {
        if(arr->capacity==0){
            newCap=4;
        }
        else{
            newCap=arr->capacity * 2;
        }
        int *newData = realloc(arr->data, newCap * sizeof(int));
        if (newData == NULL) {
            printf("Memory allocation failed\n");
            return;
        }

        arr->data = newData;
        arr->capacity = newCap;
    }

    arr->data[arr->size++] = value;
}

void sortArray(Array *arr){
    for (int i = 0; i < arr->size - 1; i++) {
        for (int j = i + 1; j < arr->size; j++) {
            if (arr->data[j] < arr->data[i]) {
                int temp = arr->data[i];
                arr->data[i] = arr->data[j];
                arr->data[j] = temp;
            }
        }
    }
}

//define types
typedef enum DeviceType{
    INPUT,
    OUTPUT,
    AND,
    OR,
    NOT,
    XOR,
    INVALID
} DeviceType;

//struct to store characteristics of each device
typedef struct Device{
    DeviceType type;
    int id;

    Array inputs;
    int numIn;

    Array outputs;
    int numOut;

    int state;
    bool done;
} Device;

Device newDevice(){
    Device dev;
    dev.type=INVALID;
    dev.inputs=newArray();
    dev.outputs=newArray();
    dev.numIn=0;
    dev.numOut=0;
    dev.state=0;
    dev.done=false;
    return dev;
}

//keep track of all devices in file
typedef struct Circuit{
    Device *devices;
    int numDevices;

    Array inIDs;
    Array outIDs;
} Circuit;

Circuit newCircuit(){
    Circuit circ;
    circ.devices=NULL;
    circ.numDevices=0;
    circ.inIDs=newArray();
    circ.outIDs=newArray();
    return circ;
}

void addDevice(Circuit *c, Device *d){
    c->devices = realloc(c->devices, (d->id + 1) * sizeof(Device));
    c->devices[d->id] = *d;

    if (d->id >= c->numDevices) {
        c->numDevices = d->id + 1;
    }

    if (d->type == INPUT) {
        addToArray(&c->inIDs, d->id);
    }

    if (d->type == OUTPUT) {
        addToArray(&c->outIDs, d->id);
    }
}

void freeCircuit(Circuit *c) {
    // free internal arrays
    for (int i = 0; i < c->numDevices; i++) {
        free(c->devices[i].inputs.data);
        free(c->devices[i].outputs.data);
    }

    // free device array
    free(c->devices);

    // free input/output ID arrays
    free(c->inIDs.data);
    free(c->outIDs.data);
}

//extract device type from text
static DeviceType parseType(const char *line) { //error when not const bc strstr
    if (strcmp(line, "Type=INPUT") == 0) {
        return INPUT;
    }
    if (strcmp(line, "Type=OUTPUT") == 0) {
        return OUTPUT;
    }
    if (strcmp(line, "Type=AND") == 0) {
        return AND;
    }
    if (strcmp(line, "Type=OR") == 0) {
        return OR;
    }
    if (strcmp(line, "Type=NOT") == 0) {
        return NOT;
    }
    if (strcmp(line, "Type=XOR") == 0) {
        return XOR;
    }

    fprintf(stderr, "Invalid device type: %s\n", line);
    return INVALID; //check in method where this is called
    
}

//remove new lines and other white space
void trim(char* line){
    int read = 0;
    int write = 0;

    while (line[read] != '\0') {
        // copy non-whitespace characters
        if (line[read] != ' ' &&
            line[read] != '\t' &&
            line[read] != '\n') 
        {
            line[write] = line[read];
            write++;
        }
        read++;
    }

    line[write] = '\0';
}

Array parseValues(const char *line){
    Array arr = newArray();
    int i = 0;

    // keep going until =
    while (line[i] != '=' && line[i] != '\0') {
        i++;
    }
    i++;

    // parse integers
    while (line[i] != '\0') {
        int value = 0;

        while (line[i] >= '0' && line[i] <= '9') {
            value = value * 10 + (line[i] - '0');
            i++;
        }
        addToArray(&arr, value);

        //skip commas
        if (line[i] == ',') {
            i++;
        }
    }
    return arr;
}

Device parseStanza(FILE *f){
    Device dev = newDevice();

    char *line = NULL;
    size_t len = 0;
    while(getline(&line, &len, f)!=-1){
        trim(line); //remove all whitespace

        //get each characteristic & update the device
        if (strcmp(line, "{") == 0) {
            continue;
        }
        if (strcmp(line, "}") == 0) {
            break;
        }
        if (strncmp(line, "Type=", 5) == 0) {
            dev.type = parseType(line);
        }
        else if (strncmp(line, "UniqueID=", 9) == 0) {
            dev.id = atoi(line + 9);
        }
        else if (strncmp(line, "Input=", 6) == 0) {
            dev.inputs = parseValues(line);
            dev.numIn=dev.inputs.size;
        }
        else if (strncmp(line, "Output=", 7) == 0) {
            dev.outputs = parseValues(line);
            dev.numOut=dev.outputs.size;
        }
    }
    free(line);
    return dev;
}

Circuit parseFile(const char *file){
    Circuit circ = newCircuit();

    FILE *f = fopen(file, "r");
    if(f==NULL || !f){
        fprintf(stderr, "Problem opening input file.");
        return circ; //check for NULL when this method is called
    }

    char *line=NULL;
    size_t len = 0;
    while(getline(&line, &len, f)!=-1){
        trim(line);

        //indicates start of new stanza
        if(strcmp(line, "{")==0){
            Device dev = parseStanza(f);
            addDevice(&circ, &dev);
        }
    }
    free(line);
    fclose(f);
    return circ;
}

int dfsEval(Circuit c, int id){
    Device *d = &c.devices[id];

    if(d->done){
        return d->state;
    }

    int result;
    switch (d->type){

        case INPUT:
            result = d->state;
            break;

        case OUTPUT:
            result = dfsEval(c, d->inputs.data[0]);
            break;

        case AND:
            result=1;
            for (int i = 0; i < d->numIn; i++) {
                result = result & dfsEval(c, d->inputs.data[i]);
            }
            break;
            
        case OR:
            result = 0;
                for (int i = 0; i < d->numIn; i++) {
                    result = result | dfsEval(c, d->inputs.data[i]);
                    if(result==1){
                        break;
                    }
                }
                break;

        case NOT:
            result = !dfsEval(c, d->inputs.data[0]);
            break;

        case XOR:
            result=0;
            for (int i = 0; i < d->numIn; i++) {
                result = result ^ dfsEval(c, d->inputs.data[i]);
            }
            break;
        
        default:
            fprintf(stderr, "Undefined device type. Parsing error.\n");
            result =0;
    }
    d->state=result;
    d->done=true;
    return result;
}

void generateRows(Circuit *c, int idx){
    
    //base case
    if(idx==c->inIDs.size){

        //reset dfs tracking
        for(int i=0; i<c->numDevices; i++){
            if(c->devices[i].type!=INPUT){
                c->devices[i].done=0;
            }
        }

        for(int i=0; i<c->inIDs.size; i++){
            printf("%d ", c->devices[c->inIDs.data[i]].state);
        }

        printf("| ");

        for(int i=0; i<c->outIDs.size; i++){
            printf("%d ", dfsEval(*c, c->outIDs.data[i]));
        }

        printf("\n");
        return;
    }

    //recursive case
    int id=c->inIDs.data[idx];
    for(int val=0; val<=1; val++){
        c->devices[id].state=val;
        c->devices[id].done=true;
        generateRows(c, idx+1);
    }
}

void printTable(Circuit c){
    //sort table
    sortArray(&c.inIDs);
    sortArray(&c.outIDs);

    //print header
    for (int i=0; i<c.inIDs.size; i++){
        printf("%d ", c.inIDs.data[i]);
    }

    printf("| ");

    for(int i=0; i<c.outIDs.size; i++){
        printf("%d ", c.outIDs.data[i]);
    }

    printf("\n");
    generateRows(&c, 0);
}

int testmain(int argc, char *argv[]){
    if(argc==2){
        Circuit circ = parseFile(argv[1]);
        printTable(circ);
        freeCircuit(&circ);
        return 0;
    }
    fprintf(stderr, "Invalid number of inputs.");
    return 1;
}

int main(int argc, char *argv[]){
    testmain(argc, argv);
}
