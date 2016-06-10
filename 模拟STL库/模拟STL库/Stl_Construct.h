#pragma once

#include"TypeTraits.h"
#include"Stl_Iterator.h"



template<class T>
inline void Destory(T *pointer)
{
	pointer->~T();
}

template<class T1,class T2>
inline void Construct(T1 *p,const T2& value)
{
	new(p)T1(value);
}


template<class ForwardIterator>
inline void __DestoryAux(ForwardIterator first, ForwardIterator last,__FalseType)
{
	for (; first < last; ++first)
	{
		Destory(&*first);
	}
}

template<class ForwardIterator>
inline void __DestoryAux(ForwardIterator first, ForwardIterator last, __TrueType)
{}


template<class ForwardIterator, class T>
inline void __Destory(ForwardIterator first, ForwardIterator last,T *)
{
	typedef typename __TypeTraits<T>::HasTrivialDestructor TrivialDestructor;
	__DestoryAux(first, last, TrivialDestructor());
}//����ĺ����в�ͬ�ĵط�

template<class ForwardIterator>
inline void Destory(ForwardIterator first, ForwardIterator last)
{
	__Destory(first, last, ValueType(first));

}


inline void Destroy(char*, char*) {}
inline void Destroy(wchar_t*, wchar_t*) {}