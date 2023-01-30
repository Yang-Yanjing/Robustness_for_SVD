}
static void TsStoragePushCmd( ts_storage_t *p_storage, const ts_cmd_t *p_cmd, bool b_flush )
{
    ts_cmd_t cmd = *p_cmd;
    assert( !TsStorageIsFull( p_storage, p_cmd ) );
    if( cmd.i_type == C_SEND )
    {
        block_t *p_block = cmd.u.send.p_block;
        cmd.u.send.p_block = NULL;
        cmd.u.send.i_offset = ftell( p_storage->p_filew );
        if( fwrite( p_block, sizeof(*p_block), 1, p_storage->p_filew ) != 1 )
        {
            block_Release( p_block );
            return;
        }
        p_storage->i_file_size += sizeof(*p_block);
        if( p_block->i_buffer > 0 )
        {
            if( fwrite( p_block->p_buffer, p_block->i_buffer, 1, p_storage->p_filew ) != 1 )
            {
                block_Release( p_block );
                return;
            }
        }
        p_storage->i_file_size += p_block->i_buffer;
        block_Release( p_block );
        if( b_flush )
            fflush( p_storage->p_filew );
    }
    p_storage->p_cmd[p_storage->i_cmd_w++] = cmd;
}
