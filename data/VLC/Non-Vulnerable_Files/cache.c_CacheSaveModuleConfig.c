}
static int CacheSaveModuleConfig (FILE *file, const module_t *module)
{
    uint16_t lines = module->confsize;
    SAVE_IMMEDIATE (module->i_config_items);
    SAVE_IMMEDIATE (module->i_bool_items);
    SAVE_IMMEDIATE (lines);
    for (size_t i = 0; i < lines; i++)
        if (CacheSaveConfig (file, module->p_config + i))
           goto error;
    return 0;
error:
    return -1;
}
