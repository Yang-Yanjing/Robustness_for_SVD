static int thread_init_internal(ThreadContext *c, int nb_threads)
{
    int i, ret;
    if (!nb_threads) {
        int nb_cpus = av_cpu_count();
        
        if (nb_cpus > 1)
            nb_threads = nb_cpus + 1;
        else
            nb_threads = 1;
    }
    if (nb_threads <= 1)
        return 1;
    c->nb_threads = nb_threads;
    c->workers = av_mallocz_array(sizeof(*c->workers), nb_threads);
    if (!c->workers)
        return AVERROR(ENOMEM);
    c->current_job = 0;
    c->nb_jobs     = 0;
    c->done        = 0;
    pthread_cond_init(&c->current_job_cond, NULL);
    pthread_cond_init(&c->last_job_cond,    NULL);
    pthread_mutex_init(&c->current_job_lock, NULL);
    pthread_mutex_lock(&c->current_job_lock);
    for (i = 0; i < nb_threads; i++) {
        ret = pthread_create(&c->workers[i], NULL, worker, c);
        if (ret) {
           pthread_mutex_unlock(&c->current_job_lock);
           c->nb_threads = i;
           slice_thread_uninit(c);
           return AVERROR(ret);
        }
    }
    slice_thread_park_workers(c);
    return c->nb_threads;
}
