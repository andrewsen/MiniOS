#include <new>

void *operator new(size_t size)
{
    return (void*)kmalloc(size);
}

void *operator new(size_t size, void*p)
{

}

void *operator new[](size_t size)
{
    return (void*)kmalloc(size);
}

void operator delete(void *p)
{
    //cout << "delete(" << (uint)p << ")\n";
    kfree(p);
}

void operator delete[](void *p)
{
    //cout << "delete(" << (uint)p << ")\n";
    kfree(p);
}

void operator delete(void *p, unsigned int size)
{
    //cout << "delete(" << (uint)p << ", " << size <<  ")\n";
    kfree(p);
}
