}
static int vicmp(const char *name, int num, const vdp_instance_t *vi)
{
    int val = strnullcmp(name, vi->name);
    if (val)
        return val;
    if (num < 0)
        num = XDefaultScreen(vi->display);
    return num - vi->num;
}
