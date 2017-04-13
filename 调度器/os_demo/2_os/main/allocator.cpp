#include "allocator.h"
#include "debug.h"

namespace nstd{

	/****************************************************
	function:  Allocator::DeleteHandleNode
	Description: 当一个有效的 Handle 析构时，需要调用这个
	函数，把 Handle 从链表中删除。
	****************************************************/
	void Allocator::DeleteHandleNode(HandleBase* node) {
		assert(pool);
		assert(node->mem);
		EnterCriticalSection();
		BlockHeader *b = GetHeader(node->mem);
		assert(b->handleChain);
		HandleBase* prev = b->handleChain;
		if(prev==node){
			b->handleChain = prev->next;
			/*
			如果产生了一个新的可用的数据块
			*/
			if(b->IsAvailable())
				NewAvailableBlock(b);
		}
		else{
			while(prev->next!=node) {
				prev = prev->next;
				assert(prev);
			}
			prev->next = node->next;
		}
		ExitCriticalSection();
		return ;
	}

	/****************************************************
	function: Allocator::Init(void*, SizeType)
	parameters:
		pool_init: 指向一个可用的内存池。
			这个内存池的首地址本身的字节对齐必须满足模板
			参数 allignment。
			pool_init 的值不可以为 NULL。
		size: 可用的内存池的大小。必须大于等于
			(PoolHeaderSizeRounded+BlockHeaderSizeRounded 
			+ allignment )。
			通常分配 128 字节已经能满足要求。
	****************************************************/
	void Allocator::Init(void* pool_init, SizeType size) {
		/* 首地址必须对齐 */
		assert(((unsigned long)pool_init%allignment)==0);
		/* 不能为空 */
		assert(pool_init);
		/* 足够大 */
		assert(size>= ( PoolHeaderSizeRounded
			+ BlockHeaderSizeRounded
			+ MinAllocateRounded ));
		
		pool = (PoolHeader*)pool_init;
		
		/*
		初始化 pool header
		*/
		pool->size = size;
		pool->nextPool = NULL;
		pool->firstAvailable = 
			(BlockHeader*)(((char*)pool) + PoolHeaderSizeRounded);
		/*
		初始化 fristAvailable
		*/
		pool->firstAvailable->DeInit(this);
		pool->firstAvailable->size = size - 
			( PoolHeaderSizeRounded + BlockHeaderSizeRounded);
		
		dout<<"内存池首地址 "<<pool<<endl
			<<"内存池大小"<<(u32)pool->size<<endl
			<<"第一个可用内存块的地址为 "<<pool->firstAvailable<<endl
			<<"第一个可用内存块大小为 "<<(u32)pool->firstAvailable->size
			<<endl;
		return;
	}

