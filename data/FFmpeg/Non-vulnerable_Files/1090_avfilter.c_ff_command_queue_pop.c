void ff_command_queue_pop(AVFilterContext *filter)
{
    AVFilterCommand *c= filter->command_queue;
    av_freep(&c->arg);
    av_freep(&c->command);
    filter->command_queue= c->next;
    av_free(c);
}
