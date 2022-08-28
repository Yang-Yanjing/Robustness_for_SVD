static int config_output(AVFilterLink *outlink)
{
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    return 0;
}
