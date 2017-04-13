#ifndef NSTD_SMART_POINTER_H
#define NSTD_SMART_POINTER_H

#include "nstd_assert.H"
#include "nstd_member_detector.h"

/** @defgroup nstd
  * @{
  */
namespace nstd{

    /** @defgroup smart_pointer
      * @brief 智能指针模块. 些模块的具体使用信息请参考
      * @ref struct obj_ptr.
      * @ingroup nstd
      * @{
      */


/** struct default_delete_policy
  * @brief 默认的类型操纵 policy, 通常用户不需要修改这个
  */
template<typename Type>
struct default_delete_policy{
    /*
    smpi_is_heap_obj
    smart pointer interface, is heap object
    */
    #define api_fun_name  smpi_is_heap_obj

    template<typename T, int>
    struct select{
        inline
        static void del(T* c_ptr){
            // the query function may be virtual, we cannot call an object's
            // virtual member function with a null pointer
            if((c_ptr!=0)&&(c_ptr->api_fun_name())){
                delete c_ptr;   // delete it only it is on the heap
            }
        }
    };
    template<typename T>
    struct select<T,0>{
        inline
        static void del(T* c_ptr){
            delete c_ptr;   // delete it only it is on the heap
        }
    };

    CREATE_MEMBER_DETECTOR(api_fun_name);

    enum{
        detect_result = DETECT_MEMBER(Type, api_fun_name)

    };
   inline static
   void delete_object_policy(Type* p){
         select<Type,detect_result>
            ::del(p);
    }
    #undef api_fun_name
};

/** @brief The obj_ownership type object is return by the obj_ptr::give()
  * function to transfer ownership. see @ref obj_ptr for more details.
  * In generral, do not use this type in your code unless you want to
  * transfer ownership from obj_ptr to another obj_ptr between function
  * calls.
  * @note the obj_ownership's ownership of object must be taken
  * before its destructor is called! The ownership cannot be floating
  * and must be given to its owner (obj_ptr) before it goes out of scope.
  */
template<class Type>
struct obj_ownership{
    typedef Type element_type;
    typedef element_type* pointer_type;
    private:
    mutable pointer_type c_ptr; /**< defined as mutable because you
                                have to take his ownership when this
                                obj_ownership is copying to another
                                ownership holder */
    public:
    inline obj_ownership():c_ptr(0){}
    explicit inline obj_ownership(Type* p):c_ptr(p){}

    /* owner ship must not be floating */
    inline ~obj_ownership() { DYNAMIC_ASSERT(c_ptr==0);}

    /** @brief Transfer ownership from one obj_ownership type object
      * to another obj_ownership type object.
      */
    inline
    obj_ownership& operator =(obj_ownership& o){ /* enough for copy constructor */
        c_ptr = o.c_ptr;
        o.c_ptr = 0;
        return *this;
    }

    obj_ownership& operator =(const obj_ownership& i){ /* enough for copy constructor */
        c_ptr = i.release();
        return *this;
    }

    /** @brief 转为其他类型的 obj_ownership */
    template<typename T>
    operator obj_ownership<T>() throw(){
        return  obj_ownership<T> (release());
    }

// from c++ 0x standard.
// If the class definition does not explicitly declare a copy constructor,
// there is no user-declared move construc-tor, and there is no user-declared
// move assignment operator, a copy constructor is implicitly declared as
// defaulted Such an implicit declaration is deprecated if the class has a
// user-declared copy assignment
// If the definition of a class X does not explicitly declare a move
// constructor, one will be implicitly declared as defaulted if and only if
// — X does not have a user-declared copy constructor,
// — X does not have a user-declared copy assignment operator,
// — X does not have a user-declared move assignment operator,
// — X does not have a user-declared destructor, and
// — the move constructor would not be implicitly defined as deleted.
// [ Note: When the move constructor is not implicitly declared or explicitly
// supplied, expressions that otherwise would have invoked the move constructor
// may instead invoke a copy constructor. —end note

//     inline
//     obj_ownership(obj_ownership& o){
//         c_ptr = o.c_ptr;
//         o.c_ptr = 0;
//         return *this;
//     }

