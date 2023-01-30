}
static int CmdInitAdd( ts_cmd_t *p_cmd, es_out_id_t *p_es, const es_format_t *p_fmt, bool b_copy )
{
    p_cmd->i_type = C_ADD;
    p_cmd->i_date = mdate();
    p_cmd->u.add.p_es = p_es;
    if( b_copy )
    {
        p_cmd->u.add.p_fmt = malloc( sizeof(*p_fmt) );
        if( !p_cmd->u.add.p_fmt )
            return VLC_EGENERIC;
        es_format_Copy( p_cmd->u.add.p_fmt, p_fmt );
    }
    else
    {
        p_cmd->u.add.p_fmt = (es_format_t*)p_fmt;
    }
    return VLC_SUCCESS;
}
