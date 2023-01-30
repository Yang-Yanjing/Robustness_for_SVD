/*** Logging core dispatcher ***/
void libvlc_log_get_context(const libvlc_log_t *ctx,
                            const char **restrict module,
                            const char **restrict file,
                            unsigned *restrict line)
{
    if (module != NULL)
        *module = ctx->psz_module;
    if (file != NULL)
        *file = NULL;
    if (line != NULL)
        *line = 0;
}
