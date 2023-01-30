}
static void CmdCleanControl( ts_cmd_t *p_cmd )
{
    if( ( p_cmd->u.control.i_query == ES_OUT_SET_GROUP_META ||
          p_cmd->u.control.i_query == ES_OUT_SET_META ) &&
        p_cmd->u.control.u.int_meta.p_meta )
    {
        vlc_meta_Delete( p_cmd->u.control.u.int_meta.p_meta );
    }
    else if( p_cmd->u.control.i_query == ES_OUT_SET_GROUP_EPG &&
             p_cmd->u.control.u.int_epg.p_epg )
    {
        vlc_epg_Delete( p_cmd->u.control.u.int_epg.p_epg );
    }
    else if( p_cmd->u.control.i_query == ES_OUT_SET_ES_FMT &&
             p_cmd->u.control.u.es_fmt.p_fmt )
    {
        es_format_Clean( p_cmd->u.control.u.es_fmt.p_fmt );
        free( p_cmd->u.control.u.es_fmt.p_fmt );
    }
}
