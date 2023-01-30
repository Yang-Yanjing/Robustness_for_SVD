}
static int CmdInitDel( ts_cmd_t *p_cmd, es_out_id_t *p_es )
{
    p_cmd->i_type = C_DEL;
    p_cmd->i_date = mdate();
    p_cmd->u.del.p_es = p_es;
    return VLC_SUCCESS;
}
