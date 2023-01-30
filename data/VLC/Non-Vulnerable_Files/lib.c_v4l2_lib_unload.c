__attribute__((destructor))
static void v4l2_lib_unload (void)
{
    if (v4l2_handle != NULL)
        dlclose (v4l2_handle);
}
