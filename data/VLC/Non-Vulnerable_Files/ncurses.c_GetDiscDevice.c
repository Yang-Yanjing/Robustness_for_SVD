}
static char *GetDiscDevice(intf_thread_t *intf, const char *name)
{
    static const struct { const char *s; size_t n; const char *v; } devs[] =
    {
        { "cdda://", 7, "cd-audio", },
        { "dvd://",  6, "dvd",      },
        { "vcd://",  6, "vcd",      },
    };
    char *device;
    for (unsigned i = 0; i < sizeof devs / sizeof *devs; i++) {
        size_t n = devs[i].n;
        if (!strncmp(name, devs[i].s, n)) {
            if (name[n] == '@' || name[n] == '\0')
                return config_GetPsz(intf, devs[i].v);
            /* Omit the beginning MRL-selector characters */
            return strdup(name + n);
        }
    }
    device = strdup(name);
    if (device) /* Remove what we have after @ */
        device[strcspn(device, "@")] = '\0';
    return device;
}
