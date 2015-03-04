#include "DSM2.h"

using namespace std;

int main( int argc, char* argv[] )
{
    DSM2 dsm();

    while( true )
    {
        cout << "Thro" << dsm.values[0] <<
                "Aile" << dsm.values[1] << 
                "Elev" << dsm.values[2] << 
                "Rudd" << dsm.values[3] << 
                "Gear" << dsm.values[4] << 
                "Aux1" << dsm.values[5] << 
                "FL" << dsm.frameLoss << endl;

        dsm.update( true );
    }
}