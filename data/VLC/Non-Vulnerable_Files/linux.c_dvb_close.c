}
void dvb_close (dvb_device_t *d)
{
#ifndef USE_DMX
    if (!d->budget)
    {
        for (size_t i = 0; i < MAX_PIDS; i++)
            if (d->pids[i].fd != -1)
                close (d->pids[i].fd);
    }
#endif
#ifdef HAVE_DVBPSI
    if (d->cam != NULL)
        en50221_End (d->cam);
#endif
    if (d->frontend != -1)
        close (d->frontend);
    close (d->demux);
    close (d->dir);
    free (d);
}
