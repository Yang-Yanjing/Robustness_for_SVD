}
static bool module_show(const module_t *m, bool advanced)
{
    for (size_t i = 0; i < m->confsize; i++)
    {
        const module_config_t *item = m->p_config + i;
        if (!CONFIG_ITEM(item->i_type))
            continue;
        if (item->b_removed)
            continue;
        if ((!advanced) && item->b_advanced)
            continue;
        return true;
    }
    return false;
}
