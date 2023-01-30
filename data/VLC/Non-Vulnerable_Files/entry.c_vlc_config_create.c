}
static module_config_t *vlc_config_create (module_t *module, int type)
{
    unsigned confsize = module->confsize;
    module_config_t *tab = module->p_config;
    if ((confsize & 0xf) == 0)
    {
        tab = realloc_or_free (tab, (confsize + 17) * sizeof (*tab));
        if (tab == NULL)
            return NULL;
        module->p_config = tab;
    }
    memset (tab + confsize, 0, sizeof (tab[confsize]));
    if (IsConfigIntegerType (type))
    {
        tab[confsize].max.i = INT_MAX;
        tab[confsize].min.i = INT_MIN;
    }
    tab[confsize].i_type = type;
    if (CONFIG_ITEM(type))
    {
        module->i_config_items++;
        if (type == CONFIG_ITEM_BOOL)
            module->i_bool_items++;
    }
    module->confsize++;
    return tab + confsize;
}
