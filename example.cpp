#include "sugarless.hpp"
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
    sugarless::Command git(argv[0]),clone("clone");
    clone.flag('M',"M",NULL,true,"128");
    git.flag('w', "w", "overwrite")
        .flag('o', "o", "output", true, "./a.out")
        .flag('M', "M", "max", true)
        .flag('m', "m", "min", true)
        .set(clone);

    git.parse(argc,argv);

    cout << git << endl;

    if(git.has("clone")){
        cout << "has" << endl;
        if (clone.has('M')) {
            cout << "clone max:" << endl;
            cout << clone.get('M') << endl;
        }
    }


    return 0;
}
