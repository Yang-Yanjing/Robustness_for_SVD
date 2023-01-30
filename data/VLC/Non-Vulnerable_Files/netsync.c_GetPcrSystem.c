}
static mtime_t GetPcrSystem(input_thread_t *input)
{
    int canc = vlc_savecancel();
    /* TODO use the delay */
    mtime_t system;
    if (input_GetPcrSystem(input, &system, NULL))
        system = -1;
    vlc_restorecancel(canc);
    return system;
}
