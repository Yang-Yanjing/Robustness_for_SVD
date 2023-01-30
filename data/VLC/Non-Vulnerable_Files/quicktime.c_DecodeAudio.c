 *****************************************************************************/
static block_t *DecodeAudio( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t     *p_block;
    int         i_error;
#ifdef LOADER
    /* We must do open and close in the same thread (unless we do
     * Setup_LDT_Keeper in the main thread before all others */
    if( p_sys == NULL )
    {
        if( OpenAudio( p_dec ) )
        {
            /* Fatal */
            p_dec->b_error = true;
            return NULL;
        }
        p_sys = p_dec->p_sys;
    }
#endif
    if( pp_block == NULL || *pp_block == NULL )
    {
        return NULL;
    }
    p_block = *pp_block;
    if( p_sys->i_out_frames > 0 && p_sys->i_out >= p_sys->i_out_frames )
    {
        /* Ask new data */
        p_sys->i_out = 0;
        p_sys->i_out_frames = 0;
        *pp_block = NULL;
        return NULL;
    }
    if( p_sys->i_out_frames <= 0 )
    {
        p_sys->pts = p_block->i_pts;
        mtime_t i_display_date = 0;
        if( !(p_block->i_flags & BLOCK_FLAG_PREROLL) )
            i_display_date = decoder_GetDisplayDate( p_dec, p_block->i_pts );
        if( i_display_date > 0 && i_display_date < mdate() )
        {
            block_Release( p_block );
            *pp_block = NULL;
            return NULL;
        }
        /* Append data */
        if( p_sys->i_buffer_size < p_sys->i_buffer + p_block->i_buffer )
        {
            p_sys->i_buffer_size = p_sys->i_buffer + p_block->i_buffer + 1024;
            p_sys->p_buffer = xrealloc( p_sys->p_buffer, p_sys->i_buffer_size );
        }
        memcpy( &p_sys->p_buffer[p_sys->i_buffer], p_block->p_buffer,
                p_block->i_buffer );
        p_sys->i_buffer += p_block->i_buffer;
        if( p_sys->i_buffer > p_sys->InFrameSize )
        {
            int i_frames = p_sys->i_buffer / p_sys->InFrameSize;
            unsigned long i_out_frames, i_out_bytes;
            vlc_mutex_lock( &qt_mutex );
            i_error = p_sys->SoundConverterConvertBuffer( p_sys->myConverter,
                                                          p_sys->p_buffer,
                                                          i_frames,
                                                          p_sys->out_buffer,
                                                          &i_out_frames,
                                                          &i_out_bytes );
            vlc_mutex_unlock( &qt_mutex );
            /*
            msg_Dbg( p_dec, "decoded %d frames -> %ld frames (error=%d)",
                     i_frames, i_out_frames, i_error );
            msg_Dbg( p_dec, "decoded %ld frames = %ld bytes",
                     i_out_frames, i_out_bytes );
            */
            p_sys->i_buffer -= i_frames * p_sys->InFrameSize;
            if( p_sys->i_buffer > 0 )
            {
                memmove( &p_sys->p_buffer[0],
                         &p_sys->p_buffer[i_frames * p_sys->InFrameSize],
                         p_sys->i_buffer );
            }
            if( p_sys->pts > VLC_TS_INVALID &&
                p_sys->pts != date_Get( &p_sys->date ) )
            {
                date_Set( &p_sys->date, p_sys->pts );
            }
            else if( !date_Get( &p_sys->date ) )
            {
                return NULL;
            }
            if( !i_error && i_out_frames > 0 )
            {
                /* we have others samples */
                p_sys->i_out_frames = i_out_frames;
                p_sys->i_out = 0;
            }
        }
    }
    if( p_sys->i_out < p_sys->i_out_frames )
    {
        block_t *p_out;
        int  i_frames = __MIN( p_sys->i_out_frames - p_sys->i_out, 1000 );
        p_out = decoder_NewAudioBuffer( p_dec, i_frames );
        if( p_out )
        {
            p_out->i_pts = date_Get( &p_sys->date );
            p_out->i_length = date_Increment( &p_sys->date, i_frames )
                              - p_out->i_pts;
            memcpy( p_out->p_buffer,
                    &p_sys->out_buffer[2 * p_sys->i_out * p_dec->fmt_out.audio.i_channels],
                    p_out->i_buffer );
            p_sys->i_out += i_frames;
        }
        return p_out;
    }
    return NULL;
}