    ////////////////////////////////////////////////////////
    element_type* release()const{
        element_type* temp = c_ptr;
        c_ptr = 0;
        return temp;
    }
};

/** @def NEW_OBJ(type,arglist)
  * @brief The NEW_OBJ macro is used to constructor a obj_ptr<T> object.
  * For example:
  * @code
  *     obj_ptr<int> p = NEW(int, (5) );
  *     // now *p == 5
  * @endcode
  * @param type the type of the object.
  * @param arglist the object's constructor argument list.
  * @note the expression cannot be used alone.
  * @code
  *     NEW_OBJ(int, (5)); // bad usage! this will generate a run time
  *                        // assertion failed!
  * @endcode
  */
#define NEW_OBJ(type,arglist)   (obj_ownership<type>(new type arglist))


/** @struct obj_ptr
  * @brief Smart pointer type to hold object on heap, it can delete object
  * automaticly when its destructor is called. This is a kind of single
  * owner smart pointer, which means that copy between obj_ptr is not
  * allow, but you are allow to transfer ownership between obj_ptr. The
  * obj_ptr owns the object pointed by it, and automaticly cleans the
  * object when the obj_ptr goes out of scope.
  * @param Type the type of object that is pointed by the smart pointer.
  * @param delete_policy When the obj_ptr is out of scope, the destructor
  * of the obj_ptr will be calld, and it behaves like the following
  * @code
  * ~obj_ptr(){
  * delete_policy::delete_object_policy(c_ptr).
  * }
  * @endcode
  * Normally, if you are using default_delete_policy<Type>, then the
  * delete_object_policy behaves like the following
  * @code
  * delete_object_policy(Type* c_ptr){
  *     delete c_ptr;
  * }
  * @endcode However, if your Type has a member function defined as
  * @code
  * virtual bool smpi_is_heap_obj()
  * @endcode
  * Then the delete_object_policy behaves like the following:
  * @code
  * delete_object_policy(Type* c_ptr){
  *     // the query function may be virtual, we cannot call an object's
  *     // virtual member function with a null pointer
  *     if((c_ptr!=0)&&(c_ptr->api_fun_name())){
  *         delete c_ptr;   // delete it only it is on the heap
  *     }
  * }
  * @endcode
  * If smpi_is_heap_obj() returns true, then c_ptr will be deleted in
  * delete_object_policy. Otherwise the delete_object_policy will do
  * nothing to the pointer. This is used in situations, say, some gui
  * system require some kind of resource with certain interface (virtual
  * functions), and the GUI system use smart pointer to automaticly
  * manage the resource object. If the interface defines a virtual
  * function (usually pure virtual):
  * @code
  * class font_interface{
  * public:
  *     virtual char* get_dot_matrix(char character);
  *     ...
  *
  *     virtual smpi_is_heap_obj() = 0;
  * }
  * @endcode
  * Then your user-defined font object, which you want to be selected
  * into the gui system, doesn't have to be created on the heap.
  * You can inherit the interface and implement your own
  * smpi_is_heap_obj, returns the correct answer and then the system
  * will work pretty well with the smart pointer! Note that you may
  * have to add additional data member and changge its constructor
  * function interface to ensure that the object knows where he is
  * and smpi_is_heap_obj returns the correct answer. Sometimes you
  * are pretty sure that your user-defined class object would never be
  * created on the heap, you can just return false in the
  * smpi_is_heap_obj in this case. The requirement is that you must
  * return the correct answer in the virtual smpi_is_heap_obj() function
  * if you defined it. Besides, if you defined it, you must make sure
  * that any derived class define this virtual method and return the
  * correct answer.
  * @note Object of this type <b> CANNOT RETURN BY ANY FUNCTION </b>.
  * @example nstd_smart_pointer_example.cpp
  */
template<typename Type,
        typename delete_policy = default_delete_policy<Type> >
struct obj_ptr: private delete_policy
{
public:
    typedef Type element_type;

	/** @brief default constructor */
	obj_ptr(): c_ptr(0){}

