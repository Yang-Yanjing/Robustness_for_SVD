}
static void SubpictureDestroy( subpicture_t *p_subpic )
{
    subpicture_updater_sys_t *p_sys = p_subpic->updater.p_sys;
    DecSysRelease( p_sys->p_dec_sys );
    free( p_sys->p_subs_data );
    free( p_sys );
}
