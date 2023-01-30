}
static int AVCResetHandler( raw1394handle_t handle, unsigned int generation )
{
    raw1394_update_generation( handle, generation );
    return 0;
}
