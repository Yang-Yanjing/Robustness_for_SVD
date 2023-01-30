 *****************************************************************************/
void CacheMerge( vlc_object_t *p_this, module_t *p_cache, module_t *p_module )
{
    (void)p_this;
    p_cache->pf_activate = p_module->pf_activate;
    p_cache->pf_deactivate = p_module->pf_deactivate;
    p_cache->handle = p_module->handle;
    /* FIXME: This looks too simplistic an algorithm to me. What if the module
     * file was altered such that the number of order of submodules was
     * altered... after VLC started -- Courmisch, 09/2008 */
    module_t *p_child = p_module->submodule,
             *p_cchild = p_cache->submodule;
    while( p_child && p_cchild )
    {
        p_cchild->pf_activate = p_child->pf_activate;
        p_cchild->pf_deactivate = p_child->pf_deactivate;
        p_child = p_child->next;
        p_cchild = p_cchild->next;
    }
    p_cache->b_loaded = true;
    p_module->b_loaded = false;
}
