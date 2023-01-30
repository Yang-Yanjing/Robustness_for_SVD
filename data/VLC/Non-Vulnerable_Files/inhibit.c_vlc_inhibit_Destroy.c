}
void vlc_inhibit_Destroy (vlc_inhibit_t *ih)
{
    assert (ih != NULL);
    module_unneed (ih, ((inhibit_t *)ih)->module);
    vlc_object_release (ih);
}
