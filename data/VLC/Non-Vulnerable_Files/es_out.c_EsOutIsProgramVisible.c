}
static bool EsOutIsProgramVisible( es_out_t *out, int i_group )
{
    return out->p_sys->i_group_id == 0 || out->p_sys->i_group_id == i_group;
}
