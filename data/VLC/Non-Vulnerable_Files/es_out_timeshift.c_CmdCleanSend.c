}
static void CmdCleanSend( ts_cmd_t *p_cmd )
{
    if( p_cmd->u.send.p_block )
        block_Release( p_cmd->u.send.p_block );
}