	/****************************************************
	function: Allocator::Allocate
	description: 申请得到一块可用的内存
	parameters:
		size: 申请的内存的大小
	return: 内存块的 BlockHeader
	remark: 分配出去后的 BlockHeader 的 lockCount 为 1
	****************************************************/
	Allocator::BlockHeader*
	Allocator::AllocateMemory(SizeType size) {
		assert(pool);
		/*
		silly request
		*/
		if(size==0) return NULL;
		
		/*
		round it
		*/
		size  = (~(allignment-1))&(size+allignment-1 );
		size = MinAllocateRounded>size?
				MinAllocateRounded:size;
		
		EnterCriticalSection();
		
		/*
		首先直接寻找有没有合适的可用的 block
		*/	
		BlockHeader* b = pool->firstAvailable;
		BlockHeader* prevAvail = NULL; 
		while(b){
			// if(b->size >= size) break;
			/*
			检查是否有需要合并的，否则容易制造碎片
			*/
			MergeAdjacentFollowing(b);
			if(b->size >= size) break;
			
			prevAvail = b;
			b = b->availBlock;
		}
		if((b==NULL)&&(pool->firstAvailable)){
			/*
			这个时候，就需要进行碎片整理工作了
			碎片整理算法的内容是：
			在可用数据块的单向链表中，寻找拼接后大小能够满
			足要求的，相连的可相拼接的几个数据块，把它们进行拼接
			(这里的相连是说那几个数据块之间本身就是一个单向链表的
			结构而可拼接的意思是说这几个数据块之间的所有不可用的
			数据块都是可移动的)。
			为了让拼接的过程中移动的数据较少，采取的策略是：
			假设可用数据块 0 xxx 3 xxx 5 三个数据块需要相拼接，那
			么先把数据块 5 向 3 的方向移动，与数据块 3 拼接成新的
			数据块，新的数据块再向数据块 0 的方向移动与其拼接。(如
			果是3->0然后再让 5->0 那么要移动的数据就会比较多了。也
			可以选择0->3 然后再 3->5 的拼接方式)
			当然，这样的算法效率比较低，但是已经能够满足很多需要了。
			*/
			prevAvail = NULL;
			BlockHeader* mergeBegin = pool->firstAvailable;
			BlockHeader* mergeEnd = mergeBegin;
			SizeType mergeSize = mergeBegin->size;
			while(1){
				BlockHeader* prevMergeEnd = mergeEnd;
				mergeEnd=mergeEnd->availBlock;
				if(mergeEnd){
					if( IsMergeAble(prevMergeEnd,mergeEnd) )
						mergeSize+=mergeEnd->size+BlockHeaderSizeRounded;
					else{
						/*
						遇到了不能被移动的数据块，囧，又得重新开始搜寻了
						*/
						prevAvail = prevMergeEnd;
						mergeBegin = mergeEnd;
						mergeSize = mergeBegin->size;
						continue;
					}
				}
				else{
					/*
					还是不能找出足够大的内存块
					*/
					dout<<"还是找不出足够大的内存块"<<endl;
					break;
				}
				/*
				检查合并后的大小是否能够满足需求
				*/
				if(mergeSize>=size){
					b = Merge(mergeBegin, mergeEnd, prevAvail);
					break;
				}
			}
		}
		BlockHeader* result = NULL;
		if(b){
			/*
			有足够大的可用的内存块可以分配
			*/
			if(b->size >= 
				size+BlockHeaderSizeRounded + MinAllocateRounded)
			{
				/* 如果可以把这个 block 拆分 */
				result = (BlockHeader*)( 
					(char*)b+/*+BlockHeaderSizeRounded*/
					b->size-size/*-BlockHeaderSizeRounded*/);	
				result->DeInit(this);
				result->size=size;
				result->availBlock=b;
				result->lockCount=1;
				b->size -= BlockHeaderSizeRounded+size;
			}
			else{
				/* 如果不可拆分直接分配出去 */
				result = b;
				if(prevAvail) {
					prevAvail->availBlock = result->availBlock;
				}
				else {
					pool->firstAvailable =  result->availBlock;
				}
				/*
				加锁，让在函数
				PointedByFollowingAllocatedBlocks 中把
				result 这个 block 视为不可用的。
				*/
				result->lockCount=1;
				PointedByFollowingAllocatedBlocks(prevAvail);
			}
		}
		ExitCriticalSection();
		return result;
	}

	/****************************************************
	function: Allocator::Free
	description: 强制释放申请的内存
	parameters:
		size: 得到的内存的首地址
	****************************************************/
	void Allocator::ForceFree(void *mem){
		assert(mem);
		
		BlockHeader* b = GetHeader(mem);
		
		assert(!b->IsAvailable());
		
		/*
		如果有，先删除所有引用这个内存块的 handle，把它标记为可用
		*/
		while(b->handleChain){
			HandleBase* temp = b->handleChain;
			b->handleChain = temp->next;	/* 从单向链表中删除 */
			temp->mem = NULL;	/* null it */
			temp->next = NULL;	
		}
		b->lockCount = 0;
		
		NewAvailableBlock(b);
	}

	/****************************************************
	function: Allocator::PointByFollowingAllocatedBlocks
	parameter:
		b: 当前的 block
	description:
		让一个 block 的后面的所有被分配出去的 block 指向
		这个 block。
	****************************************************/
	void Allocator::PointedByFollowingAllocatedBlocks
		(const BlockHeader* const b)
	{
		for(BlockHeader*i=b?
				GetNextAdjacentBlock(b):
				(BlockHeader*)((char*)pool+PoolHeaderSizeRounded); 
			i&&(!(i->IsAvailable())); 
			i=GetNextAdjacentBlock(i))
		{
			i->availBlock = (BlockHeader*)b;
		}
	}

	/****************************************************
	function: Allocator::GetextBlock
	parameter:
		b: 当前的 block
	ret:
		相邻的下一个 block，如果没有，用返回 NULL。
	****************************************************/
	Allocator::BlockHeader* Allocator::GetNextAdjacentBlock
		(const BlockHeader*const b)
	{
		BlockHeader* ret = (BlockHeader*)
			((char*)b+BlockHeaderSizeRounded+b->size);
		char* end = (char*)pool+pool->size;
		return ((char*)ret < end) ? ret :NULL;
	}

