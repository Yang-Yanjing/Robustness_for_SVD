 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    /* get lock, avoid segfault */
    vlc_mutex_lock( &qt_mutex );
    if( p_dec->fmt_out.i_cat == AUDIO_ES )
    {
        int i_error;
        unsigned long ConvertedFrames=0;
        unsigned long ConvertedBytes=0;
        i_error = p_sys->SoundConverterEndConversion( p_sys->myConverter, NULL,
                                                      &ConvertedFrames,
                                                      &ConvertedBytes );
        msg_Dbg( p_dec, "SoundConverterEndConversion => %d", i_error );
        i_error = p_sys->SoundConverterClose( p_sys->myConverter );
        msg_Dbg( p_dec, "SoundConverterClose => %d", i_error );
        free( p_sys->p_buffer );
    }
    else if( p_dec->fmt_out.i_cat == VIDEO_ES )
    {
        free( p_sys->plane );
    }
#ifndef __APPLE__
    FreeLibrary( p_sys->qtml );
    FreeLibrary( p_sys->qts );
    msg_Dbg( p_dec, "FreeLibrary ok." );
#else
    ExitMovies();
#endif
#if 0
    /* Segfault */
#ifdef LOADER
    Restore_LDT_Keeper( p_sys->ldt_fs );
    msg_Dbg( p_dec, "Restore_LDT_Keeper" );
#endif
#endif
    vlc_mutex_unlock( &qt_mutex );
    free( p_sys );
}
