}
void video_format_Print( vlc_object_t *p_this,
                         const char *psz_text, const video_format_t *fmt )
{
    msg_Dbg( p_this,
             "%s sz %ix%i, of (%i,%i), vsz %ix%i, 4cc %4.4s, sar %i:%i, msk r0x%x g0x%x b0x%x",
             psz_text,
             fmt->i_width, fmt->i_height, fmt->i_x_offset, fmt->i_y_offset,
             fmt->i_visible_width, fmt->i_visible_height,
             (char*)&fmt->i_chroma,
             fmt->i_sar_num, fmt->i_sar_den,
             fmt->i_rmask, fmt->i_gmask, fmt->i_bmask );
}
