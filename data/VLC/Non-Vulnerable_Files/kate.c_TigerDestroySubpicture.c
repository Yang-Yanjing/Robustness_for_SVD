#ifdef HAVE_TIGER
static void TigerDestroySubpicture( subpicture_t *p_subpic )
{
    DecSysRelease( p_subpic->updater.p_sys->p_dec_sys );
    free( p_subpic->updater.p_sys );
}
