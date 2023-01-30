static int     OutSeek (sout_access_out_t *, off_t);
static int UrlInterruptCallback(void *access)
{
    return !vlc_object_alive((vlc_object_t*)access);
}
