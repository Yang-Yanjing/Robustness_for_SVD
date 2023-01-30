}
void vdp_release_x11(vdp_t *vdp)
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
    assert(vi->refs > 0);
    vi->refs--;
    if (vi->refs > 0)
        vi = NULL; /* Keep the instance for now */
    else
        *pp = vi->next; /* Unlink the instance */
    pthread_mutex_unlock(&lock);
    if (vi != NULL)
        vdp_instance_destroy(vi);
}
