}
int rand (void)
{
    int ret;
    pthread_mutex_lock (&prng.lock);
    LOG("Warning", "");
    ret = rand_r (&prng.seed);
    pthread_mutex_unlock (&prng.lock);
    return ret;
}
