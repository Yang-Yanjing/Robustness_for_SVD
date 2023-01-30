}
static void update_request_thread_cleanup( void *obj )
{
    filter_t* p_filter = (filter_t*)obj;
    p_filter->p_sys->b_continue = false;
}
