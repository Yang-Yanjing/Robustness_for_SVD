static vdp_instance_t *list = NULL;
static vdp_instance_t *vdp_instance_lookup(const char *name, int num)
{
    vdp_instance_t *vi = NULL;
    for (vi = list; vi != NULL; vi = vi->next)
    {
        int val = vicmp(name, num, vi);
        if (val == 0)
        {
            assert(vi->refs < UINTPTR_MAX);
            vi->refs++;
            break;
        }
    }
    return vi;
}
