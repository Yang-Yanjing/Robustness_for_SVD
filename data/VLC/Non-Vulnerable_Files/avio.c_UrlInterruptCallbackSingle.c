static vlc_object_t *current_access = NULL;
static int UrlInterruptCallbackSingle(void)
{
    return UrlInterruptCallback(current_access);
}
