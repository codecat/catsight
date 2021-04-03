#include <Common.h>
#include <Script/Types/Pointer.h>

Script::Types::Pointer::Pointer()
{
	m_p = 0;
}

Script::Types::Pointer::Pointer(uintptr_t p)
{
	m_p = p;
}
