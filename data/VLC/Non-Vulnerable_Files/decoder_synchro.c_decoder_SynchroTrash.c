 *****************************************************************************/
void decoder_SynchroTrash( decoder_synchro_t * p_synchro )
{
    p_synchro->i_trashed_pic++;
    p_synchro->i_nb_ref = p_synchro->i_trash_nb_ref;
}
