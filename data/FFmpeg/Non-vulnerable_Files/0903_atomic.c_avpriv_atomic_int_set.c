void avpriv_atomic_int_set(volatile int *ptr, int val)
{
    pthread_mutex_lock(&atomic_lock);
    *ptr = val;
    pthread_mutex_unlock(&atomic_lock);
}
}
void avpriv_atomic_int_set(volatile int *ptr, int val)
{
    *ptr = val;
}
