 */
static void CloseCommon(imem_sys_t *sys)
{
    free(sys->source.cookie);
    free(sys);
}
