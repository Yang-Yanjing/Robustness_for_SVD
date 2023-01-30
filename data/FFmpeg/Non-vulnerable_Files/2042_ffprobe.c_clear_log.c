static void clear_log(int need_lock)
{
    int i;
    if (need_lock)
        pthread_mutex_lock(&log_mutex);
    for (i=0; i<log_buffer_size; i++) {
        av_freep(&log_buffer[i].context_name);
        av_freep(&log_buffer[i].parent_name);
        av_freep(&log_buffer[i].log_message);
    }
    log_buffer_size = 0;
    if(need_lock)
        pthread_mutex_unlock(&log_mutex);
}
