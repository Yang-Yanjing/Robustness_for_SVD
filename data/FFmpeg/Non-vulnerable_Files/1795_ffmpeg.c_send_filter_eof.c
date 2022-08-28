static int send_filter_eof(InputStream *ist)
{
    int i, ret;
    for (i = 0; i < ist->nb_filters; i++) {
        ret = ifilter_send_eof(ist->filters[i]);
        if (ret < 0)
            return ret;
    }
    return 0;
}
