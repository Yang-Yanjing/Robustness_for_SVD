}
static void ESSubtitleUpdate( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    int         i_spu = dvdnav_get_active_spu_stream( p_sys->dvdnav );
    int32_t i_title, i_part;
    ButtonUpdate( p_demux, false );
    dvdnav_current_title_info( p_sys->dvdnav, &i_title, &i_part );
    if( i_title > 0 ) return;
    if( i_spu >= 0 && i_spu <= 0x1f )
    {
        ps_track_t *tk = &p_sys->tk[PS_ID_TO_TK(0xbd20 + i_spu)];
        ESNew( p_demux, 0xbd20 + i_spu );
        /* be sure to unselect it (reset) */
        es_out_Control( p_demux->out, ES_OUT_SET_ES_STATE, tk->es,
                        (bool)false );
        /* now select it */
        es_out_Control( p_demux->out, ES_OUT_SET_ES, tk->es );
    }
    else
    {
        for( i_spu = 0; i_spu <= 0x1F; i_spu++ )
        {
            ps_track_t *tk = &p_sys->tk[PS_ID_TO_TK(0xbd20 + i_spu)];
            if( tk->b_seen )
            {
                es_out_Control( p_demux->out, ES_OUT_SET_ES_STATE, tk->es,
                                (bool)false );
            }
        }
    }
}
