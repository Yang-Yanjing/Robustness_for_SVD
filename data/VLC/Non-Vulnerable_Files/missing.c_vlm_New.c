#undef vlm_New
vlm_t *vlm_New (vlc_object_t *obj)
{
     msg_Err (obj, "VLM not compiled-in!");
     return NULL;
}
