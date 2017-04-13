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
	��Ϊ�޷��� Allocator �����޷����� friend template ...
	���Զ�����һ�� PtrBase ��������Ϊ Allocator �� friend��
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
		���õ��ڴ��Ӧ�� alloc
		*/
		Allocator* alloc;
		
		/*
		Handle chain ����һ��Ԫ��
		*/
		HandleBase* next;
						
		/*
		�ڽ�����Ƭ����ʱ������ֵ���ܻᱻ�޸ģ���������
		����Ϊ volatile��
		*/
		void * volatile mem;

		bool IsLastRef();
		
		SizeType GetObjNum();
		
		/*
		�ͷŶ�����е��ڴ棬ע������������治����ָ���жϣ����Բ���
		ֱ��ʹ�á�
		*/
		void Release();
		
		/*
		�õ� Handle �����õ��ڴ��Ӧ�� allocator
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
		block ���׵�ַ
		*/
		void *base;

		/*
		�û�����ʱʹ�õ�ָ�롣
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
		������ falseʱ����ʾ��� handle ����Ч�ġ�
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
		�������������������ʱ���󣬴��۱Ƚϳ��أ�
		�Ż�����Ҳ�Ƚ��鷳���ɴ�Ͳ�֧����
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
	Ϊ�������ػ�
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
		
		/* ���¶���������Ҫ�����ڴ�ĺ����������� */
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
		SizeType: ����һ���������ص�unsigned���ͣ����С��
		  �Ա�֤�洢�ڴ��ж���Ĵ�С��
		allignment: ���뷽ʽ����32λ����ͨ����Ҫ4�ֽڶ��롣
	****************************************************/
	class Allocator{
	public:
		/*
		����һ���ܱ�ʾ�ڴ�ش�С���޷�������
		*/
		typedef Allocator_SizeType	SizeType;
		enum{
			/*
			���뷽ʽ��4�ֽڶ���
			*/
			  allignment = 4
			/*
			�����ȥ����С�ڴ��Ĵ�С
			*/
			, min_allocate = 4
		};
	protected:

		/*
		һ���ڴ��� header������������ڴ�����Ϣ
		*/
		struct BlockHeader{
			public:
			/*
			����δ�����ȥ�� block���������ָ����һ�����õ�
			block�������Ѿ������ȥ�� block���������ָ��ǰ
			һ�����õ� block��Ϊ�˷���ѱ� free ������ block
			���ӻ�ȥ����
			*/
			BlockHeader* availBlock;
			/*
			reference ָ��һ���������ȥ��ָ�� 
			*/
			HandleBase* handleChain;
			
			/*
			ͨ�������Ա�ҵ��ƹ�����ڴ�ص� allocator
			*/
			Allocator* alloc;
			
			/*
			�������ʾ����ڴ���ռ�ò��Ҳ��ܱ��ƶ�
			*/
			SizeType lockCount;
			
			/*
			��� block �Ĵ�С�������� header
			*/
			SizeType size;
			/*
			�û�����Ķ��������
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
			PoolHeader* nextPool;	/* ���������ʱ��ʹ�� */
			SizeType	size;		/* ��ǰ�ڴ�صĴ�С������ header */
			BlockHeader* firstAvailable; /* ��һ�����õ� block */
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
			handle �� mem ��ʱ�����ܸı�ģ���������
			���� critical section
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
	description: ����һ���ڴ�������һ�������Ķ���
	template parameter:
		Type: �ڴ��еĶ��������
	parameter:
		num: ���������
		ini: ����ı����ĳ�ʼֵ
	remark: ����������ܲ�����ö���Ĺ��캯��������ͨ��
		��ֵ�ķ�ʽ��ɶ���Ĺ��졣�����ڱ�������֧�ֱȽ��µ�
		C++��׼������֮�١���
	****************************************************/
		template<typename Type>
		Handle<Type> Allocate(SizeType num, const Type& ini){
			assert(num);	/* ���������� 0 ���ڴ� */

			BlockHeader* b = AllocateMemory(num*sizeof(Type));

			if(b==NULL){
				return Handle<Type>();
			}
			assert(b->lockCount==1);
			b->objNum = num;	/* Ϊ�������� */
			Type* mem = (Type*)((char*)b+BlockHeaderSizeRounded);
			Handle<Type> h(this,mem);
			
			/*
			��ʼ��
			*/
			while(num--){
				*(mem++) = ini;
			}
			
			/*
			�շ������ʱ�� lockCount Ϊ 1��
			���ڰ�������
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
			b->objNum = num;	/* Ϊ�������� */
			Type* mem = (Type*)((char*)b+BlockHeaderSizeRounded);
			Handle<Type> h(this,mem);
			
			/*
			������࣬�Ͷ����ǽ���Ĭ�ϵĳ�ʼ��
			*/
			if(nstd::is_class_type<Type>::result){
				Type ini=Type();
				while(num--){
					*(mem++) = ini;
				}
			}
			
			/*
			�շ������ʱ�� lockCount Ϊ 1��
			���ڰ�������
			*/
			b->lockCount = 0;
			return h;
		}
	};

} /* namespace nstd */

extern nstd::Allocator* GetAllocator();

#endif
