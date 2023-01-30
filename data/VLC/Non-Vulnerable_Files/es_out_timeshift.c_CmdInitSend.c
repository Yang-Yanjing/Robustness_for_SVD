}
static void CmdInitSend( ts_cmd_t *p_cmd, es_out_id_t *p_es, block_t *p_block )
{
    p_cmd->i_type = C_SEND;
    p_cmd->i_date = mdate();
    p_cmd->u.send.p_es = p_es;
    p_cmd->u.send.p_block = p_block;
}
