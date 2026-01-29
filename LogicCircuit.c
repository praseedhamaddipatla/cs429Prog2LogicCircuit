#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

//define types so do not have to deal with strings
typedef enum DeviceType{
    INPUT,
    OUTPUT,
    AND,
    OR,
    NOT,
    XOR
} DeviceType;

//struct to store characteristics of each device
typedef struct Device{
    DeviceType type;
    int id;

    int* inputs;
    int numIn;

    int* outputs;
    int numOut;

    int state;
    bool done;
} Device;

//keep track of all devices in file
typedef struct Circuit{
    Device* devices;
    int numDevices;

    int inputIDs;
    int numInputIDs;

    int outputIDs;
    int numOutputIDs;
} Circuit;

//extract device type from text
static DeviceType getType(const char *line) { //error when not const bc strstr
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
    return NULL; //do null check in method where this is called
    
}

//remove new lines and other white space
static void trim(char* line){
    int i=0;
    while(line[i]!='\0'){
        if(line[i]=='\n'){
            line[i]='\0';
            return;
        }
        i++;
    }
}

static int* parseValues(const char *line, int *count){
    int *list;
    int capacity = 0;
    *count=0;

    //find where numbers start
    int i=0;
    while(line[i]!='=' && line[i]!='\0'){
        i++; //until =
    }
    i++;
    
    //start reading numbers
    while(line[i] != '\0'){
        int num = 0;
        while(line[0]>='0' && line[i]<='9'){
            num= num*10 + (line[i]-'0');
            i++;
        }

        if(*count>=capacity){
            if(capacity==0){
                capacity=4;
            }
            else{
                capacity*=2;
            }
            list=malloc(capacity*sizeof(int));
        }

        
    }
}

