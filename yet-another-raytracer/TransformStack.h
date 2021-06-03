#pragma once

#include "Types.h"

#include <cassert>
#include <stack>
#include <exception>

/*
	Right-multiply stack of transformations. The last transformation is applied first.
	Each next item stored is composite transformations of all previous items and new one.
*/
class TransformStack
{
public:

	TransformStack(void)
	{
		// pushing Identity.
		m_stack.push(matrix4::identity());
	}

	~TransformStack(void)
	{
	}

	void push(const matrix4 & matrix)
	{
		//auto composite = matrix * m_stack.top();
		m_stack.push(matrix);
	}

	// Returns the composite transform of whole stack.
	const matrix4 & peek() const { return m_stack.top(); }

	void pop()
	{
        assert(m_stack.size() > 1);

		//if (m_stack.size() == 1)
		//	throw std::exception();
		m_stack.pop();
	}

private:
	std::stack<matrix4> m_stack;
};

