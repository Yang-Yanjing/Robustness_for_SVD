 ****************************************************************************/
static void sap_destroy (vlc_object_t *p_this)
{
    libvlc_priv (p_this->p_libvlc)->p_sap = NULL;
}
