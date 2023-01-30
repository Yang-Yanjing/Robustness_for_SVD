 */
void vlc_module_unload(module_t *module, vlc_deactivate_t deinit, ...)
{
    if (module->pf_deactivate != NULL)
    {
        va_list ap;
        va_start(ap, deinit);
        deinit(module->pf_deactivate, ap);
        va_end(ap);
    }
}
