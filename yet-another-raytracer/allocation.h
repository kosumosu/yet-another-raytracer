#pragma once

#include <memory>

#define MM_ALIGNMENT 16

//inline void * operator new(size_t size) throw()
//{
//	void * ptr = _aligned_malloc(size, MM_ALIGNMENT);
//	return ptr;
//}
//
//inline void operator delete(void * ptr) throw()
//{
//	_aligned_free(ptr);
//}