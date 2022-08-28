static int show_log(WriterContext *w, int section_ids, int section_id, int log_level)
{
    int i;
    pthread_mutex_lock(&log_mutex);
    if (!log_buffer_size) {
        pthread_mutex_unlock(&log_mutex);
        return 0;
    }
    writer_print_section_header(w, section_ids);
    for (i=0; i<log_buffer_size; i++) {
        if (log_buffer[i].log_level <= log_level) {
            writer_print_section_header(w, section_id);
            print_str("context", log_buffer[i].context_name);
            print_int("level", log_buffer[i].log_level);
            print_int("category", log_buffer[i].category);
            if (log_buffer[i].parent_name) {
                print_str("parent_context", log_buffer[i].parent_name);
                print_int("parent_category", log_buffer[i].parent_category);
            } else {
                print_str_opt("parent_context", "N/A");
                print_str_opt("parent_category", "N/A");
            }
            print_str("message", log_buffer[i].log_message);
            writer_print_section_footer(w);
        }
    }
    clear_log(0);
    pthread_mutex_unlock(&log_mutex);
    writer_print_section_footer(w);
    return 0;
}
