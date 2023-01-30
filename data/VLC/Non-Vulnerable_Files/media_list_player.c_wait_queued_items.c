}
static inline void wait_queued_items(struct check_items_order_data *check)
{
    // Wait dummily for check_items_order_callback() to flag 'done_playing':
    while (!check->done_playing)
        sched_yield();
}
