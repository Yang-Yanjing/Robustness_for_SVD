}
static void test_moduledescriptionlist (libvlc_module_description_t *list)
{
    libvlc_module_description_t *module = list;
    while ( module ) {
        assert (strlen (module->psz_name) );
        assert (strlen (module->psz_shortname) );
        assert (module->psz_longname == NULL || strlen (module->psz_longname));
        assert (module->psz_help == NULL || strlen (module->psz_help));
        module = module->p_next;
    }    
    libvlc_module_description_list_release (list);
}
