}
void libvlc_log_get_object(const libvlc_log_t *ctx,
                           const char **restrict name,
                           const char **restrict header,
                           uintptr_t *restrict id)
{
    if (name != NULL)
        *name = (ctx->psz_object_type != NULL)
                ? ctx->psz_object_type : "generic";
    if (header != NULL)
        *header = ctx->psz_header;
    if (id != NULL)
        *id = ctx->i_object_id;
}
