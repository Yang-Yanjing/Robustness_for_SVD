}
static void CmdExecuteAdd( es_out_t *p_out, ts_cmd_t *p_cmd )
{
    p_cmd->u.add.p_es->p_es = es_out_Add( p_out, p_cmd->u.add.p_fmt );
}
