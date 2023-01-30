static int log_buffer_size;
static void log_callback(void *ptr, int level, const char *fmt, va_list vl)
{
    AVClass* avc = ptr ? *(AVClass **) ptr : NULL;
    va_list vl2;
    char line[1024];
    static int print_prefix = 1;
    void *new_log_buffer;
    va_copy(vl2, vl);
    av_log_default_callback(ptr, level, fmt, vl);
    av_log_format_line(ptr, level, fmt, vl2, line, sizeof(line), &print_prefix);
    va_end(vl2);
#if HAVE_THREADS
    pthread_mutex_lock(&log_mutex);
    new_log_buffer = av_realloc_array(log_buffer, log_buffer_size + 1, sizeof(*log_buffer));
    if (new_log_buffer) {
        char *msg;
        int i;
        log_buffer = new_log_buffer;
        memset(&log_buffer[log_buffer_size], 0, sizeof(log_buffer[log_buffer_size]));
        log_buffer[log_buffer_size].context_name= avc ? av_strdup(avc->item_name(ptr)) : NULL;
        if (avc) {
            if (avc->get_category) log_buffer[log_buffer_size].category = avc->get_category(ptr);
            else                   log_buffer[log_buffer_size].category = avc->category;
        }
        log_buffer[log_buffer_size].log_level   = level;
        msg = log_buffer[log_buffer_size].log_message = av_strdup(line);
        for (i=strlen(msg) - 1; i>=0 && msg[i] == '\n'; i--) {
            msg[i] = 0;
        }
        if (avc && avc->parent_log_context_offset) {
            AVClass** parent = *(AVClass ***) (((uint8_t *) ptr) +
                                   avc->parent_log_context_offset);
            if (parent && *parent) {
                log_buffer[log_buffer_size].parent_name = av_strdup((*parent)->item_name(parent));
                log_buffer[log_buffer_size].parent_category =
                    (*parent)->get_category ? (*parent)->get_category(parent) :(*parent)->category;
            }
        }
        log_buffer_size ++;
    }
    pthread_mutex_unlock(&log_mutex);
#endif
}
