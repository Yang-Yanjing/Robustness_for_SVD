 */
const char *module_get_name( const module_t *m, bool long_name )
{
    if( long_name && ( m->psz_longname != NULL) )
        return m->psz_longname;
    if (m->psz_shortname != NULL)
        return m->psz_shortname;
    return module_get_object (m);
}
