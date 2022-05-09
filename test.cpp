#include <iostream>

int main()
{
    using namespace std;
    short int r = 0x0201;
    char *out = (char*)&r;


    out[0]+=48;
    out[1]+=48;
//    out[2]+=48;
//    out[3]+=48;

    cout << out[0] << "\n";
    cout << out[1] << "\n";
 //   cout << out[2] << "\n";
 //   cout << out[3] << "\n";
}