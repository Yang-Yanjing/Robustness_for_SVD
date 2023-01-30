}
static void TsStoragePack( ts_storage_t *p_storage )
{
    /* Try to release a bit of memory */
    if( p_storage->i_cmd_w >= p_storage->i_cmd_max )
        return;
    p_storage->i_cmd_max = __MAX( p_storage->i_cmd_w, 1 );
    ts_cmd_t *p_new = realloc( p_storage->p_cmd, p_storage->i_cmd_max * sizeof(*p_storage->p_cmd) );
    if( p_new )
        p_storage->p_cmd = p_new;
}
