 *****************************************************************************/
static picture_t *DecodeVideo( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t       *p_block;
    picture_t     *p_pic;
    mtime_t       i_pts;
    ComponentResult cres;
#ifdef LOADER
    /* We must do open and close in the same thread (unless we do
     * Setup_LDT_Keeper in the main thread before all others */
    if( p_sys == NULL )
    {
        if( OpenVideo( p_dec ) )
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
    *pp_block = NULL;
    i_pts = p_block->i_pts > VLC_TS_INVALID ? p_block->i_pts : p_block->i_dts;
    mtime_t i_display_date = 0;
    if( !(p_block->i_flags & BLOCK_FLAG_PREROLL) )
        i_display_date = decoder_GetDisplayDate( p_dec, i_pts );
    if( i_display_date > 0 && i_display_date < mdate() )
    {
        p_sys->i_late++;
    }
    else
    {
        p_sys->i_late = 0;
    }
#ifndef NDEBUG
    msg_Dbg( p_dec, "bufsize: %zu", p_block->i_buffer);
#endif
    if( p_sys->i_late > 10 )
    {
        msg_Dbg( p_dec, "late buffer dropped (%"PRId64")", i_pts );
        block_Release( p_block );
        return NULL;
    }
    vlc_mutex_lock( &qt_mutex );
    if( ( p_pic = decoder_NewPicture( p_dec ) ) )
    {
        p_sys->decpar.data                  = (Ptr)p_block->p_buffer;
        p_sys->decpar.bufferSize            = p_block->i_buffer;
        (**p_sys->framedescHandle).dataSize = p_block->i_buffer;
        cres = p_sys->ImageCodecBandDecompress( p_sys->ci, &p_sys->decpar );
        ++p_sys->decpar.frameNumber;
        if( cres &0xFFFF )
        {
            msg_Dbg( p_dec, "quicktime_video: ImageCodecBandDecompress"
                     " cres=0x%X (-0x%X) %d :(",
                     (int)cres,(int)-cres, (int)cres );
        }
        memcpy( p_pic->p[0].p_pixels, p_sys->plane,
                p_dec->fmt_in.video.i_width * p_dec->fmt_in.video.i_height * 2 );
        p_pic->date = i_pts;
    }
    vlc_mutex_unlock( &qt_mutex );
    block_Release( p_block );
    return p_pic;
}
