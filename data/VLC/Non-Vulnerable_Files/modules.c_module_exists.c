 */
bool module_exists (const char * psz_name)
{
    return module_find (psz_name) != NULL;
}
