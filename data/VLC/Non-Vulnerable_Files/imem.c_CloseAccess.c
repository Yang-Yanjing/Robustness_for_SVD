 */
static void CloseAccess(vlc_object_t *object)
{
    access_t *access = (access_t *)object;
    CloseCommon((imem_sys_t*)access->p_sys);
}
