#undef module_start
int module_start (vlc_object_t *obj, const module_t *m)
{
   int (*activate) (vlc_object_t *) = m->pf_activate;
   return (activate != NULL) ? activate (obj) : VLC_SUCCESS;
}
