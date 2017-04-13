#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "nstd_type.h"
#include "nstd_assert.h"

namespace nstd{

	class HandleBase;
	class PtrBase;
	class Allocator;
	
	enum PtrDebugCmd{
		  PtrDebugDisable = 0
		, PtrDebugEnable = !PtrDebugDisable
	};
	template<typename T, PtrDebugCmd d>class Ptr;
	template <typename T>class Hanlde;
	
	typedef u16 Allocator_SizeType;
	
	/*
	因为无法在 Allocator 类中无法定义 friend template ...
	所以定义了一个 PtrBase 基类来作为 Allocator 的 friend。
	*/
	class HandleBase{
		
	protected:
		
		typedef Allocator_SizeType SizeType;

		friend class Allocator;
		friend class PtrBase;

		void Init(Allocator* alloc_init, void* mem_init);
		void Init(const HandleBase& h);
		finline HandleBase():alloc(NULL),next(NULL),mem(NULL){}	
		finline HandleBase(const HandleBase& h){Init(h);};
		HandleBase(Allocator* alloc_init, void* mem_init) {Init(alloc_init,mem_init);}
		
		/*
		引用的内存对应的 alloc
		*/
		Allocator* alloc;
		
		/*
		Handle chain 的下一个元素
		*/
		HandleBase* next;
						
		/*
		在进行碎片整理时，它的值可能会被修改，所以这里
		声明为 volatile。
		*/
		void * volatile mem;

		bool IsLastRef();
		
		SizeType GetObjNum();
		
		/*
		释放对象持有的内存，注意这个函数里面不作空指针判断，所以不能
		直接使用。
		*/
		void Release();
		
		/*
		得到 Handle 所引用的内存对应的 allocator
		*/
		finline
		Allocator* GetAllocator() const{return alloc;}
		
		void IncLockCount() const;
		void DecLockCount() const;
	};

	class PtrBase{
	private:
		PtrBase& operator =(const PtrBase& p){return*this;}
	protected:
		friend class HandleBase;
		friend class Allocator;
		typedef Allocator_SizeType SizeType;

		/*
		block 的首地址
		*/
		void *base;

		/*
		用户操作时使用的指针。
		*/
		void* p;

		void Init( const HandleBase& h);
		void Init( void* base_init);
		void Init( void* base_init, void* p_init);	
	
		/*
		constructors
		*/
		finline PtrBase():base(NULL),p(NULL){}	
		finline PtrBase(const HandleBase& h){ Init(h); }
		PtrBase(const PtrBase& ptr) { Init(ptr.base, ptr.p); }
		
		void Release();
		
		bool IsLastRef();
		
		SizeType GetObjNum();
		Allocator* GetAllocator();
	};

	template<typename T>
	class Handle: public HandleBase{
		friend class Ptr<T,PtrDebugDisable>;
		friend class Allocator;
	public:
		/*
		constructors
		*/
		finline Handle():HandleBase(){}
	protected:
		finline Handle(Allocator* alloc_init,void* mem_init)
				:HandleBase(alloc_init,mem_init){}
	public:
		
		finline Handle(const Handle<T>& h):HandleBase(h){}
			
		Handle(const Ptr<T,PtrDebugDisable>& p);
			
		void Release() {
			if(mem){
				if(nstd::is_class_type<T>::result
				  && IsLastRef())
				{
					IncLockCount();
					SizeType num 
							= GetObjNum();
					T* p = (T*)mem;
					while(num--){
						(p++)->~T();
					}
					DecLockCount();
				}
				HandleBase::Release();
			}
		}
		
		Handle<T>& operator = (const Handle<T>& h){
			if(mem!=h.mem) {
				Handle<T>::Release();
				Init(h);
			}
			return *this;
		}
		
		/*
		当返回 false时，表示这个 handle 是无效的。
		*/
		finline operator bool() {return mem;}
		
		finline ~Handle() { Handle<T>::Release();}
		
		finline SizeType Length(){ return mem?GetObjNum():0; }
	};

	template <typename T, PtrDebugCmd d = PtrDebugDisable>
	class Ptr: public PtrBase{
		friend class Handle<T>;
	public:
		typedef int IndexType;
		
		T* operator ->()			{	return ((T*)p);}
		T& operator *() 			{ return *((T*)p); }
		finline
		T& operator[](IndexType i)		{ return ((T*)p)[i];}
		finline 
		T& operator()(IndexType i)		{ return ((T*)p)[i]; }
		
		finline 
		Ptr<T>& operator ++()	{ p = (((T*)p)+1);return *this;}
		finline 
		Ptr<T>& operator --()	{p = (((T*)p)-1);return *this;}		
		finline
		Ptr<T>& operator +=(IndexType i) { p = (((T*)p)+i);return *this;}
		finline
		Ptr<T>& operator -=(IndexType i) { p = (((T*)p)-i);return *this;}
		
