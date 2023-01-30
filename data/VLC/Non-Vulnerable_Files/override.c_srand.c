} prng = { PTHREAD_MUTEX_INITIALIZER, 0, };
void srand (unsigned int seed)
{
    pthread_mutex_lock (&prng.lock);
    LOG("Warning", "%d", seed);
    prng.seed = seed;
    pthread_mutex_unlock (&prng.lock);
}
