#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#ifndef TESTING

int testmain(int argc, char *argv[]);

void testHelper(char *args[], int argc, char *expected, char *testName)
{
    // redirect output
    freopen("output.txt", "w", stdout);

    testmain(argc, args);

    // restore output
    fflush(stdout);
    freopen("/dev/tty", "w", stdout);

    // read result
    FILE *f = fopen("output.txt", "r");
    if (!f)
    {
        printf("Could not open output.txt\n");
        return;
    }

    char result[10000] = {0};  // large buffer
    char line[200];
    while (fgets(line, sizeof(line), f))
    {
        strcat(result, line);
    }
    fclose(f);

    // remove trailing new line
    size_t len = strlen(result);
    if (len > 0 && result[len-1] == '\n')
    {
        result[len-1] = '\0';
    }

    if (strcmp(result, expected) == 0)
    {
        printf("PASS: %s\n", testName);
    }
    else
    {
        printf("FAIL: %s\n", testName);
        printf("    expected:\n%s\n", expected);
        printf("    actual:\n%s\n", result);
    }
}

void simpleCircuit()
{
    char *a1[] = {"test", "/u/psm2357/cs429/cs429Prog2LogicCircuit/simple.txt"};
    char *expected = "0 1 2 | 7\n0 0 0 | 1\n0 0 1 | 1\n0 1 0 | 0\n0 1 1 | 1\n1 0 0 | 0\n1 0 1 | 0\n1 1 0 | 0\n1 1 1 | 1";
    testHelper(a1, 2, expected, "Simple Circuit");
}

void complexCircuit()
{
    char *a1[] = {"test", "/u/psm2357/cs429/cs429Prog2LogicCircuit/complex.txt"};
    char *expected = "0 1 2 3 | 15 16 17 18 19\n\
0 0 0 0 | 1 1 0 0 1\n\
0 0 0 1 | 1 1 0 0 0\n\
0 0 1 0 | 1 1 1 1 1\n\
0 0 1 1 | 0 0 0 0 0\n\
0 1 0 0 | 1 0 0 0 0\n\
0 1 0 1 | 1 0 0 0 1\n\
0 1 1 0 | 1 0 0 0 0\n\
0 1 1 1 | 1 1 0 0 1\n\
1 0 0 0 | 1 1 0 0 1\n\
1 0 0 1 | 1 1 0 0 0\n\
1 0 1 0 | 1 1 1 1 1\n\
1 0 1 1 | 0 0 0 0 0\n\
1 1 0 0 | 0 0 0 0 0\n\
1 1 0 1 | 0 0 0 0 1\n\
1 1 1 0 | 0 0 0 0 0\n\
1 1 1 1 | 1 1 0 0 1";
    testHelper(a1, 2, expected, "Complex Circuit");
}

void weirdSpaceCircuit()
{
    char *a1[] = {"test", "/u/psm2357/cs429/cs429Prog2LogicCircuit/spaced.txt"};
    char *expected = "0 1 2 | 7\n0 0 0 | 1\n0 0 1 | 1\n0 1 0 | 0\n0 1 1 | 1\n1 0 0 | 0\n1 0 1 | 0\n1 1 0 | 0\n1 1 1 | 1";
    testHelper(a1, 2, expected, "Weirdly Spaced Circuit");
}

//is test is supposed to fail
void invalidCircuit(){
    char *a1[] = {"test", "/u/psm2357/cs429/cs429Prog2LogicCircuit/invalid.txt"};
    char *expected = "Error: Invalid device type\n";
    testHelper(a1, 2, expected, "Invalid Circuit");
}

int main()
{
    simpleCircuit();
    complexCircuit();
    weirdSpaceCircuit();
    invalidCircuit();

    printf("All tests finished.\n");
    return 0;
}

#endif