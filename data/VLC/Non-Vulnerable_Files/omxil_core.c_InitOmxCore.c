#endif
int InitOmxCore(vlc_object_t *p_this)
{
    int i;
    vlc_mutex_lock( &omx_core_mutex );
    if( omx_refcount > 0 ) {
        omx_refcount++;
        vlc_mutex_unlock( &omx_core_mutex );
        return VLC_SUCCESS;
    }
#ifdef RPI_OMX
    /* Load an extra library first, if available */
    extra_dll_handle = NULL;
    for( i = 0; ppsz_extra_dll_list[i]; i++ )
    {
        extra_dll_handle = dll_open( ppsz_extra_dll_list[i] );
        if( extra_dll_handle ) break;
    }
    if( extra_dll_handle )
    {
        pf_host_init = dlsym( extra_dll_handle, "bcm_host_init" );
        pf_host_deinit = dlsym( extra_dll_handle, "bcm_host_deinit" );
        if (pf_host_init)
            pf_host_init();
    }
#endif
    /* Load the OMX core */
    for( i = 0; ppsz_dll_list[i]; i++ )
    {
        dll_handle = dll_open( ppsz_dll_list[i] );
        if( dll_handle ) break;
    }
    if( !dll_handle )
    {
        CloseExtraDll();
        vlc_mutex_unlock( &omx_core_mutex );
        return VLC_EGENERIC;
    }
    pf_init = dlsym( dll_handle, "OMX_Init" );
    pf_deinit = dlsym( dll_handle, "OMX_Deinit" );
    pf_get_handle = dlsym( dll_handle, "OMX_GetHandle" );
    pf_free_handle = dlsym( dll_handle, "OMX_FreeHandle" );
    pf_component_enum = dlsym( dll_handle, "OMX_ComponentNameEnum" );
    pf_get_roles_of_component = dlsym( dll_handle, "OMX_GetRolesOfComponent" );
    if( !pf_init || !pf_deinit || !pf_get_handle || !pf_free_handle ||
        !pf_component_enum || !pf_get_roles_of_component )
    {
        msg_Warn( p_this, "cannot find OMX_* symbols in `%s' (%s)",
                  ppsz_dll_list[i], dlerror() );
        dll_close(dll_handle);
        CloseExtraDll();
        vlc_mutex_unlock( &omx_core_mutex );
        return VLC_EGENERIC;
    }
    /* Initialise the OMX core */
    OMX_ERRORTYPE omx_error = pf_init();
    if(omx_error != OMX_ErrorNone)
    {
        msg_Warn( p_this, "OMX_Init failed (%x: %s)", omx_error,
                  ErrorToString(omx_error) );
        dll_close(dll_handle);
        CloseExtraDll();
        vlc_mutex_unlock( &omx_core_mutex );
        return VLC_EGENERIC;
    }
    omx_refcount++;
    vlc_mutex_unlock( &omx_core_mutex );
    return VLC_SUCCESS;
}
