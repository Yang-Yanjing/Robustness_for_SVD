 *****************************************************************************/
static void GetAR( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    int i_old_sar_num = p_sys->i_sar_num;
    int i_old_sar_den = p_sys->i_sar_den;
    /* Check whether the input gave a particular aspect ratio */
    if( p_dec->fmt_in.video.i_sar_num > 0 &&
        p_dec->fmt_in.video.i_sar_den > 0 )
    {
        p_sys->i_sar_num = p_dec->fmt_in.video.i_sar_num;
        p_sys->i_sar_den = p_dec->fmt_in.video.i_sar_den;
    }
    else
    {
        /* Use the value provided in the MPEG sequence header */
        if( p_sys->p_info->sequence->pixel_height > 0 )
        {
            p_sys->i_sar_num = p_sys->p_info->sequence->pixel_width;
            p_sys->i_sar_den = p_sys->p_info->sequence->pixel_height;
        }
        else
        {
            /* Invalid aspect, assume 4:3.
             * This shouldn't happen and if it does it is a bug
             * in libmpeg2 (likely triggered by an invalid stream) */
            p_sys->i_sar_num = p_sys->p_info->sequence->picture_height * 4;
            p_sys->i_sar_den = p_sys->p_info->sequence->picture_width * 3;
        }
    }
    if( p_sys->i_sar_num == i_old_sar_num &&
        p_sys->i_sar_den == i_old_sar_den )
        return;
    if( p_sys->p_info->sequence->frame_period > 0 )
        msg_Dbg( p_dec,
                 "%dx%d (display %d,%d), sar %i:%i, %u.%03u fps",
                 p_sys->p_info->sequence->picture_width,
                 p_sys->p_info->sequence->picture_height,
                 p_sys->p_info->sequence->display_width,
                 p_sys->p_info->sequence->display_height,
                 p_sys->i_sar_num, p_sys->i_sar_den,
                 (uint32_t)((uint64_t)1001000000 * 27 /
                     p_sys->p_info->sequence->frame_period / 1001),
                 (uint32_t)((uint64_t)1001000000 * 27 /
                     p_sys->p_info->sequence->frame_period % 1001) );
    else
        msg_Dbg( p_dec, "bad frame period" );
}
