#include <iostream>
#include "DSM2.h"

using namespace std;

int main(int argc, char const *argv[])
{
    DSM2 dsm;
    while( true )
    {
        dsm.update();
        cout << "THRO " << dsm.values[0] << "\tAILE " << dsm.values[1] << "\tELEV " << dsm.values[2] << "\tRUDD " << dsm.values[3] << "\tGEAR " << dsm.values[4] << "\tAUX1 " << dsm.values[5] << endl;
    }
    return 0;
}