		/*
		这两个操作符会产生临时对象，代价比较沉重，
		优化起来也比较麻烦，干脆就不支持了
		*/
		/*
		finline Ptr<T> operator ++(int) {
			Ptr<T> temp(*this);
			++*this;
			return temp;
		}

		finline Ptr<T> operator --(int) {
			Ptr<T> temp(*this);
			--*this;
			return temp;
		}
		*/

		
		finline Ptr():PtrBase() {}
		finline Ptr(const Handle<T>& h):PtrBase(h){}
		finline Ptr(const Ptr<T>& ptr):PtrBase(ptr) {}
		finline ~Ptr(){ Ptr<T>::Release(); }
		
		finline void Release()	{
			if(base){
				if(nstd::is_class_type<T>::result
				  && IsLastRef())
				{
					SizeType num 
							= GetObjNum();
					T* p = (T*)base;
					while(num--){
						(p++)->~T();
					}
				}
				PtrBase::Release();
			}
		}
			
		finline operator bool() {return p;}
		
		Ptr<T>& operator =(const Ptr<T>& ptr) {
			if(base!= ptr.base) {
				Release();
				Init(ptr.base,ptr.p);
			}
			else{
				p = ptr.p;
			}
			return *this;
		}
		Ptr<T>& operator =(const Handle<T>& h){
			if(base!= h.mem) {
				Release();
				Init(h);
			}
			else{
				p = h.mem;
			}
			return *this;
		}
		
		finline SizeType Length(){ return base?GetObjNum():0; }
		
		finline
		bool IsInRange(IndexType offset=0) 
			{ return (((T*)p+offset)>=(T*)base)
					&&(((T*)p+offset)<(T*)base+Length()); }
		finline
		bool IsValid() {return operator bool();}
	};

	template <typename T> tinline
	Handle<T>::Handle(const Ptr<T>& p){ Init(p.GetAllocator(),p.base); }
	
	/*
	为调试做特化
	*/
	template <typename T>
	class Ptr<T,PtrDebugEnable>: public Ptr<T,PtrDebugDisable>{
		public:
		typedef Ptr<T,PtrDebugDisable> Father;
		typedef typename Father::IndexType IndexType;
		
		finline Ptr():Father() {}
		finline Ptr(const Handle<T>& h):Father(h){}
		finline Ptr(const Ptr<T>& ptr):Father(ptr) {}
		~Ptr(){ Father::Release(); }
		
		/* 重新定义所有需要访问内存的函数，检测错误 */
 		T* operator ->()	{
			assert(Father::IsValid()&&Father::IsInRange());
			return Father::operator->();
		}
 		T& operator *() {
			assert(Father::IsValid()&&Father::IsInRange());
			return Father::operator*();
		}
		T& operator[]( IndexType i)	{
			assert(Father::IsValid()&&Father::IsInRange(i));
			return Father::operator[](i);
		}
		T& operator()( IndexType i)	{
			assert(Father::IsValid()&&Father::IsInRange(i));
			return Father::operator()(i);
		}
	};
	
	
	/****************************************************
	class : Allocator
	class parameters:
		SizeType: 它是一个与机器相关的unsigned类型，其大小足
		  以保证存储内存中对象的大小。
		allignment: 对齐方式。在32位机上通常需要4字节对齐。
	****************************************************/
	class Allocator{
	public:
		/*
		定义一个能表示内存池大小的无符号类型
		*/
		typedef Allocator_SizeType	SizeType;
		enum{
			/*
			对齐方式，4字节对齐
			*/
			  allignment = 4
			/*
			分配出去的最小内存块的大小
			*/
			, min_allocate = 4
		};
	protected:

		/*
		一个内存块的 header，包含着这个内存块的信息
		*/
		struct BlockHeader{
			public:
			/*
			对于未分配出去的 block，这个变量指向下一个可用的
			block。对于已经分配出去的 block，这个变量指向前
			一个可用的 block（为了方便把被 free 出来的 block
			链接回去）。
			*/
			BlockHeader* availBlock;
			/*
			reference 指向一个被分配出去的指针 
			*/
			HandleBase* handleChain;
			
			/*
			通过这个成员找到掌管这块内存池的 allocator
			*/
			Allocator* alloc;
			
			/*
			加锁后表示这段内存字占用并且不能被移动
			*/
			SizeType lockCount;
			
			/*
			这个 block 的大小，不包括 header
			*/
			SizeType size;
			/*
			用户申请的对象的数量
			*/
			SizeType objNum;
			
			finline void DeInit(Allocator* alloc_init)
				{availBlock=NULL,handleChain=NULL,alloc=alloc_init,lockCount=0,size=0;}
			
			finline
			bool IsAvailable() 
				{return (lockCount==0)&&(handleChain==NULL);}
			finline
				bool IsMovable() { return lockCount==0; }
		};
		
