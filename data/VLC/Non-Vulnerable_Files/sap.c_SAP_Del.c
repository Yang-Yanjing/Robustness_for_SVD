 */
void SAP_Del (sap_handler_t *p_sap, const session_descriptor_t *p_session)
{
    vlc_mutex_lock (&p_sap->lock);
    /* TODO: give a handle back in SAP_Add, and use that... */
    sap_address_t *addr, **paddr;
    sap_session_t *session, **psession;
    paddr = &p_sap->first;
    for (addr = p_sap->first; addr; addr = addr->next)
    {
        psession = &addr->first;
        vlc_mutex_lock (&addr->lock);
        for (session = addr->first; session; session = session->next)
        {
            if (session->p_sd == p_session)
                goto found;
            psession = &session->next;
        }
        vlc_mutex_unlock (&addr->lock);
        paddr = &addr->next;
    }
    assert (0);
found:
    *psession = session->next;
    if (addr->first == NULL)
        /* Last session for this address -> unlink the address */
        *paddr = addr->next;
    vlc_mutex_unlock (&p_sap->lock);
    if (addr->first == NULL)
    {
        /* Last session for this address -> unlink the address */
        vlc_mutex_unlock (&addr->lock);
        AddressDestroy (addr);
    }
    else
    {
        addr->session_count--;
        vlc_cond_signal (&addr->wait);
        vlc_mutex_unlock (&addr->lock);
    }
    free (session);
}
