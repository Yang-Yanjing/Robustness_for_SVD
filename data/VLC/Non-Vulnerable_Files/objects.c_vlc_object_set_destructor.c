 *****************************************************************************/
void vlc_object_set_destructor( vlc_object_t *p_this,
                                vlc_destructor_t pf_destructor )
{
    vlc_object_internals_t *p_priv = vlc_internals(p_this );
    p_priv->pf_destructor = pf_destructor;
}
