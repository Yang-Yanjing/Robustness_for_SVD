 *****************************************************************************/
void DirClose( vlc_object_t * p_this )
{
    access_t *p_access = (access_t*)p_this;
    access_sys_t *p_sys = p_access->p_sys;
    while (p_sys->current)
    {
        directory_t *current = p_sys->current;
        p_sys->current = current->parent;
        closedir (current->handle);
        free (current->uri);
        while (current->i < current->filec)
            free (current->filev[current->i++]);
        free (current->filev);
#ifndef HAVE_OPENAT
        free (current->path);
#endif
        free (current);
    }
    free (p_sys->xspf_ext);
    free (p_sys->ignored_exts);
    free (p_sys);
}
