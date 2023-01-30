}
static void DeinterlaceRemove(vout_thread_t *vout)
{
    char *filter = var_GetNonEmptyString(vout, "video-filter");
    char *start = FilterFind(filter, "deinterlace");
    if (!start) {
        free(filter);
        return;
    }
    /* */
    strcpy(&start[0], &start[strlen("deinterlace")]);
    if (*start == ':')
        memmove(start, start + 1, strlen(start) /* + 1 - 1 */);
    var_SetString(vout, "video-filter", filter);
    free(filter);
}
