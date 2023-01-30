}
void CloseAvio(vlc_object_t *object)
{
    access_t *access = (access_t*)object;
    access_sys_t *sys = access->p_sys;
#if LIBAVFORMAT_VERSION_MAJOR < 54
    SetupAvioCb(NULL);
#endif
    avio_close(sys->context);
    free(sys);
}
