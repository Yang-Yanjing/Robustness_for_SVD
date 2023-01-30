 *****************************************************************************/
mtime_t decoder_SynchroDate( decoder_synchro_t * p_synchro )
{
    /* No need to lock, since PTS are only used by the video parser. */
    return p_synchro->current_pts;
}
