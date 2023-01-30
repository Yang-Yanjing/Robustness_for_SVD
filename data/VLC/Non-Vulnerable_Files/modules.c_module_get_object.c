 */
const char *module_get_object( const module_t *m )
{
    if (unlikely(m->i_shortcuts == 0))
        return "unnamed";
    return m->pp_shortcuts[0];
}
