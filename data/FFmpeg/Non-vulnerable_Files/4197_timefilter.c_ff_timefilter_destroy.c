void ff_timefilter_destroy(TimeFilter *self)
{
    av_freep(&self);
}
