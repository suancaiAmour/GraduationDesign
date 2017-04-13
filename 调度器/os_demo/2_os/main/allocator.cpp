#include "allocator.h"
#include "debug.h"

namespace nstd{

	/****************************************************
	function:  Allocator::DeleteHandleNode
	Description: ��һ����Ч�� Handle ����ʱ����Ҫ�������
	�������� Handle ��������ɾ����
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
			���������һ���µĿ��õ����ݿ�
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
		pool_init: ָ��һ�����õ��ڴ�ء�
			����ڴ�ص��׵�ַ������ֽڶ����������ģ��
			���� allignment��
			pool_init ��ֵ������Ϊ NULL��
		size: ���õ��ڴ�صĴ�С��������ڵ���
			(PoolHeaderSizeRounded+BlockHeaderSizeRounded 
			+ allignment )��
			ͨ������ 128 �ֽ��Ѿ�������Ҫ��
	****************************************************/
	void Allocator::Init(void* pool_init, SizeType size) {
		/* �׵�ַ������� */
		assert(((unsigned long)pool_init%allignment)==0);
		/* ����Ϊ�� */
		assert(pool_init);
		/* �㹻�� */
		assert(size>= ( PoolHeaderSizeRounded
			+ BlockHeaderSizeRounded
			+ MinAllocateRounded ));
		
		pool = (PoolHeader*)pool_init;
		
		/*
		��ʼ�� pool header
		*/
		pool->size = size;
		pool->nextPool = NULL;
		pool->firstAvailable = 
			(BlockHeader*)(((char*)pool) + PoolHeaderSizeRounded);
		/*
		��ʼ�� fristAvailable
		*/
		pool->firstAvailable->DeInit(this);
		pool->firstAvailable->size = size - 
			( PoolHeaderSizeRounded + BlockHeaderSizeRounded);
		
		dout<<"�ڴ���׵�ַ "<<pool<<endl
			<<"�ڴ�ش�С"<<(u32)pool->size<<endl
			<<"��һ�������ڴ��ĵ�ַΪ "<<pool->firstAvailable<<endl
			<<"��һ�������ڴ���СΪ "<<(u32)pool->firstAvailable->size
			<<endl;
		return;
	}

