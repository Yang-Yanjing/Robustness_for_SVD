 *****************************************************************************/
void decoder_SynchroDecode( decoder_synchro_t * p_synchro )
{
    p_synchro->decoding_start = mdate();
    p_synchro->i_nb_ref = p_synchro->i_dec_nb_ref;
}
