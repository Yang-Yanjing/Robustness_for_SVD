}
vdp_t *vdp_hold_x11(vdp_t *vdp, VdpDevice *restrict devp)
{
    vdp_instance_t *vi, **pp = &list;
    pthread_mutex_lock(&lock);
    for (;;)
    {
        vi = *pp;
        assert(vi != NULL);
        if (vi->vdp == vdp)
            break;
        pp = &vi->next;
    }
    assert(vi->refs < UINTPTR_MAX);
    vi->refs++;
    pthread_mutex_unlock(&lock);
    if (devp != NULL)
        *devp = vi->device;
   return vdp;
}
