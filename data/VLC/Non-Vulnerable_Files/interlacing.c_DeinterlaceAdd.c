}
static void DeinterlaceAdd(vout_thread_t *vout)
{
    char *filter = var_GetNonEmptyString(vout, "video-filter");
    if (FilterFind(filter, "deinterlace")) {
        free(filter);
        return;
    }
    /* */
    if (filter) {
        char *tmp = filter;
        if (asprintf(&filter, "%s:%s", tmp, "deinterlace") < 0)
            filter = tmp;
        else
            free(tmp);
    } else {
        filter = strdup("deinterlace");
    }
    if (filter) {
        var_SetString(vout, "video-filter", filter);
        free(filter);
    }
}