	/****************************************************
	function: Allocator::InsertNode
	parameter:
		node: 需要被插入的结点。
	remark: 这个函数只在内存块被加锁时才调用，所以内部不
		需要 EnterCriticalSection ExitCriticalSection
	****************************************************/
	void Allocator::InsertNode(HandleBase* node) {
		assert(node->mem);
		//EnterCriticalSection();
		BlockHeader* h= (BlockHeader*)
			((char*)node->mem - BlockHeaderSizeRounded);
		node->next = h->handleChain;
		h->handleChain=node;
		//ExitCriticalSection();
	}
	/*
		以一个 Handle 构造另一个 Handle 时调用这个函数。
		必须进入 CriticalSection
	*/
	void Allocator::InsertNode(const HandleBase* list, HandleBase* node){
		EnterCriticalSection();
		BlockHeader* b= (BlockHeader*)
			((char*)list->mem - BlockHeaderSizeRounded);
		node->next = b->handleChain;
		b->handleChain=node;
		node->mem = list->mem;
		node->alloc = list->alloc;
		ExitCriticalSection();
	}
	
	/****************************************************
	function: Allocator::NewAvailableBlock
	description: 当有一个数据块从不可用变成可用的时候，
		需要调用这个函数，进行必要的整理工作来维护链表结构。
	parameter:
		node: 需要被插入的结点。
	remark: 在这里不进行数据块合并工作，而是把合并工作交给 
		Allocate 函数在搜索可用的数据块的过程中顺便完成。
	****************************************************/
	void Allocator::NewAvailableBlock(BlockHeader* b) {
		assert(b);
		/*
		接入可用的链表中。
		*/
		if(b->availBlock) {
			BlockHeader *nextAvail = b->availBlock->availBlock;
			b->availBlock->availBlock = b;
			b->availBlock = nextAvail;
		}
		else {
			/*
			如果 b->availBlock==NULL，说明它前面没有可用的 block，
			那么它将成为第一个可用的 block。
			*/
			BlockHeader* temp = pool->firstAvailable;
			pool->firstAvailable = b;
			b->availBlock = temp;
		}
		
		/*
		接入中用链表后，还需要调整，让随它后面的不可用的 block 的
		header 指向它
		*/
		PointedByFollowingAllocatedBlocks(b);
	}

	/****************************************************
	function: Allocator::MergeAdjacentFollowing
	description: 如果一个数据块相邻的后面的几个都是可用的，
		这个函数会把它们合并。
	parameter:
		b: 数据块的 header。
	****************************************************/
	void Allocator::MergeAdjacentFollowing(BlockHeader* b) {
		bool merged = false;
		for(BlockHeader* temp;
			b->availBlock&&
			(b->availBlock == (temp=GetNextAdjacentBlock(b)));)
		{
			/* 和下一个相邻的 block 进行合并 */
			b->size += BlockHeaderSizeRounded+temp->size;
			b->availBlock = temp->availBlock;
			merged = true;
		}
		if(merged){ 
			/* 合并后，维护链表的结构 */
			PointedByFollowingAllocatedBlocks(b);
		}
	}

	/****************************************************
	function: Allocator::MergeAble
	description: 这个函数用来判断两个可用的 block 是否可
		以相拼接。即如果这两个可用的 block 之间的所有其它
		block 都是可以移动的。
	parameter:
		b1: 第一个可用的 block
		b2: 第二个可用的 block
	remark:
		两个 block 之间必须有不可用的 block
	****************************************************/
	bool Allocator::
		IsMergeAble(BlockHeader* b1, BlockHeader* b2)
	{
		assert(b1<b2);
		assert(b1->IsAvailable()&&b2->IsAvailable());
		while(b1<b2){
			b1 = GetNextAdjacentBlock(b1);
			if(!b1->IsMovable())
				return false;
		}
		assert(b1==b2);
		return true;
	}

