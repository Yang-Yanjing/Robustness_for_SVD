static void (*pf_host_deinit)(void);
static void CloseExtraDll()
{
    if (pf_host_deinit)
        pf_host_deinit();
    // Intentionally not unloading the host library, since it cannot be
    // unloaded cleanly after it has been initialized.
}
