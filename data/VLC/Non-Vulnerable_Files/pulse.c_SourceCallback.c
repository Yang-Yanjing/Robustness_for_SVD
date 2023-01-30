}
static void SourceCallback(pa_context *ctx, const pa_source_info *i, int eol,
                           void *userdata)
{
    services_discovery_t *sd = userdata;
    if (eol)
        return;
    AddSource (sd, i);
    (void) ctx;
}
