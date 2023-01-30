 *****************************************************************************/
static int EncOpen( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t*)p_this;
    encoder_sys_t *p_sys = NULL;
    IMediaObject *p_dmo = NULL;
    HINSTANCE hmsdmo_dll = NULL;
    /* Initialize OLE/COM */
    CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
    if( LoadDMO( p_this, &hmsdmo_dll, &p_dmo, &p_enc->fmt_out, true )
        != VLC_SUCCESS )
    {
        hmsdmo_dll = 0;
        p_dmo = 0;
        goto error;
    }
    if( p_enc->fmt_in.i_cat == VIDEO_ES )
    {
        if( EncoderSetVideoType( p_enc, p_dmo ) != VLC_SUCCESS ) goto error;
    }
    else
    {
        if( EncoderSetAudioType( p_enc, p_dmo ) != VLC_SUCCESS ) goto error;
    }
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_enc->p_sys = p_sys = malloc(sizeof(*p_sys)) ) == NULL )
    {
        goto error;
    }
    p_sys->hmsdmo_dll = hmsdmo_dll;
    p_sys->p_dmo = p_dmo;
    /* Find out some properties of the inputput */
    {
        uint32_t i_size, i_align, dum;
        if( p_dmo->vt->GetInputSizeInfo( p_dmo, 0, &i_size, &i_align, &dum ) )
            msg_Err( p_enc, "GetInputSizeInfo() failed" );
        else
            msg_Dbg( p_enc, "GetInputSizeInfo(): bytes %i, align %i, %i",
                     i_size, i_align, dum );
    }
    /* Find out some properties of the output */
    {
        uint32_t i_size, i_align;
        p_sys->i_min_output = 0;
        if( p_dmo->vt->GetOutputSizeInfo( p_dmo, 0, &i_size, &i_align ) )
        {
            msg_Err( p_enc, "GetOutputSizeInfo() failed" );
            goto error;
        }
        else
        {
            msg_Dbg( p_enc, "GetOutputSizeInfo(): bytes %i, align %i",
                     i_size, i_align );
            p_sys->i_min_output = i_size;
        }
    }
    /* Set output properties */
    p_enc->fmt_out.i_cat = p_enc->fmt_out.i_cat;
    if( p_enc->fmt_out.i_cat == AUDIO_ES )
        date_Init( &p_sys->end_date, p_enc->fmt_out.audio.i_rate, 1 );
    else
        date_Init( &p_sys->end_date, 25 /* FIXME */, 1 );
    return VLC_SUCCESS;
 error:
    if( p_dmo ) p_dmo->vt->Release( (IUnknown *)p_dmo );
    if( hmsdmo_dll ) FreeLibrary( hmsdmo_dll );
    /* Uninitialize OLE/COM */
    CoUninitialize();
    free( p_sys );
    return VLC_EGENERIC;
}
