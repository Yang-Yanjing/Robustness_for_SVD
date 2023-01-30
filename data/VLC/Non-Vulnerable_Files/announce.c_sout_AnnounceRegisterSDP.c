session_descriptor_t *
sout_AnnounceRegisterSDP( vlc_object_t *obj, const char *psz_sdp,
                          const char *psz_dst )
{
    session_descriptor_t *p_session = calloc( 1, sizeof (*p_session) );
    if( !p_session )
        return NULL;
    p_session->psz_sdp = strdup( psz_sdp );
    /* GRUIK. We should not convert back-and-forth from string to numbers */
    struct addrinfo *res;
    if (vlc_getaddrinfo (psz_dst, 0, NULL, &res) == 0)
    {
        if (res->ai_addrlen <= sizeof (p_session->addr))
            memcpy (&p_session->addr, res->ai_addr,
                    p_session->addrlen = res->ai_addrlen);
        freeaddrinfo (res);
    }
    vlc_mutex_lock (&sap_mutex);
    sap_handler_t *p_sap = libvlc_priv (obj->p_libvlc)->p_sap;
    if (p_sap == NULL)
    {
        p_sap = SAP_Create (VLC_OBJECT (obj->p_libvlc));
        libvlc_priv (obj->p_libvlc)->p_sap = p_sap;
        vlc_object_set_destructor ((vlc_object_t *)p_sap, sap_destroy);
    }
    else
        vlc_object_hold ((vlc_object_t *)p_sap);
    vlc_mutex_unlock (&sap_mutex);
    if (p_sap == NULL)
        goto error;
    msg_Dbg (obj, "adding SAP session");
    if (SAP_Add (p_sap, p_session))
    {
        vlc_mutex_lock (&sap_mutex);
        vlc_object_release ((vlc_object_t *)p_sap);
        vlc_mutex_unlock (&sap_mutex);
        goto error;
    }
    return p_session;
error:
    free (p_session->psz_sdp);
    free (p_session);
    return NULL;
}
