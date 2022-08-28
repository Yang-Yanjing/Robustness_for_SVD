static inline int init_opencl_mtx(void)
{
#if HAVE_THREADS
    if (!atomic_opencl_lock) {
        int err;
        pthread_mutex_t *tmp = av_malloc(sizeof(pthread_mutex_t));
        if (!tmp)
            return AVERROR(ENOMEM);
        if ((err = pthread_mutex_init(tmp, NULL))) {
            av_free(tmp);
            return AVERROR(err);
        }
        if (avpriv_atomic_ptr_cas(&atomic_opencl_lock, NULL, tmp)) {
            pthread_mutex_destroy(tmp);
            av_free(tmp);
        }
    }
#endif
    return 0;
}
