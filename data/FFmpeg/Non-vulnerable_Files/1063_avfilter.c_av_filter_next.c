AVFilter **av_filter_next(AVFilter **filter)
{
    return filter ? &(*filter)->next : &first_filter;
}
