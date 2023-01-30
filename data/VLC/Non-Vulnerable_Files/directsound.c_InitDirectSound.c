 */
static int InitDirectSound( audio_output_t *p_aout )
{
    aout_sys_t *sys = p_aout->sys;
    GUID guid, *p_guid = NULL;
    HRESULT (WINAPI *OurDirectSoundCreate)(LPGUID, LPDIRECTSOUND *, LPUNKNOWN);
    OurDirectSoundCreate = (void *)
        GetProcAddress( p_aout->sys->hdsound_dll,
                        "DirectSoundCreate" );
    if( OurDirectSoundCreate == NULL )
    {
        msg_Warn( p_aout, "GetProcAddress FAILED" );
        goto error;
    }
    char *dev = var_GetNonEmptyString( p_aout, "directx-audio-device" );
    if( dev != NULL )
    {
        LPOLESTR lpsz = ToWide( dev );
        free( dev );
        if( SUCCEEDED( IIDFromString( lpsz, &guid ) ) )
            p_guid = &guid;
        else
            msg_Err( p_aout, "bad device GUID: %ls", lpsz );
        free( lpsz );
    }
    /* Create the direct sound object */
    if FAILED( OurDirectSoundCreate( p_guid, &sys->s.p_dsobject, NULL ) )
    {
        msg_Warn( p_aout, "cannot create a direct sound device" );
        goto error;
    }
    return VLC_SUCCESS;
error:
    sys->s.p_dsobject = NULL;
    return VLC_EGENERIC;
}
