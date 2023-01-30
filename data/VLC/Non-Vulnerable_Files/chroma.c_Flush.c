}
static void Flush(filter_t *filter)
{
    filter_sys_t *sys = filter->p_sys;
    for (unsigned i = 0; i < MAX_PAST + MAX_FUTURE; i++)
        if (sys->history[i].field != NULL)
        {
            sys->history[i].field->destroy(sys->history[i].field);
            sys->history[i].field = NULL;
        }
}
