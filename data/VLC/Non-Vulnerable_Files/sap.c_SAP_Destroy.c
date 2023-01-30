}
void SAP_Destroy (sap_handler_t *p_sap)
{
    assert (p_sap->first == NULL);
    vlc_mutex_destroy (&p_sap->lock);
    vlc_object_release (p_sap);
}
