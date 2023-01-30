int avfilter_register(AVFilter *filter)
{
    AVFilter **f = last_filter;
    int i;
    
    av_assert0((filter->flags & AVFILTER_FLAG_SUPPORT_TIMELINE) != AVFILTER_FLAG_SUPPORT_TIMELINE);
    for(i=0; filter->inputs && filter->inputs[i].name; i++) {
        const AVFilterPad *input = &filter->inputs[i];
#if FF_API_AVFILTERPAD_PUBLIC
        av_assert0(     !input->filter_frame
                    || (!input->start_frame && !input->end_frame));
#endif
    }
    filter->next = NULL;
    while(*f || avpriv_atomic_ptr_cas((void * volatile *)f, NULL, filter))
        f = &(*f)->next;
    last_filter = &filter->next;
    return 0;
}
