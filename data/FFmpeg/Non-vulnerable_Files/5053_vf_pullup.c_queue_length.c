static int queue_length(PullupField *begin, PullupField *end)
{
    PullupField *f;
    int count = 1;
    if (!begin || !end)
        return 0;
    for (f = begin; f != end; f = f->next)
        count++;
    return count;
}
