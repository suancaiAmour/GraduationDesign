#ifndef __cplusplus
#define _DISTRIBUTED_ARRAY_H_
#endif
#ifndef _DISTRIBUTED_ARRAY_H_
#define _DISTRIBUTED_ARRAY_H_

/** @file distributed_array.h
  * @author Pony279
  * @brief 这个头文件的使用信息请参考 @ref GLOBAL_ARRAY_BEGIN 和
  * @ref CLASS_ARRAY_BEGIN .
  * @note 如果使用的编译器支持 constexpr, 请在包含这个头文件之前
  * 定义 CONSTEXPR_SUPPORT, 以获得更好的优化效果.
  */

#include "nstd_static_assert.h"
#include "nstd_member_detector.h"

/** @defgroup nstd
  * @{
  */

	/** @defgroup compile_time_array
	  * @brief 这个模板的使用信息请参考 @ref GLOBAL_ARRAY_BEGIN 和
	  * @ref CLASS_ARRAY_BEGIN .
	  * @ingroup nstd
	  * @{
	  */

#ifdef _MSC_VER /* VC 的编译器的空基类优化做得不好 */
	#define EMPTY_BASE_CLASS_OPTIMIZATION_ASSERT_FAILED
#endif

#ifndef __COUNTER__
	#define __COUNTER__ __LINE__
#endif

/** 如果定义了 CONSTEXPR_SUPPORT 表示编译器支持 constexpr 关键
	字 */
//#define CONSTEXPR_SUPPORT

#ifdef CONSTEXPR_SUPPORT
	#define constexpr_fun	constexpr
	#define constexpr_obj	constexpr
#else
	#define constexpr_fun	inline
	#define constexpr_obj	const
#endif

/** @def GLOBAL_ARRAY_BEGIN
  * @brief 用于建立编译期数组, 其初始值可以在不同的地方声明.
  * 使用示例如下:
  *	@code
  *	#include <iostream>
  *	#include "distributed_array.h"
  *
  *	GLOBAL_ARRAY_BEGIN(int, arr);
  *	ARRAY_ELEMENT(arr, 1);
  *
  * void fun();	// declare a function is ok.
  *
  *	ARRAY_ELEMENT(arr, 3);
  *	ARRAY_ELEMENT(arr, 5);
  *	GLOBAL_ARRAY_END(arr);
  *
  *	int main(){
  *		using namespace std;
  *
  *		cout<<"arr.length() = " <<arr.length()<<endl;
  *		for(int i=0; i<arr.length(); i++)
  *			cout<<"arr["<<i<<"] = "<<arr[i]<<endl;
  *	}
  * // program output:
  * // arr.length() = 3
  * // arr[0] = 1
  * // arr[1] = 3
  * // arr[2] = 5
  * @endcode
  */
#define GLOBAL_ARRAY_BEGIN(ele_type,name)			\
    template<int,typename dummy=void> struct		\
	name##_ele_{};									\
	template<typename _dummy>	struct				\
	name##_ele_<0,_dummy>							\
	{												\
		enum {										\
			begin = __COUNTER__ 					\
		};											\
		typedef const ele_type type;				\
		 constexpr_fun name##_ele_(){}				\
	}

#define ARRAY_ELEMENT(name, value)                  \
        template<typename T_##name> struct          \
            name##_ele_<__COUNTER__                 \
                    ,T_##name>                      \
        {                                           \
            typedef name##_ele_<0>::type type;      \
			type e;                           		\
            constexpr_fun name##_ele_():e(value){}	\
        }

/** @brief 创建特化计数器.
  * @param type 模板的类型. 该模板类型只能接收一个 int 类型的
  * 参数.
  * @param member type 模板特化后特有的成员
  * @param name 计数器的名字.
  */
#define SPECIALIZATION_COUNTER_CREATE(type,member,name)		\
template <int begin, int end>                               \
struct name{												\
    enum { 													\
    	value = name<begin,(begin+end)/2>::value			\
				+ name<(begin+end)/2+1,end>::value 			\
	};														\
};                                                          \
template <int _i>                                           \
struct name< _i,  _i>{										\
    CREATE_MEMBER_DETECTOR(member);                         \
    enum { value =  DETECT_MEMBER(type<_i>,member) };		\
}

/** @brief 得到特化计数器的计数结果.
  * @param n 特化计数器的名字.
  * @param begin 起始特化标号.
  * @param end 终止特化标号. begin 和 end 限定了查找的范围.
  */
#define SPECIALIZATION_COUNTER_VALUE(name,begin,end)		\
						name<begin,end>::value

/** @def SPECIALIZATION_FINDER_CREATE
  * @brief 模板的特化类型查找器. 参考 @ref SPECIALIZATION_FINDER_RESULT
  * @param count 模板对应的特化计数器的名字.
  * @param template_type 模板的名字.
  * @param name 查找器的名字
  */
