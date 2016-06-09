#pragma once






template<class T, class Alloc>
class SimpleAlloc
{
public:
	static T *Allocate(size_t n)
	{
		return 0 == n ? 0 : (T*)Alloc::Allocate(n * sizeof(T));
	}
	static T *Allocate(void)
	{
		return (T*)Alloc::Allocate(sizeof(T));
	}
	static void Deallocate(T *p, size_t n)
	{
		if (0 != n) Alloc::Deallocate(p, n * sizeof(T));
	}
	static void Deallocate(T *p)
	{
		Alloc::Deallocate(p, sizeof(T));
	}
};










template<int inst>
class __MallocAllocTemplate//һ��������
{
private:
	static void *OomMalloc(size_t n)
	{
		void(*MyMallocHandler)();//�ڴ�����ͨ���û����õ�handler����ڴ治�����⣨�ͷ�һ�����ڴ棩
		void *result;

		while (1)
		{
			MyMallocHandler = __MallocAllocOomHandler;
			if (0 == MyMallocHandler)
			{
				cout << "�𿪱��ˣ�ʵ��û���ˣ��˴�Ӧ���׳��쳣��" << endl;
				exit(-1);
			}
			(*MyMallocHandler)();
			result = malloc(n);
			if (result) return(result);
		}
	}
	static void	*OomRealloc(void *ptr, size_t n)
	{
		void(*MyMallocHandler)();//�ڴ�����ͨ���û����õ�handler����ڴ治�����⣨�ͷ�һ�����ڴ棩
		void *result;

		while (1)
		{
			MyMallocHandler = __MallocAllocOomHandler;
			if (0 == MyMallocHandler)
			{
				cout << "�𿪱��ˣ�ʵ��û���ˣ��˴�Ӧ���׳��쳣��" << endl;
				exit(-1);
			}
			(*MyMallocHandler)();
			result = realloc(p, n);
			if (result) return(result);
		}
	}
	static void(*__MallocAllocOomHandler)();//���庯��ָ�룬��ʵûɶ��
public:
	static void *Allocate(size_t n)
	{
		void *result = malloc(n);
		if (0 == result)
		{
			result = OomMalloc(n);
		}
		return result;
	}
	static void Deallocate(void *p, size_t n)//�����n�ƺ��ô�����
	{
		free(p);
	}
	static void *Reallocate(void *p, size_t oldsize, size_t newsize)
	{
		void *result = realloc(p, newsize);
		if (0 == result)
		{
			result = OomRealloc(p, newsize);
		}
		return result;
	}
	static void(*SetMallocHandler(void(*f)()))()
	{
		void(*old)() = __MallocAllocOomHandler;
		__MallocAllocOomHandler = f;
		return(old);//�����µĴ�����ָ�룬���ؾɵĺ���ָ��
	}

};
template <int inst>
void(*__MallocAllocTemplate<inst>::__MallocAllocOomHandler)() = 0;//����˵�Ǹ�����ָ��һ��ûɶ�ö���ΪNULL��


typedef __MallocAllocTemplate<0> MallocAlloc;

# ifdef __USE_MALLOC

typedef malloc_alloc Alloc;
# else


template<bool threads, int inst>
class __DefaultAllocTemplate//�����ռ������������ڴ��
{
	enum { __ALIGN = 8 };
	enum { __MAX_BYTES = 128 };
	enum { __NFREELISTS = __MAX_BYTES / __ALIGN };
private:

	static size_t ROUND_UP(size_t bytes) {
		return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
	}
	union Obj {
		union Obj * FreeListLink;
		char ClientData[1];    /* The client sees this.        */
	};



	static Obj * volatile FreeList[__NFREELISTS];//������������
	static char *StartFree;
	static char *EndFree;
	static size_t HeapSize;



	static  size_t FREELIST_INDEX(size_t bytes) {
		return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
	}



