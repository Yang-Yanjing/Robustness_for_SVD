}
static bool TsStorageIsFull( ts_storage_t *p_storage, const ts_cmd_t *p_cmd )
{
    if( p_cmd && p_cmd->i_type == C_SEND && p_storage->i_cmd_w > 0 )
    {
        size_t i_size = sizeof(*p_cmd->u.send.p_block) + p_cmd->u.send.p_block->i_buffer;
        if( p_storage->i_file_size + i_size >= p_storage->i_file_max )
            return true;
    }
    return p_storage->i_cmd_w >= p_storage->i_cmd_max;
}