#define SPECIALIZATION_FINDER_CREATE(count,template_type,name)	\
template<int index,int begin,int end>							\
struct name{													\
    enum {														\
        n = SPECIALIZATION_COUNTER_VALUE						\
				(count,begin,(begin+end)/2)     				\
    };                                                          \
    template<bool , typename dummy=void>						\
    struct select{												\
		typedef name<index, begin,(begin+end)/2> type;			\
    };															\
    template <typename dummy>									\
    struct select<false,dummy>{									\
    	typedef name<index-n,(begin+end)/2+1,end> type;			\
	};															\
	typedef typename select<(n>index)>::type range;				\
    typedef typename range::type type;							\
};																\
template<int index,int name##i>									\
struct name<index,name##i,name##i>{								\
    typedef template_type<name##i> type;						\
}

/** @def SPECIALIZATION_FINDER_RESULT
  * @brief 得到特化查找器的查找结果, 结果是一个特化后的
  * 类型. 例如:
  * @code
  *	template<int i> struct T{};
  *	template<> struct T<3>{static const int i = 0;};
  *	template<> struct T<8>{static const int i = 2;};
  *
  * // the finder needs a counter first
  *	SPECIALIZATION_COUNTER_CREATE(T,i,counter);
  *	SPECIALIZATION_FINDER_CREATE(counter,T,finder);
  *
  *	// get the first specialization type from T<0> to T<10>
  *	SPECIALIZATION_FINDER_RESULT(finder,0,0,10) a;
  *	// get the second specialization type from T<0> to T<10>
  *	SPECIALIZATION_FINDER_RESULT(finder,1,0,10) b;
  *
  *	int main(){
  *		using namespace std;
  *	cout<<"a.i = "<<a.i<<endl
  *		<<"b.i = "<<b.i<<endl;
  * }
  * // program output
  * // a.i = 0
  * // b.i = 2
  * @endcode
  * @param name 特化查找器的名字.
  * @param index 特化的索引号 (第一次特化的索引号为0).
  * @param begin 查找的起始模板参数
  * @param end 查找的终止模板参数
  */
#define SPECIALIZATION_FINDER_RESULT(name,index,begin,end)		\
        name<index,begin,end>::type

#define ARRAY_COUNTER_MAX	200

/** @brief 在声明完数组的元素后, 使用这个宏来产生最终的
  * 数组类型.
  * @param temp_name 数组元素对应的模板的名字
  * @param type_name 生成的类型的名字
  */
#define ARRAY_TYPE_CREATE(temp_name,type_name)					\
	struct type_name{											\
		private:												\
		SPECIALIZATION_COUNTER_CREATE(temp_name,e,counter);		\
		enum{													\
			  begin = temp_name<0>::begin						\
			, end_counter = temp_name<-1>::end					\
			, current_end = (end_counter - begin) >				\
							ARRAY_COUNTER_MAX ?					\
						begin : end_counter						\
			, count = SPECIALIZATION_COUNTER_VALUE				\
						(counter,begin,current_end)				\
			, end = count>0?current_end:-1						\
		};														\
		template<int b, int e>									\
		struct arr:												\
			  public arr<b,(b+e)/2>								\
			, public arr<(b+e)/2+1,e>							\
		{														\
			constexpr_fun arr(){}								\
		};														\
		template<int b>											\
        struct arr<b, b>: public temp_name<b>					\
		{														\
			constexpr_fun arr(){}								\
		};														\
		template<int b>											\
        struct arr<b, -1> /* for error checking */				\
		{														\
			STATIC_ASSERT(b==-1);								\
		};														\
		arr<begin+1,end> a;										\
		typedef temp_name<0>::type type;						\
		public:													\
		constexpr_fun type_name(){}								\
        constexpr_fun size_t length() const						\
		{ return sizeof(*this)/sizeof(type); }					\
        type& operator[](size_t i) const						\
        { return ((type*)(this))[i]; }							\
	}

