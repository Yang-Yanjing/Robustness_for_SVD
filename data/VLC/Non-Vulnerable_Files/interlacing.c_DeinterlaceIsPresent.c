}
static bool DeinterlaceIsPresent(vout_thread_t *vout)
{
    char *filter = var_GetNonEmptyString(vout, "video-filter");
    bool is_found = FilterFind(filter, "deinterlace") != NULL;
    free(filter);
    return is_found;
}