		struct PoolHeader{
			PoolHeader* nextPool;	/* 这个变量暂时不使用 */
			SizeType	size;		/* 当前内存池的大小，包括 header */
			BlockHeader* firstAvailable; /* 第一个可用的 block */
		} *pool;
		
		enum{
			  PoolHeaderSizeRounded = (~(allignment-1))
							& ( sizeof(PoolHeader)+allignment-1 )
			, BlockHeaderSizeRounded =  (~(allignment-1))
							& ( sizeof(BlockHeader)+allignment-1 )
			, MinAllocateRounded = (~(allignment-1))
							& ( min_allocate+allignment-1 )
		};
		static finline
		BlockHeader* GetHeader(void* mem){
			return (BlockHeader*)(((char*)mem)-BlockHeaderSizeRounded);
		}
		static finline
		Allocator* GetAllocator(void* mem){	return GetHeader(mem)->alloc; }
		
		void PointedByFollowingAllocatedBlocks(const BlockHeader* const b);
		BlockHeader* GetNextAdjacentBlock(const BlockHeader*const b);
		
		friend class HandleBase;
		
		void InsertNode(HandleBase* node);
		void InsertNode(const HandleBase* list, HandleBase* node);
		void DeleteHandleNode(HandleBase* node);
		
		void NewAvailableBlock(BlockHeader* b);
		void MergeAdjacentFollowing(BlockHeader* b);
		
		bool IsMergeAble(BlockHeader* b1, BlockHeader* b2);
		BlockHeader* Merge(BlockHeader* begin
				, BlockHeader* end, BlockHeader* beginPrev);
		
		void UpdateBlocksHandleChain
				(BlockHeader* begin, BlockHeader* end);
		
	protected:
		
		friend class PtrBase;
		finline 
		void EnterCriticalSection(){}
		finline
		void ExitCriticalSection(){}
		
		void IncLockCount(const HandleBase& );
		void DecLockCount(const HandleBase& );
		void IncLockCount(const PtrBase& p);
		void DecLockCount(const PtrBase& p);
			
		BlockHeader* AllocateMemory(SizeType size);
		
		bool RemainLastRef(const PtrBase& mem);
		bool RemainLastRef(const HandleBase& h);
		
		finline 
		SizeType GetObjNum(const HandleBase& h) {
			assert(h.mem);
			EnterCriticalSection();
			/*
			handle 的 mem 随时都可能改变的，所属必须
			进入 critical section
			*/
			SizeType result =  GetHeader(h.mem)->objNum;
			ExitCriticalSection();
			return result;
		}
		
		finline 
		SizeType GetObjNum(const PtrBase& p) {
			assert(p.base);
			return GetHeader(p.base)->objNum;;
		}
		
		void ForceFree(void* men);
	public:
		
		void DebugPrint();
		
		void Init(void* pool_init, SizeType size);
		
	/****************************************************
	function: StorageManager::Allocate
	description: 申请一块内存来保存一定数量的对象
	template parameter:
		Type: 内存中的对象的类型
	parameter:
		num: 对象的数量
		ini: 对象的被赋的初始值
	remark: 这个函数可能不会调用对象的构造函数，而是通过
		赋值的方式完成对象的构造。（由于编译器不支持比较新的
		C++标准的无奈之举。）
	****************************************************/
		template<typename Type>
		Handle<Type> Allocate(SizeType num, const Type& ini){
			assert(num);	/* 不可能申请 0 的内存 */

			BlockHeader* b = AllocateMemory(num*sizeof(Type));

			if(b==NULL){
				return Handle<Type>();
			}
			assert(b->lockCount==1);
			b->objNum = num;	/* 为析构保存 */
			Type* mem = (Type*)((char*)b+BlockHeaderSizeRounded);
			Handle<Type> h(this,mem);
			
			/*
			初始化
			*/
			while(num--){
				*(mem++) = ini;
			}
			
			/*
			刚分配出来时的 lockCount 为 1，
			现在把它解锁
			*/
			b->lockCount = 0; 
			return h;
		}
		template<typename Type>
		Handle<Type> Allocate(SizeType num){
			BlockHeader* b = AllocateMemory(num*sizeof(Type));

			if(b==NULL){
				return Handle<Type>();
			}
			assert(b->lockCount==1);
			b->objNum = num;	/* 为析构保存 */
			Type* mem = (Type*)((char*)b+BlockHeaderSizeRounded);
			Handle<Type> h(this,mem);
			
			/*
			如果是类，就对它们进行默认的初始化
			*/
			if(nstd::is_class_type<Type>::result){
				Type ini=Type();
				while(num--){
					*(mem++) = ini;
				}
			}
			
			/*
			刚分配出来时的 lockCount 为 1，
			现在把它解锁
			*/
			b->lockCount = 0;
			return h;
		}
	};

} /* namespace nstd */

extern nstd::Allocator* GetAllocator();

#endif
