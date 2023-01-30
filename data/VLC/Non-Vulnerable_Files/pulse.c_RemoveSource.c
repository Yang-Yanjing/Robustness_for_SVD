 */
static void RemoveSource (services_discovery_t *sd, uint32_t idx)
{
    services_discovery_sys_t *sys = sd->p_sys;
    struct device **dp = tfind (&idx, &sys->root, cmpsrc);
    if (dp == NULL)
        return;
    struct device *d = *dp;
    tdelete (d, &sys->root, cmpsrc);
    DestroySource (d);
}
