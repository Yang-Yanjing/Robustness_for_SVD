int avpriv_atomic_int_add_and_fetch(volatile int *ptr, int inc)
{
    int res;
    pthread_mutex_lock(&atomic_lock);
    *ptr += inc;
    res = *ptr;
    pthread_mutex_unlock(&atomic_lock);
    return res;
}
}
int avpriv_atomic_int_add_and_fetch(volatile int *ptr, int inc)
{
    *ptr += inc;
    return *ptr;
}
