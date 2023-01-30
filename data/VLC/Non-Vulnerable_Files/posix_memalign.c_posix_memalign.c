#include <malloc.h>
int posix_memalign (void **ptr, size_t align, size_t size)
{
    if (check_align (align))
        return EINVAL;
    int saved_errno = errno;
    void *p = memalign (align, size);
    if (p == NULL)
    {
        errno = saved_errno;
        return ENOMEM;
    }
    *ptr = p;
    return 0;
}
#else
int posix_memalign (void **ptr, size_t align, size_t size)
{
    if (check_align (align))
        return EINVAL;
    *ptr = NULL;
    return size ? ENOMEM : 0;
}