	static void *Refill(size_t n)
	{
		int nobjs = 20;
		char *chunk = ChunkAlloc(n, nobjs);
		Obj* volatile MyFreeList;
		Obj* result;
		int i;
		if (1 == nobjs)
		{
			return chunk;
		}
		int index = FREELIST_INDEX(n);
		MyFreeList = FreeList[index];
		result = (Obj *)chunk;
		MyFreeList = (Obj *)(chunk + n);//ȡ��ʣ�µĽڵ㰤�����鵽��Ӧ����������λ����ȥ
		Obj* end = (Obj *)MyFreeList;
		for (i = 1;; i++)
		{
			if (nobjs - 1 == i)
			{
				end->FreeListLink = 0;
				break;
			}
			else
			{
				end->FreeListLink = (Obj *)(chunk + (i + 1)*n);
			}
		}
		return result;
	}
	static char *ChunkAlloc(size_t size, int &nobjs)
	{
		char *result;
		size_t totalbytes = size*nobjs;
		size_t bytesleft = EndFree - StartFree;
		if (bytesleft >= totalbytes)//�ڴ�ص��ڴ���ȫ����
		{
			result = StartFree;
			StartFree += totalbytes;
			return result;
		}
		else if (bytesleft >= size)//�ڴ�ص��ڴ���Ȼ����20�����������ٹ�1��
		{
			nobjs = bytesleft / size;
			totalbytes = nobjs*size;
			result = StartFree;
			StartFree += totalbytes;
			return result;
		}
		else
		{
			size_t bytestoget = 2 * totalbytes + ROUND_UP(HeapSize >> 4);
			if (bytesleft > 0)//����һ�����ڴ�û�з��䣬��Ҫ�ȷ����ȥ
			{
				int index = FREELIST_INDEX(bytesleft);
				Obj * volatile MyFreeList = FreeList[index];
				Obj *tmp = (Obj *)StartFree;
				tmp->FreeListLink = MyFreeList;//ͷ��
				MyFreeList = tmp;
			}
			StartFree = (char *)malloc(bytestoget);
			if (0 == StartFree)
			{
				int i;
				Obj *volatile MyFreeList, *p;
				for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
					int index = FREELIST_INDEX(i);
					MyFreeList = FreeList[index];
					p = MyFreeList;
					if (0 != p)
					{
						MyFreeList = p->FreeListLink;
						StartFree = (char *)p;
						EndFree = StartFree + i;
						return(ChunkAlloc(size, nobjs));
					}
				}
				EndFree = 0;
				StartFree = (char *)MallocAlloc::Allocate(bytestoget);

			}
			HeapSize += bytestoget;
			EndFree = StartFree + bytestoget;
			return (ChunkAlloc(size, nobjs));
		}
	}



	


public:
	static void *Allocate(size_t n)
	{
		Obj* volatile MyFreeList;
		Obj* result;
		if (n > (size_t)__MAX_BYTES)//����128�ֽڵ�һ����������ȥ
		{
			return MallocAlloc::Allocate(n);
		}
		int index = FREELIST_INDEX(n);
		MyFreeList = FreeList[index];
		result = MyFreeList;
		if (result == 0)
		{
			void *r = Refill(ROUND_UP(n));
			return r;
		}
		MyFreeList = result->FreeListLink;
		return result;
	}


	static void Deallocate(void *p, size_t n)
	{
		int index = FREELIST_INDEX(n);
		Obj* q = (Obj *)p;
		Obj* volatile MyFreeList;
		if (n > (size_t)__MAX_BYTES)
		{
			MallocAlloc::Deallocate(p, n);
			return;
		}
		MyFreeList = FreeList[index];//���ڴ�黹��������������ͷ��ķ�ʽ������������
		q->FreeListLink = MyFreeList;
		MyFreeList = q;

	}




};
template <bool threads, int inst>
typename __DefaultAllocTemplate<threads, inst>::Obj* volatile __DefaultAllocTemplate<threads, inst>::FreeList[__DefaultAllocTemplate<threads, inst>::__NFREELISTS];

template <bool threads, int inst>
char* __DefaultAllocTemplate<threads, inst>::StartFree = 0;
template <bool threads, int inst>
char* __DefaultAllocTemplate<threads, inst>::EndFree = 0;
template <bool threads, int inst>
size_t __DefaultAllocTemplate<threads, inst>::HeapSize = 0;;


typedef __DefaultAllocTemplate<0, 0> Alloc;


#endif





void Test1()
{
	// ���Ե���һ�������������ڴ�
	cout << "���Ե���һ�������������ڴ�" << endl;
	char*p1 = SimpleAlloc<char, Alloc>::Allocate(129);
	SimpleAlloc<char, Alloc>::Deallocate(p1, 129);

	// ���Ե��ö��������������ڴ�
	cout << "���Ե��ö��������������ڴ�" << endl;
	char*p2 = SimpleAlloc<char, Alloc>::Allocate(128);
	char*p3 = SimpleAlloc<char, Alloc>::Allocate(128);
	char*p4 = SimpleAlloc<char, Alloc>::Allocate(128);
	char*p5 = SimpleAlloc<char, Alloc>::Allocate(128);
	SimpleAlloc<char, Alloc>::Deallocate(p2, 128);
	SimpleAlloc<char, Alloc>::Deallocate(p3, 128);
	SimpleAlloc<char, Alloc>::Deallocate(p4, 128);
	SimpleAlloc<char, Alloc>::Deallocate(p5, 128);

	for (int i = 0; i < 21; ++i)
	{
		printf("���Ե�%d�η���\n", i + 1);
		char*p = SimpleAlloc<char, Alloc>::Allocate(128);
	}
}