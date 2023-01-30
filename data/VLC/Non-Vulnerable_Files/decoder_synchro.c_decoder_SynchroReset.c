 *****************************************************************************/
void decoder_SynchroReset( decoder_synchro_t * p_synchro )
{
    p_synchro->i_nb_ref = 0;
    p_synchro->i_trash_nb_ref = p_synchro->i_dec_nb_ref = 0;
}
