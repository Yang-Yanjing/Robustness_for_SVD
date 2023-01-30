}
static int CmdExecuteSend( es_out_t *p_out, ts_cmd_t *p_cmd )
{
    block_t *p_block = p_cmd->u.send.p_block;
    p_cmd->u.send.p_block = NULL;
    if( p_block )
    {
        if( p_cmd->u.send.p_es->p_es )
            return es_out_Send( p_out, p_cmd->u.send.p_es->p_es, p_block );
        block_Release( p_block );
    }
    return VLC_EGENERIC;
}
