static pthread_mutex_t atomic_lock = PTHREAD_MUTEX_INITIALIZER;
int avpriv_atomic_int_get(volatile int *ptr)
{
    int res;
    pthread_mutex_lock(&atomic_lock);
    res = *ptr;
    pthread_mutex_unlock(&atomic_lock);
    return res;
}
#elif !HAVE_THREADS
int avpriv_atomic_int_get(volatile int *ptr)
{
    return *ptr;
}
