#define DEMUX_INDEX_SIZE_MAX (100000)
static void demux_IndexInit( demux_index_t *p_idx )
{
    p_idx->i_idx = 0;
    p_idx->i_idx_max = 0;
    p_idx->idx = NULL;
}