#define ARRAY_TYPE_CREATE_WITHOUT_EBCO(temp_name,type_name)		\
struct type_name{												\
		private:												\
		SPECIALIZATION_COUNTER_CREATE(temp_name,e,counter);		\
		SPECIALIZATION_FINDER_CREATE(counter,temp_name,finder);	\
		enum{													\
			  begin_counter = temp_name<0>::begin				\
			, begin = 0											\
			, end_counter = temp_name<-1>::end					\
			, current_end = (end_counter - begin_counter) >		\
							ARRAY_COUNTER_MAX ?					\
						begin_counter : end_counter				\
			, count = SPECIALIZATION_COUNTER_VALUE				\
						(counter,begin,current_end)				\
			, end = count>0?count-1:-1							\
		};														\
		template<int b, int e>									\
		struct arr:												\
			  public arr<b,(b+e)/2>								\
			, public arr<(b+e)/2+1,e>							\
		{														\
			constexpr_fun arr(){}								\
		};														\
		template<int b>											\
        struct arr<b, b>:SPECIALIZATION_FINDER_RESULT			\
			(finder,b,begin_counter,end_counter)				\
		{														\
			constexpr_fun arr(){}								\
		};														\
		template<int b>											\
        struct arr<b, -1> /* for error checking */				\
		{														\
			STATIC_ASSERT(b==-1);								\
		};														\
		arr<begin,end> a;										\
		typedef temp_name<0>::type type;						\
		public:													\
		constexpr_fun type_name(){}								\
        constexpr_fun size_t length() const						\
		{ return sizeof(*this)/sizeof(type); }					\
        type& operator[](size_t i) const						\
        { return ((type*)(this))[i]; }							\
	}

#define ARRAY_END_HELPER(name)						\
	template<typename _dummy>	struct				\
	name <-1,_dummy>								\
	{												\
		enum {										\
			end = __COUNTER__ 						\
		};											\
		 constexpr_fun name (){}					\
	}

#define GLOBAL_ARRAY_END(name) 						\
	ARRAY_END_HELPER(name##_ele_);					\
	ARRAY_TYPE_CREATE(name##_ele_,name##_arrT) constexpr_obj name

/** @def CLASS_ARRAY_BEGIN
  * @brief 用于建立一个编译期数组, 数组的元素可以在不同的地方声明.
  * CLASS_ARRAY_BEGIN 和 CLASS_ARRAY_END 用于在 class/struct 中定义数组的
  * 元素, 其用法如下:
  * @code
  * // class definition in the .h file
  *	class Test{
  *		CLASS_ARRAY_BEGIN(int, arr);
  *		ARRAY_ELEMENT(arr, 1);
  *		ARRAY_ELEMENT(arr, 3);
  *		int class_member;
  *		ARRAY_ELEMENT(arr, 5);
  *
  *		CLASS_ARRAY_END(arr);
  *	};
  * // add this code in the .cpp file
  *	CLASS_ARRAY_IMPLEMENT(Test,arr);
  *	int main(){
  *		using namespace std;
  *		// Note that arr is a global variable,
  *		// not a member of struct Test
  *		cout<<"arr.length() = " <<arr.length()<<endl;
  *		for(int i=0; i<arr.length(); i++)
  *			cout<<"arr["<<i<<"] = "<<arr[i]<<endl;
  *	}
  * // program output
  * // arr.length() = 3
  * // arr[0] = 1
  * // arr[1] = 3
  * // arr[2] = 5
  * @endcode
  * @note 虽然数组是在类/结构体内容定义的, 但是最终实现的数组
  * 是一个全局数组, 而不是类的成员!
  */
#define CLASS_ARRAY_BEGIN	GLOBAL_ARRAY_BEGIN

#define	CLASS_ARRAY_END(name)		\
	ARRAY_END_HELPER(name##_ele_);	\
	friend struct name##_wrap

#define CLASS_ARRAY_IMPLEMENT(class_name, array)				\
	ARRAY_TYPE_CREATE(class_name::array##_ele_,array##_wrap)	\
	constexpr_obj array

#ifdef EMPTY_BASE_CLASS_OPTIMIZATION_ASSERT_FAILED
	#undef ARRAY_TYPE_CREATE
	#define ARRAY_TYPE_CREATE ARRAY_TYPE_CREATE_WITHOUT_EBCO
#endif

/** empty base class optimization assert */
class EBCO_assert{

	CLASS_ARRAY_BEGIN(int, array_1);
	CLASS_ARRAY_BEGIN(char, array_2);
	ARRAY_ELEMENT(array_1,1);
	ARRAY_ELEMENT(array_2,2);
	ARRAY_ELEMENT(array_1,1);
	ARRAY_ELEMENT(array_2,2);
	CLASS_ARRAY_END(array_1);
	CLASS_ARRAY_END(array_2);

	ARRAY_TYPE_CREATE(array_1_ele_,arr1_type);
	ARRAY_TYPE_CREATE(array_2_ele_,arr2_type);

    /**< 如果这个 STATIC_ASSERT 失败了, 可能是由于编译器没有做好空
    基类优化. 可以尝试在包含此头文件之前添加宏定义
    #define EMPTY_BASE_CLASS_OPTIMIZATION_ASSERT_FAILED
    然后再重新编译.
    */
    STATIC_ASSERT(sizeof(arr1_type)==2*sizeof(int)
				&& sizeof(arr2_type)==2*sizeof(char));
};

	// group compile_time_array
	/**
	  * @}
	  */

// group nstd
/**
  * @}
  */

#endif
