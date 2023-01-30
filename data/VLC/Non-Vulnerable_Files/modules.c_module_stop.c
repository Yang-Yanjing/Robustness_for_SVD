#undef module_stop
void module_stop (vlc_object_t *obj, const module_t *m)
{
   void (*deactivate) (vlc_object_t *) = m->pf_deactivate;
    if (deactivate != NULL)
        deactivate (obj);
}