    /** @brief Construct with an ownership.
      * @code
      * // example 1
      * // initialize a smart pointer to point to an integer created on
      * // the heap and initialized with value 5;
      * obj_ptr<int> p( NEW_OBJ(int,(5)) );
      *
      * // example 2
      * // to transfer ownership explicitly
      * obj_ptr p1;
      * //... blah blah blah
      * obj_ptr p2(p1.give());
      * @endcode
      */
    template <typename ObjType>
    obj_ptr(const obj_ownership<ObjType>& i)
    { c_ptr = i.release(); }

    template <typename ObjType>     // introduce new object
    obj_ptr& operator =(const obj_ownership<ObjType>& i)
    {
        receive_heap_obj(i.release());
        return *this;
    }

	/** @brief Receive an object that is created on the heap and holds
      * the ownership of that object. for example:
      * @code
      * obj_ptr<int> p; p.receive_heap_obj(new int());
      * @endcode
      * @note the pointer must be returned my new Type(...).
	  */ inline
	obj_ptr<element_type>& receive_heap_obj(element_type* p)	/* not on the stack */
	{
        delete_policy::delete_object_policy(c_ptr);
        c_ptr = p;
		return *this;
	}

    /** @brief Delete the object owned by the obj_ptr. Sometimes you may
	  * want your object to be deleted quickly, than call this function
	  * manually.
	  */ inline
    void delete_object() {
		receive_heap_obj(0);
    }

    inline
    ~obj_ptr () { delete_object(); /* delete a null pointer is safe */ }

	/** @brief Expose its pointer, while the object is still having the
	  * ownership.
	  * @note In general, do not use this function unless you want to use
	  * the old c/c++ library that take pointer as the parameter. The
	  * obj_ptr would still delete the pointer when it is destructed.
	  * @return element_type pointer
	  */
    inline
    element_type* expose_c_ptr() const {
        return c_ptr;
    }

	/** @brief Dereference the pointer
	  */
    inline
    element_type& operator* () const {
        DYNAMIC_ASSERT(0 != expose_c_ptr());
        return *expose_c_ptr();
    }

	/** @brief Use this function to access object's member pointed by this
	  * obj_pointer.
	  */
    inline
	element_type* operator-> () const {
        /**< Although the user may call its static member function
        while c_ptr is null. whaterer, let's make it illegal */
        DYNAMIC_ASSERT(0 != expose_c_ptr());
        return  expose_c_ptr();
    }

	/** @brief Release the ownership of the object (without deleting it)
	  * and return the pointer to it. You have to delete the pointer by
	  * yourself.
	  * @brief In general, do not call this function unless you want to
	  * use some old c/c++ library functions that delete object automaticly
	  * by themselves.
	  */ inline
    element_type* release_c_ptr()  {
        element_type* __tmp = c_ptr;
        c_ptr = 0;
        return __tmp;
    }

    /** @brief Delete the object owned by obj_ptr. The same as the
      * delete_object() member function
      */
    inline
    void clean() { delete_object(); }

    /** @brief this function is used for membership transfer from
      * this obj_ptr to another obj_ptr.
	  * obj_ptr<int> p2(p1.give_ownership());
      */
    obj_ownership<element_type> give(){
        element_type* temp = c_ptr;
        c_ptr = 0;
        return obj_ownership<element_type>(temp);
    }

    inline
    obj_ownership<element_type> give_ownership(){ return give();}

    /** @brief to check whether this obj_ptr owns a valid object */
    inline
    operator bool() const { return c_ptr;}

private:

    /** @brief The obj_ptr should be the unique owner of object,
      * comparison between obj_ptr is meanless. Thus this operator
      * member function is not allowed to be called.
      */
    bool operator ==(obj_ptr<element_type>&) const;

    /** @brief do not use this operator explicitly */
    obj_ptr<element_type>& operator=(const obj_ptr<element_type>& );

    /** @brief copy constructor is not allowed */
    obj_ptr(obj_ptr<element_type>&);
    element_type* c_ptr;
};

// group smart_pointer
/**
  * @}
  */

} /* namespace nstd */
// group nstd
/**
  * @}
  */
#endif
