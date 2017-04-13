#include <iostream>
#include "nstd_member_detector.h"

struct T{
    void Member1();
};

CREATE_MEMBER_DETECTOR(Member1);
CREATE_MEMBER_DETECTOR(Member2);

// main function
int nstd_detector_example(void)
{
    using namespace std;
    if(DETECT_MEMBER(T,Member1))
        cout<<"T::Member1 is found!"<<endl;
    else
        cout<<"T::Member1 is not found!"<<endl;
    
    cout<<endl;
    if(DETECT_MEMBER(T,Member2))
        cout<<"T::Member2 is found!"<<endl;
    else
        cout<<"T::Member2 is not found!"<<endl;
    
    return 0;
}


////////////////////// program output //////////////////////

// T::Member1 is found!

// T::Member2 is not found!
