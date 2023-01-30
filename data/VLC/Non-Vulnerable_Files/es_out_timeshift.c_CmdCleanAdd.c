}
static void CmdCleanAdd( ts_cmd_t *p_cmd )
{
    es_format_Clean( p_cmd->u.add.p_fmt );
    free( p_cmd->u.add.p_fmt );
}
