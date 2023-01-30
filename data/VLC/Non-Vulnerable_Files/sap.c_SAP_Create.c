 */
sap_handler_t *SAP_Create (vlc_object_t *p_announce)
{
    sap_handler_t *p_sap;
    p_sap = vlc_custom_create (p_announce, sizeof (*p_sap), "sap sender");
    if (p_sap == NULL)
        return NULL;
    vlc_mutex_init (&p_sap->lock);
    p_sap->first = NULL;
    return p_sap;
}