	/****************************************************
	function: Allocator::MergeAble
	description: 将两个数据块以及这两个数据块之间的所有可
		用的数据块进行合并。
	parameter:
		begin: 第一个可用的 block
		end: 第二个可用的 block
		beginPrev: begin 这个 block 之前的一个可用的 block
			为了维护内存池的数据结构和这个提高这个函数的
			操作效率需要提供的额外的信息。
	return: 合并以后的新的数据块。
	****************************************************/
	Allocator::BlockHeader* Allocator::Merge(BlockHeader* begin
			, BlockHeader* end
			, BlockHeader* beginPrev)
	{
		assert(begin&&end&&(begin<end));
		assert((beginPrev?beginPrev->availBlock:pool->firstAvailable)
				==begin);
		/*
		保存 begin
		*/
		BlockHeader* handleUpdateBeginBlocks = begin;
		
		while(1){
			char* source = (char*)GetNextAdjacentBlock(begin);
			assert(source);
			char* destination = (char*)begin;
			BlockHeader tempHeader = *begin;
			for(SizeType len = (char*)begin->availBlock-source;
				len--;){
				*(destination++) = *(source++);
			}
			begin = (BlockHeader*)destination ;
			*begin = tempHeader;
			
			/*
			到了这里，begin 刚好和 begin->availBlock 相邻
			现在把它们合并
			*/
			begin->size += 
				begin->availBlock->size + BlockHeaderSizeRounded;
			BlockHeader* temp = begin->availBlock;
			begin->availBlock = begin->availBlock->availBlock;
			if(temp >= end){
				assert(temp==end);
				break;
			}
		}
		/*
		维护链表
		*/
		if(beginPrev)
			beginPrev->availBlock = begin;
		else
			pool->firstAvailable = begin;
		PointedByFollowingAllocatedBlocks(begin);
		PointedByFollowingAllocatedBlocks(beginPrev);
		/*
		维护 handle chain
		*/
		UpdateBlocksHandleChain
			(handleUpdateBeginBlocks,begin);
		
		return begin;
	}

	/****************************************************
	function: Allocator::UpdateBlocksHandleChain
	description: 当被占用的可移动的内存块被移动后，需要
		更新和它相关联的 handle，调用这个函数，可以更新
		指定范围的blocks。
	parameter:
		begin: 更新的起始 block
		end: 更新的终止 block，这里的 end 可以是 NULL，
			end 本身指向的 block 不会被更新
	****************************************************/
	void Allocator::UpdateBlocksHandleChain
		(BlockHeader* begin, BlockHeader* end)
	{
		assert(begin);
		void *mem;
		do{
			mem = ((char*)begin)+ BlockHeaderSizeRounded;
			for(HandleBase* h=begin->handleChain;
				h!=NULL; h = h->next)
			{
				h->mem = mem;
			}
			
			begin = GetNextAdjacentBlock(begin);
		}while(begin!=end);
	}

	/*
	判断一个内存块是不是只剩下最后一引用者
	*/
	bool Allocator::RemainLastRef(const PtrBase& ptr){
		assert(ptr.base);
		BlockHeader* b = GetHeader(ptr.base);
		return ((b->lockCount==1)&&(b->handleChain==NULL));
	}
	bool Allocator::RemainLastRef(const HandleBase& h){
		assert(h.mem);
		/*
		handle 的 mem 随时都可能改变，所以必须
		EnterCriticalSection
		*/
		EnterCriticalSection();
		BlockHeader* b = GetHeader(h.mem);
		bool result = ((b->lockCount==0)&&
					(b->handleChain && 
					(b->handleChain->next==NULL)));
		ExitCriticalSection();
		return result;
	}
	
	/*
	在 handle 析构时和由 handle 构造指针时会调用这个函数
	*/
	void Allocator::IncLockCount(const HandleBase& handle) {
		EnterCriticalSection();
		BlockHeader* b = GetHeader(handle.mem);
		b->lockCount++;
		EnterCriticalSection();
	}
	
	/*
	递增内存块的加锁计数，当内存块的加锁计数为非 0 时，
	内存块为不可用且不能被移动
	当内存块的加锁计数为 0 时，表明没有 Ptr 对象引脚
	内存块，所以内存块可以移动
	*/
	void Allocator::IncLockCount(const PtrBase& p) {
		assert(p.base);
		BlockHeader* b = GetHeader(p.base);
		EnterCriticalSection();
		b->lockCount++;
		ExitCriticalSection();
	}
	
	/*
	递减内存块的加锁计数，当内存块的加锁计数为非 0 时，
	内存块为不可用且不能被移动
	当内存块的加锁计数为 0 时，表明没有 Ptr 对象引脚
	内存块，所以内存块可以移动
	*/
	void Allocator::DecLockCount(const PtrBase& p){
		assert(p.base);
		BlockHeader* b = GetHeader(p.base);
		EnterCriticalSection();
		b->lockCount--;
		if(b->IsAvailable()){
			NewAvailableBlock(b);
		}
		ExitCriticalSection();
		return ;
	}

