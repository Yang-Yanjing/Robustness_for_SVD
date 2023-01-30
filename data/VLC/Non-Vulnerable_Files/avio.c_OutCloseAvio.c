}
void OutCloseAvio(vlc_object_t *object)
{
    sout_access_out_t *access = (sout_access_out_t*)object;
    sout_access_out_sys_t *sys = access->p_sys;
#if LIBAVFORMAT_VERSION_MAJOR < 54
    SetupAvioCb(NULL);
#endif
    avio_close(sys->context);
    free(sys);
}
