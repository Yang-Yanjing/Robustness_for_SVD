}
static bool TsStorageIsEmpty( ts_storage_t *p_storage )
{
    return !p_storage || p_storage->i_cmd_r >= p_storage->i_cmd_w;
}
