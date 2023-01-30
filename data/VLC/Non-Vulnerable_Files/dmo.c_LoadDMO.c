 *****************************************************************************/
static int LoadDMO( vlc_object_t *p_this, HINSTANCE *p_hmsdmo_dll,
                    IMediaObject **pp_dmo, es_format_t *p_fmt,
                    bool b_out )
{
    DMO_PARTIAL_MEDIATYPE dmo_partial_type;
    int i_err;
    long (STDCALL *OurDMOEnum)( const GUID *, uint32_t, uint32_t,
                               const DMO_PARTIAL_MEDIATYPE *,
                               uint32_t, const DMO_PARTIAL_MEDIATYPE *,
                               IEnumDMO ** );
    IEnumDMO *p_enum_dmo = NULL;
    WCHAR *psz_dmo_name;
    GUID clsid_dmo;
    uint32_t i_dummy;
    GETCLASS GetClass;
    IClassFactory *cFactory = NULL;
    IUnknown *cObject = NULL;
    const codec_dll *codecs_table = b_out ? encoders_table : decoders_table;
    int i_codec;
    /* Look for a DMO which can handle the requested codec */
    if( p_fmt->i_cat == AUDIO_ES )
    {
        uint16_t i_tag;
        dmo_partial_type.type = MEDIATYPE_Audio;
        dmo_partial_type.subtype = dmo_partial_type.type;
        dmo_partial_type.subtype.Data1 = p_fmt->i_original_fourcc ?: p_fmt->i_codec;
        fourcc_to_wf_tag( p_fmt->i_codec, &i_tag );
        if( i_tag ) dmo_partial_type.subtype.Data1 = i_tag;
    }
    else
    {
        dmo_partial_type.type = MEDIATYPE_Video;
        dmo_partial_type.subtype = dmo_partial_type.type;
        dmo_partial_type.subtype.Data1 = p_fmt->i_original_fourcc ?: p_fmt->i_codec;
    }
    /* Load msdmo DLL */
    *p_hmsdmo_dll = LoadLibraryA( "msdmo.dll" );
    if( *p_hmsdmo_dll == NULL )
    {
        msg_Dbg( p_this, "failed loading msdmo.dll" );
        return VLC_EGENERIC;
    }
    OurDMOEnum = (void *)GetProcAddress( *p_hmsdmo_dll, "DMOEnum" );
    if( OurDMOEnum == NULL )
    {
        msg_Dbg( p_this, "GetProcAddress failed to find DMOEnum()" );
        FreeLibrary( *p_hmsdmo_dll );
        return VLC_EGENERIC;
    }
    if( !b_out )
    {
        i_err = OurDMOEnum( &GUID_NULL, 1 /*DMO_ENUMF_INCLUDE_KEYED*/,
                            1, &dmo_partial_type, 0, NULL, &p_enum_dmo );
    }
    else
    {
        i_err = OurDMOEnum( &GUID_NULL, 1 /*DMO_ENUMF_INCLUDE_KEYED*/,
                            0, NULL, 1, &dmo_partial_type, &p_enum_dmo );
    }
    if( i_err )
    {
        FreeLibrary( *p_hmsdmo_dll );
        /* return VLC_EGENERIC; */
        /* Try loading the dll directly */
        goto loader;
    }
    /* Pickup the first available codec */
    *pp_dmo = 0;
    while( ( S_OK == p_enum_dmo->vt->Next( p_enum_dmo, 1, &clsid_dmo,
                     &psz_dmo_name, &i_dummy /* NULL doesn't work */ ) ) )
    {
        char *psz_temp = FromWide( psz_dmo_name );
        msg_Dbg( p_this, "found DMO: %s", psz_temp );
        CoTaskMemFree( psz_dmo_name );
        /* Create DMO */
        if( CoCreateInstance( &clsid_dmo, NULL, CLSCTX_INPROC,
                              &IID_IMediaObject, (void **)pp_dmo ) )
        {
            msg_Warn( p_this, "can't create DMO: %s", psz_temp );
            free( psz_temp );
            *pp_dmo = 0;
        }
        else
        {
            free( psz_temp );
            break;
        }
    }
    p_enum_dmo->vt->Release( (IUnknown *)p_enum_dmo );
    if( !*pp_dmo )
    {
        FreeLibrary( *p_hmsdmo_dll );
        /* return VLC_EGENERIC; */
        /* Try loading the dll directly */
        goto loader;
    }
    return VLC_SUCCESS;
loader:
    for( i_codec = 0; codecs_table[i_codec].i_fourcc != 0; i_codec++ )
    {
        if( codecs_table[i_codec].i_fourcc == p_fmt->i_codec )
            break;
    }
    if( codecs_table[i_codec].i_fourcc == 0 )
        return VLC_EGENERIC;    /* Can't happen */
    *p_hmsdmo_dll = LoadLibraryA( codecs_table[i_codec].psz_dll );
    if( *p_hmsdmo_dll == NULL )
    {
        msg_Dbg( p_this, "failed loading '%s'",
                 codecs_table[i_codec].psz_dll );
        return VLC_EGENERIC;
    }
    GetClass = (GETCLASS)GetProcAddress( *p_hmsdmo_dll, "DllGetClassObject" );
    if (!GetClass)
    {
        msg_Dbg( p_this, "GetProcAddress failed to find DllGetClassObject()" );
        FreeLibrary( *p_hmsdmo_dll );
        return VLC_EGENERIC;
    }
    i_err = GetClass( codecs_table[i_codec].p_guid, &IID_IClassFactory,
                      (void**)&cFactory );
    if( i_err || cFactory == NULL )
    {
        msg_Dbg( p_this, "no such class object" );
        FreeLibrary( *p_hmsdmo_dll );
        return VLC_EGENERIC;
    }
    i_err = cFactory->vt->CreateInstance( cFactory, 0, &IID_IUnknown,
                                          (void**)&cObject );
    cFactory->vt->Release( (IUnknown*)cFactory );
    if( i_err || !cObject )
    {
        msg_Dbg( p_this, "class factory failure" );
        FreeLibrary( *p_hmsdmo_dll );
        return VLC_EGENERIC;
    }
    i_err = cObject->vt->QueryInterface( cObject, &IID_IMediaObject,
                                        (void**)pp_dmo );
    cObject->vt->Release( (IUnknown*)cObject );
    if( i_err || !*pp_dmo )
    {
        msg_Dbg( p_this, "QueryInterface failure" );
        FreeLibrary( *p_hmsdmo_dll );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
