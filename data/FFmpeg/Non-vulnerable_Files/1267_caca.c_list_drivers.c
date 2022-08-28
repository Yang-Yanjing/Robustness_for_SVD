static void list_drivers(CACAContext *c)
{
    const char *const *drivers = caca_get_display_driver_list();
    int i;
    av_log(c->ctx, AV_LOG_INFO, "Available drivers:\n");
    for (i = 0; drivers[i]; i += 2)
        av_log(c->ctx, AV_LOG_INFO, "%s : %s\n", drivers[i], drivers[i + 1]);
}
