 *****************************************************************************/
static BOOL WINAPI DirectXEnumCallback2(GUID *guid, LPSTR desc,
                                        LPSTR drivername, VOID *data,
                                        HMONITOR hmon)
{
    enum_context_t *ctx = data;
    VLC_UNUSED(guid); VLC_UNUSED(desc); VLC_UNUSED(hmon);
    char *psz_drivername = drivername;
    ctx->values = xrealloc(ctx->values, (ctx->count + 1) * sizeof(char *));
    ctx->descs = xrealloc(ctx->descs, (ctx->count + 1) * sizeof(char *));
    ctx->values[ctx->count] = strdup(psz_drivername);
    ctx->descs[ctx->count] = strdup(psz_drivername);
    ctx->count++;
    return TRUE; /* Keep enumerating */
}
