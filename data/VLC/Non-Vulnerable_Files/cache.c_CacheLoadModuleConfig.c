}
static int CacheLoadModuleConfig (module_t *module, FILE *file)
{
    uint16_t lines;
    /* Calculate the structure length */
    LOAD_IMMEDIATE (module->i_config_items);
    LOAD_IMMEDIATE (module->i_bool_items);
    LOAD_IMMEDIATE (lines);
    /* Allocate memory */
    if (lines)
    {
        module->p_config = malloc (lines * sizeof (module_config_t));
        if (unlikely(module->p_config == NULL))
        {
            module->confsize = 0;
            return -1;
        }
    }
    else
        module->p_config = NULL;
    module->confsize = lines;
    /* Do the duplication job */
    for (size_t i = 0; i < lines; i++)
        if (CacheLoadConfig (module->p_config + i, file))
            return -1;
    return 0;
error:
    return -1; /* FIXME: leaks */
}
