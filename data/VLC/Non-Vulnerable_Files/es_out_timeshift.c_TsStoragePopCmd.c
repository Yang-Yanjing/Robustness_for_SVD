}
static void TsStoragePopCmd( ts_storage_t *p_storage, ts_cmd_t *p_cmd, bool b_flush )
{
    assert( !TsStorageIsEmpty( p_storage ) );
    *p_cmd = p_storage->p_cmd[p_storage->i_cmd_r++];
    if( p_cmd->i_type == C_SEND )
    {
        block_t block;
        if( !b_flush &&
            !fseek( p_storage->p_filer, p_cmd->u.send.i_offset, SEEK_SET ) &&
            fread( &block, sizeof(block), 1, p_storage->p_filer ) == 1 )
        {
            block_t *p_block = block_Alloc( block.i_buffer );
            if( p_block )
            {
                p_block->i_dts      = block.i_dts;
                p_block->i_pts      = block.i_pts;
                p_block->i_flags    = block.i_flags;
                p_block->i_length   = block.i_length;
                p_block->i_nb_samples = block.i_nb_samples;
                p_block->i_buffer = fread( p_block->p_buffer, 1, block.i_buffer, p_storage->p_filer );
            }
            p_cmd->u.send.p_block = p_block;
        }
        else
        {
            //perror( "TsStoragePopCmd" );
            p_cmd->u.send.p_block = block_Alloc( 1 );
        }
    }
}
