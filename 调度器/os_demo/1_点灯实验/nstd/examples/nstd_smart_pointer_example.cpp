#include <iostream>
#include "nstd_smart_pointer.h"
using namespace nstd;
using namespace std;

struct Base{
    virtual void fun() const
    { cout<<"Base::fun() called, this pointer: "<<this<<endl; }
    virtual ~Base(){}
// uncomment this and check the result ------------
//     virtual bool smpi_is_heap_obj(){
//         cout<<"query function smpi_is_heap_obj() is called"<<endl;
//         return true;
//     }
};

/* class to track the construction and destruction of objects */
struct Test:public Base{
    Test(){ cout<<"Test() at "<<this<<endl; }
    virtual ~Test(){ cout<<"~Test() at "<<this<<endl; }
    virtual void fun() const
    { cout<<"Test::fun() called, this pointer: "<<this<<endl; }
};

void require_ownership(obj_ownership<Test> o){
    /**< Note that you must not leave any floating ownership!
    ownership hold by o must be taken! You can use the follow
    statement to take the ownership. */
    obj_ptr<Test> p(o);
    cout<<"in require_ownership()."<<endl
        <<"object will be deleted automaticly in this function"<<endl;
}

// This usage is recommended
// The const qualifier prevent modifying the p pointer itself.
// But you may be still allowed to modify the object own by p
void call_fun(const obj_ptr<Base>& p){
    // call virtual function is ok
    p->fun();
}

// function that returns obj_ownership, 
// this can be use to initialize an obj_ptr
obj_ownership<Test> get_obj(){
    return obj_ptr<Test>( NEW_OBJ( Test, () )).give();
}

// main function
int nstd_smart_pointer_example()
{
    // initialize an obj_ptr
    obj_ptr<Test> pt NEW_OBJ(
        Test /* type */ ,() /* constructor argument list */ 
    );
    // if you do wanna call new by yourself,
    // use: pt.receive_heap_obj(new Test());
    if(pt) /* to check whether the pt is valid */
        cout<<"Test object is successfully allocated."<<endl;
    else /* you may want to call abort() or throw an exception here in practice */
        cout<<"Test object is unsuccessfully allocated."<<endl;
    
    cout<<endl; // test 1
    // to transfer ownership, you have to make it explicitly 
    require_ownership(pt.give_ownership());
    cout<<"out require_ownership() function."<<endl<<endl;
    
    cout<<endl; // test 2
    pt = NEW_OBJ(Test,());
    // there's no direct convertion from derived class's obj_ptr to base class's
    obj_ptr<Base> pb(pt.give());
    call_fun(pb);
    // you may steal it back, use pt.steal_ownership(pb); in this case
    cout<<"call_fun has returned, now clean pb :"<<endl;
    pb.clean(); /* you can manually delete the object */
    
    cout<<endl; // test 3
    pt = NEW_OBJ(Test,());
    // Another way to convert to obj_ptr<base>
    // Note that You GIVE the OWNERSHIP to a tempory object at the same time.
    // The object will be deleted after the full-expression is evaluated.
    call_fun(pt.give());    
    
    cout<<"done"<<endl;
    while(1);
}


// 程序输出:

// Test() at 20002520
// Test object is successfully allocated.

// in require_ownership().
// object will be deleted automaticly in this function
// ~Test() at 20002520
// out require_ownership() function.

// Test() at 20002520
// Test::fun() called, this pointer: 20002520
// call_fun has returned, now clean pb :
// ~Test() at 20002520

// Test() at 20002520
// Test::fun() called, this pointer: 20002520
// ~Test() at 20002520
// done


// 程序输出, 当 11-14 行的代码使能之后:

// Test() at 20002520
// Test object is successfully allocated.

// in require_ownership().
// object will be deleted automaticly in this function
// query function smpi_is_heap_obj() is called
// ~Test() at 20002520
// out require_ownership() function.

// Test() at 20002520
// Test::fun() called, this pointer: 20002520
// call_fun has returned, now clean pb :
// query function smpi_is_heap_obj() is called
// ~Test() at 20002520

// Test() at 20002520
// Test::fun() called, this pointer: 20002520
// query function smpi_is_heap_obj() is called
// ~Test() at 20002520
// done