	/*
	在 handle 析构的时候会调用这个函数
	*/
	void Allocator::DecLockCount(const HandleBase& h){
		assert(h.mem);
		BlockHeader* b = GetHeader(h.mem);
		EnterCriticalSection();
		b->lockCount--;
		/*
		因为还有一个 handle 引用着这块内存，所以这次操作
		不可能产生一个新的可用内存块。
		*/
		ExitCriticalSection();
		return ;
	}
	
	/********************************* HandleBase **********************************
	****/
	
	void HandleBase::Init(const HandleBase& h) {
		h.GetAllocator()->InsertNode(&h,this);
	}
	
	void HandleBase::Init(Allocator *alloc_init, void* mem_init){
		alloc = alloc_init;
		mem = mem_init;
		if(mem)
			alloc->InsertNode(this);
	}
	
	/*
	释放对象持有的内存，注意这个函数里面不作空指针判断，所以不能
	直接使用。
	*/
	void HandleBase::Release() {
		assert(alloc&&mem);
		alloc->DeleteHandleNode(this);
		mem = NULL;
		alloc=NULL;
	}
	
	bool HandleBase::IsLastRef() {
		return GetAllocator()
				->RemainLastRef(*this);
	}
		
	HandleBase::SizeType HandleBase::GetObjNum() {
		return GetAllocator()
					->GetObjNum(*this);
	}
	
	void HandleBase::IncLockCount() const {
		assert(mem);
		GetAllocator()->IncLockCount(*this);
	}
	
	void HandleBase::DecLockCount() const {
		assert(mem);
		GetAllocator()->DecLockCount(*this);
	}
	/****
	***************************** HandleBase **************************************/

	
	/********************************* PtrBase **********************************
	****/
	
	void PtrBase::Init( void* base_init)
	{
		p = base=base_init;
		if(base_init)
			Allocator::GetAllocator(base)
				->IncLockCount(*this);
	}
	
	void PtrBase::Init(void* base_init, void* p_init){
		base = base_init, p = p_init;
		if(base_init)
			Allocator::GetAllocator(base)
				->IncLockCount(*this);
	}
	
	void PtrBase::Init(const HandleBase& h) {
		if(h.mem)
			h.IncLockCount();
		/*
		加锁了以后 h->mem 这个成员才有意义
		*/
		base=p=h.mem;
	}
	
	/*
	释放对象持有的内存，注意这个函数里面不作空指针判断，所以不能
	直接使用。
	*/
	void PtrBase::Release()
	{
		Allocator::GetAllocator(base)
			->DecLockCount(*this);
		base=NULL;
		p=NULL;
	}
	
	/*
	判断这个 Ptr 对象是不是最后一个引脚内存块的对象。
	在 Ptr 对象析构时，根据这个来决定是否析构堆上的
	对象。
	*/
	bool PtrBase::IsLastRef() {
		return Allocator::GetAllocator(base)
				->RemainLastRef(*this);
	}
	
	/*
	得到内存块中的保存的对象的数量
	*/
	PtrBase::SizeType PtrBase::GetObjNum() {
		return Allocator::GetAllocator(base)
				->GetObjNum(*this);
	}
	
	Allocator *PtrBase::GetAllocator(){
		return base?Allocator::GetAllocator(base)
				:NULL;
	}
	/****
	***************************** PtrBase **************************************/
	
	#ifdef MYDEBUG

	void Allocator::DebugPrint(){
		dout<<"现在开始遍历内存池的数据块..."<<endl;
		dout<<"firstAvailable 的值为 0x"<<pool->firstAvailable<<endl;
		BlockHeader* b = (BlockHeader*)((char*)pool+PoolHeaderSizeRounded);
		int i = 0;
		while(b){
			char* usable;
			if(b->IsAvailable())
				usable = "可用，" ;
			else
				usable = "不可用，";
			dout<<"内存块"<<(u32)i++<<"的 header 的地址为 0x"<<b
				<<"，大小为"<<(u32)b->size
				<<"，"<< usable
				<<endl<<"    "
				<<"header 成员 availableBlock:0x"<<b->availBlock
				<<" lockCount: "<<(u32)b->lockCount<<" handleChain:0x"<<b->handleChain
				<<endl;
			b = GetNextAdjacentBlock(b);
		}
	}
	#endif

} /* namespace nstd */
