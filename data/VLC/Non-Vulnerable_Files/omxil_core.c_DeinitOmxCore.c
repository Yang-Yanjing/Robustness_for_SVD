}
void DeinitOmxCore(void)
{
    vlc_mutex_lock( &omx_core_mutex );
    omx_refcount--;
    if( omx_refcount == 0 )
    {
        pf_deinit();
        dll_close( dll_handle );
        CloseExtraDll();
    }
    vlc_mutex_unlock( &omx_core_mutex );
}
