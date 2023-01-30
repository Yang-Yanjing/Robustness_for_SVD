 */
void ObjectKillChildrens( vlc_object_t *p_obj )
{
    /* FIXME ObjectKillChildrens seems a very bad idea in fact */
    /*if( p_obj == VLC_OBJECT(p_input->p->p_sout) ) return;*/
    vlc_object_internals_t *priv = vlc_internals (p_obj);
    if (atomic_exchange (&priv->alive, false))
    {
        int fd;
        vlc_mutex_lock (&pipe_lock);
        fd = priv->pipes[1];
        vlc_mutex_unlock (&pipe_lock);
        if (fd != -1)
        {
            write (fd, &(uint64_t){ 1 }, sizeof (uint64_t));
            msg_Dbg (p_obj, "object waitpipe triggered");
        }
    }
    vlc_list_t *p_list = vlc_list_children( p_obj );
    for( int i = 0; i < p_list->i_count; i++ )
        ObjectKillChildrens( p_list->p_values[i].p_object );
    vlc_list_release( p_list );
}
