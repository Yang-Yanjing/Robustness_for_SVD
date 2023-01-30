 *****************************************************************************/
static void ESNew( demux_t *p_demux, int i_id )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    ps_track_t  *tk = &p_sys->tk[PS_ID_TO_TK(i_id)];
    bool  b_select = false;
    if( tk->b_seen ) return;
    if( ps_track_fill( tk, 0, i_id ) )
    {
        msg_Warn( p_demux, "unknown codec for id=0x%x", i_id );
        return;
    }
    /* Add a new ES */
    if( tk->fmt.i_cat == VIDEO_ES )
    {
        tk->fmt.video.i_sar_num = p_sys->sar.i_num;
        tk->fmt.video.i_sar_den = p_sys->sar.i_den;
        b_select = true;
    }
    else if( tk->fmt.i_cat == AUDIO_ES )
    {
        int i_audio = -1;
        /* find the audio number PLEASE find another way */
        if( (i_id&0xbdf8) == 0xbd88 )       /* dts */
        {
            i_audio = i_id&0x07;
        }
        else if( (i_id&0xbdf0) == 0xbd80 )  /* a52 */
        {
            i_audio = i_id&0xf;
        }
        else if( (i_id&0xbdf0) == 0xbda0 )  /* lpcm */
        {
            i_audio = i_id&0x1f;
        }
        else if( ( i_id&0xe0 ) == 0xc0 )    /* mpga */
        {
            i_audio = i_id&0x1f;
        }
        if( i_audio >= 0 )
        {
            int i_lang = dvdnav_audio_stream_to_lang( p_sys->dvdnav, i_audio );
            if( i_lang != 0xffff )
            {
                tk->fmt.psz_language = malloc( 3 );
                tk->fmt.psz_language[0] = (i_lang >> 8)&0xff;
                tk->fmt.psz_language[1] = (i_lang     )&0xff;
                tk->fmt.psz_language[2] = 0;
            }
            if( dvdnav_get_active_audio_stream( p_sys->dvdnav ) == i_audio )
            {
                b_select = true;
            }
        }
    }
    else if( tk->fmt.i_cat == SPU_ES )
    {
        int32_t i_title, i_part;
        int i_lang = dvdnav_spu_stream_to_lang( p_sys->dvdnav, i_id&0x1f );
        if( i_lang != 0xffff )
        {
            tk->fmt.psz_language = malloc( 3 );
            tk->fmt.psz_language[0] = (i_lang >> 8)&0xff;
            tk->fmt.psz_language[1] = (i_lang     )&0xff;
            tk->fmt.psz_language[2] = 0;
        }
        /* Palette */
        tk->fmt.subs.spu.palette[0] = 0xBeef;
        memcpy( &tk->fmt.subs.spu.palette[1], p_sys->clut,
                16 * sizeof( uint32_t ) );
        /* We select only when we are not in the menu */
        dvdnav_current_title_info( p_sys->dvdnav, &i_title, &i_part );
        if( i_title > 0 &&
            dvdnav_get_active_spu_stream( p_sys->dvdnav ) == (i_id&0x1f) )
        {
            b_select = true;
        }
    }
    tk->es = es_out_Add( p_demux->out, &tk->fmt );
    if( b_select )
    {
        es_out_Control( p_demux->out, ES_OUT_SET_ES, tk->es );
    }
    tk->b_seen = true;
    if( tk->fmt.i_cat == VIDEO_ES ) ButtonUpdate( p_demux, false );
}
