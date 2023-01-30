}
static subpicture_t *Subtitle( decoder_t *p_dec, char *psz_subtitle, char *psz_html, mtime_t i_pts )
{
    //decoder_sys_t *p_sys = p_dec->p_sys;
    subpicture_t *p_spu = NULL;
    /* We cannot display a subpicture with no date */
    if( i_pts <= VLC_TS_INVALID )
    {
        msg_Warn( p_dec, "subtitle without a date" );
        free( psz_subtitle );
        free( psz_html );
        return NULL;
    }
    EnsureUTF8( psz_subtitle );
    if( psz_html )
        EnsureUTF8( psz_html );
    /* Create the subpicture unit */
    p_spu = decoder_NewSubpictureText( p_dec );
    if( !p_spu )
    {
        free( psz_subtitle );
        free( psz_html );
        return NULL;
    }
    p_spu->i_start    = i_pts;
    p_spu->i_stop     = i_pts + 10000000;   /* 10s max */
    p_spu->b_ephemer  = true;
    p_spu->b_absolute = false;
    subpicture_updater_sys_t *p_spu_sys = p_spu->updater.p_sys;
    /* The "leavetext" alignment is a special mode where the subpicture
       region itself gets aligned, but the text inside it does not */
    p_spu_sys->align = SUBPICTURE_ALIGN_LEAVETEXT;
    p_spu_sys->text  = psz_subtitle;
    p_spu_sys->html  = psz_html;
    p_spu_sys->i_font_height_percent = 5;
    p_spu_sys->renderbg = true;
    return p_spu;
}