	/****************************************************
	function: Allocator::Allocate
	description: ����õ�һ����õ��ڴ�
	parameters:
		size: ������ڴ�Ĵ�С
	return: �ڴ��� BlockHeader
	remark: �����ȥ��� BlockHeader �� lockCount Ϊ 1
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
		����ֱ��Ѱ����û�к��ʵĿ��õ� block
		*/	
		BlockHeader* b = pool->firstAvailable;
		BlockHeader* prevAvail = NULL; 
		while(b){
			// if(b->size >= size) break;
			/*
			����Ƿ�����Ҫ�ϲ��ģ���������������Ƭ
			*/
			MergeAdjacentFollowing(b);
			if(b->size >= size) break;
			
			prevAvail = b;
			b = b->availBlock;
		}
		if((b==NULL)&&(pool->firstAvailable)){
			/*
			���ʱ�򣬾���Ҫ������Ƭ��������
			��Ƭ�����㷨�������ǣ�
			�ڿ������ݿ�ĵ��������У�Ѱ��ƴ�Ӻ��С�ܹ���
			��Ҫ��ģ������Ŀ���ƴ�ӵļ������ݿ飬�����ǽ���ƴ��
			(�����������˵�Ǽ������ݿ�֮�䱾�����һ�����������
			�ṹ����ƴ�ӵ���˼��˵�⼸�����ݿ�֮������в����õ�
			���ݿ鶼�ǿ��ƶ���)��
			Ϊ����ƴ�ӵĹ������ƶ������ݽ��٣���ȡ�Ĳ����ǣ�
			����������ݿ� 0 xxx 3 xxx 5 �������ݿ���Ҫ��ƴ�ӣ���
			ô�Ȱ����ݿ� 5 �� 3 �ķ����ƶ��������ݿ� 3 ƴ�ӳ��µ�
			���ݿ飬�µ����ݿ��������ݿ� 0 �ķ����ƶ�����ƴ�ӡ�(��
			����3->0Ȼ������ 5->0 ��ôҪ�ƶ������ݾͻ�Ƚ϶��ˡ�Ҳ
			����ѡ��0->3 Ȼ���� 3->5 ��ƴ�ӷ�ʽ)
			��Ȼ���������㷨Ч�ʱȽϵͣ������Ѿ��ܹ�����ܶ���Ҫ�ˡ�
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
						�����˲��ܱ��ƶ������ݿ飬�壬�ֵ����¿�ʼ��Ѱ��
						*/
						prevAvail = prevMergeEnd;
						mergeBegin = mergeEnd;
						mergeSize = mergeBegin->size;
						continue;
					}
				}
				else{
					/*
					���ǲ����ҳ��㹻����ڴ��
					*/
					dout<<"�����Ҳ����㹻����ڴ��"<<endl;
					break;
				}
				/*
				���ϲ���Ĵ�С�Ƿ��ܹ���������
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
			���㹻��Ŀ��õ��ڴ����Է���
			*/
			if(b->size >= 
				size+BlockHeaderSizeRounded + MinAllocateRounded)
			{
				/* ������԰���� block ��� */
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
				/* ������ɲ��ֱ�ӷ����ȥ */
				result = b;
				if(prevAvail) {
					prevAvail->availBlock = result->availBlock;
				}
				else {
					pool->firstAvailable =  result->availBlock;
				}
				/*
				���������ں���
				PointedByFollowingAllocatedBlocks �а�
				result ��� block ��Ϊ�����õġ�
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
	description: ǿ���ͷ�������ڴ�
	parameters:
		size: �õ����ڴ���׵�ַ
	****************************************************/
	void Allocator::ForceFree(void *mem){
		assert(mem);
		
		BlockHeader* b = GetHeader(mem);
		
		assert(!b->IsAvailable());
		
		/*
		����У���ɾ��������������ڴ��� handle���������Ϊ����
		*/
		while(b->handleChain){
			HandleBase* temp = b->handleChain;
			b->handleChain = temp->next;	/* �ӵ���������ɾ�� */
			temp->mem = NULL;	/* null it */
			temp->next = NULL;	
		}
		b->lockCount = 0;
		
		NewAvailableBlock(b);
	}

	/****************************************************
	function: Allocator::PointByFollowingAllocatedBlocks
	parameter:
		b: ��ǰ�� block
	description:
		��һ�� block �ĺ�������б������ȥ�� block ָ��
		��� block��
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
		b: ��ǰ�� block
	ret:
		���ڵ���һ�� block�����û�У��÷��� NULL��
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
		node: ��Ҫ������Ľ�㡣
	remark: �������ֻ���ڴ�鱻����ʱ�ŵ��ã������ڲ���
		��Ҫ EnterCriticalSection ExitCriticalSection
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
		��һ�� Handle ������һ�� Handle ʱ�������������
		������� CriticalSection
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
	description: ����һ�����ݿ�Ӳ����ñ�ɿ��õ�ʱ��
		��Ҫ����������������б�Ҫ����������ά������ṹ��
	parameter:
		node: ��Ҫ������Ľ�㡣
	remark: �����ﲻ�������ݿ�ϲ����������ǰѺϲ��������� 
		Allocate �������������õ����ݿ�Ĺ�����˳����ɡ�
	****************************************************/
	void Allocator::NewAvailableBlock(BlockHeader* b) {
		assert(b);
		/*
		������õ������С�
		*/
		if(b->availBlock) {
			BlockHeader *nextAvail = b->availBlock->availBlock;
			b->availBlock->availBlock = b;
			b->availBlock = nextAvail;
		}
		else {
			/*
			��� b->availBlock==NULL��˵����ǰ��û�п��õ� block��
			��ô������Ϊ��һ�����õ� block��
			*/
			BlockHeader* temp = pool->firstAvailable;
			pool->firstAvailable = b;
			b->availBlock = temp;
		}
		
		/*
		������������󣬻���Ҫ����������������Ĳ����õ� block ��
		header ָ����
		*/
		PointedByFollowingAllocatedBlocks(b);
	}

	/****************************************************
	function: Allocator::MergeAdjacentFollowing
	description: ���һ�����ݿ����ڵĺ���ļ������ǿ��õģ�
		�������������Ǻϲ���
	parameter:
		b: ���ݿ�� header��
	****************************************************/
	void Allocator::MergeAdjacentFollowing(BlockHeader* b) {
		bool merged = false;
		for(BlockHeader* temp;
			b->availBlock&&
			(b->availBlock == (temp=GetNextAdjacentBlock(b)));)
		{
			/* ����һ�����ڵ� block ���кϲ� */
			b->size += BlockHeaderSizeRounded+temp->size;
			b->availBlock = temp->availBlock;
			merged = true;
		}
		if(merged){ 
			/* �ϲ���ά������Ľṹ */
			PointedByFollowingAllocatedBlocks(b);
		}
	}

	/****************************************************
	function: Allocator::MergeAble
	description: ������������ж��������õ� block �Ƿ��
		����ƴ�ӡ���������������õ� block ֮�����������
		block ���ǿ����ƶ��ġ�
	parameter:
		b1: ��һ�����õ� block
		b2: �ڶ������õ� block
	remark:
		���� block ֮������в����õ� block
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
	description: ���������ݿ��Լ����������ݿ�֮������п�
		�õ����ݿ���кϲ���
	parameter:
		begin: ��һ�����õ� block
		end: �ڶ������õ� block
		beginPrev: begin ��� block ֮ǰ��һ�����õ� block
			Ϊ��ά���ڴ�ص����ݽṹ�����������������
			����Ч����Ҫ�ṩ�Ķ������Ϣ��
	return: �ϲ��Ժ���µ����ݿ顣
	****************************************************/
	Allocator::BlockHeader* Allocator::Merge(BlockHeader* begin
			, BlockHeader* end
			, BlockHeader* beginPrev)
	{
		assert(begin&&end&&(begin<end));
		assert((beginPrev?beginPrev->availBlock:pool->firstAvailable)
				==begin);
		/*
		���� begin
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
			�������begin �պú� begin->availBlock ����
			���ڰ����Ǻϲ�
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
		ά������
		*/
		if(beginPrev)
			beginPrev->availBlock = begin;
		else
			pool->firstAvailable = begin;
		PointedByFollowingAllocatedBlocks(begin);
		PointedByFollowingAllocatedBlocks(beginPrev);
		/*
		ά�� handle chain
		*/
		UpdateBlocksHandleChain
			(handleUpdateBeginBlocks,begin);
		
		return begin;
	}

	/****************************************************
	function: Allocator::UpdateBlocksHandleChain
	description: ����ռ�õĿ��ƶ����ڴ�鱻�ƶ�����Ҫ
		���º���������� handle������������������Ը���
		ָ����Χ��blocks��
	parameter:
		begin: ���µ���ʼ block
		end: ���µ���ֹ block������� end ������ NULL��
			end ����ָ��� block ���ᱻ����
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
	�ж�һ���ڴ���ǲ���ֻʣ�����һ������
	*/
	bool Allocator::RemainLastRef(const PtrBase& ptr){
		assert(ptr.base);
		BlockHeader* b = GetHeader(ptr.base);
		return ((b->lockCount==1)&&(b->handleChain==NULL));
	}
	bool Allocator::RemainLastRef(const HandleBase& h){
		assert(h.mem);
		/*
		handle �� mem ��ʱ�����ܸı䣬���Ա���
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
	�� handle ����ʱ���� handle ����ָ��ʱ������������
	*/
	void Allocator::IncLockCount(const HandleBase& handle) {
		EnterCriticalSection();
		BlockHeader* b = GetHeader(handle.mem);
		b->lockCount++;
		EnterCriticalSection();
	}
	
	/*
	�����ڴ��ļ������������ڴ��ļ�������Ϊ�� 0 ʱ��
	�ڴ��Ϊ�������Ҳ��ܱ��ƶ�
	���ڴ��ļ�������Ϊ 0 ʱ������û�� Ptr ��������
	�ڴ�飬�����ڴ������ƶ�
	*/
	void Allocator::IncLockCount(const PtrBase& p) {
		assert(p.base);
		BlockHeader* b = GetHeader(p.base);
		EnterCriticalSection();
		b->lockCount++;
		ExitCriticalSection();
	}
	
	/*
	�ݼ��ڴ��ļ������������ڴ��ļ�������Ϊ�� 0 ʱ��
	�ڴ��Ϊ�������Ҳ��ܱ��ƶ�
	���ڴ��ļ�������Ϊ 0 ʱ������û�� Ptr ��������
	�ڴ�飬�����ڴ������ƶ�
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
	�� handle ������ʱ�������������
	*/
	void Allocator::DecLockCount(const HandleBase& h){
		assert(h.mem);
		BlockHeader* b = GetHeader(h.mem);
		EnterCriticalSection();
		b->lockCount--;
		/*
		��Ϊ����һ�� handle ����������ڴ棬������β���
		�����ܲ���һ���µĿ����ڴ�顣
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
	�ͷŶ�����е��ڴ棬ע������������治����ָ���жϣ����Բ���
	ֱ��ʹ�á�
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
		�������Ժ� h->mem �����Ա��������
		*/
		base=p=h.mem;
	}
	
	/*
	�ͷŶ�����е��ڴ棬ע������������治����ָ���жϣ����Բ���
	ֱ��ʹ�á�
	*/
	void PtrBase::Release()
	{
		Allocator::GetAllocator(base)
			->DecLockCount(*this);
		base=NULL;
		p=NULL;
	}
	
	/*
	�ж���� Ptr �����ǲ������һ�������ڴ��Ķ���
	�� Ptr ��������ʱ����������������Ƿ��������ϵ�
	����
	*/
	bool PtrBase::IsLastRef() {
		return Allocator::GetAllocator(base)
				->RemainLastRef(*this);
	}
	
	/*
	�õ��ڴ���еı���Ķ��������
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
		dout<<"���ڿ�ʼ�����ڴ�ص����ݿ�..."<<endl;
		dout<<"firstAvailable ��ֵΪ 0x"<<pool->firstAvailable<<endl;
		BlockHeader* b = (BlockHeader*)((char*)pool+PoolHeaderSizeRounded);
		int i = 0;
		while(b){
			char* usable;
			if(b->IsAvailable())
				usable = "���ã�" ;
			else
				usable = "�����ã�";
			dout<<"�ڴ��"<<(u32)i++<<"�� header �ĵ�ַΪ 0x"<<b
				<<"����СΪ"<<(u32)b->size
				<<"��"<< usable
				<<endl<<"    "
				<<"header ��Ա availableBlock:0x"<<b->availBlock
				<<" lockCount: "<<(u32)b->lockCount<<" handleChain:0x"<<b->handleChain
				<<endl;
			b = GetNextAdjacentBlock(b);
		}
	}
	#endif

} /* namespace nstd */
