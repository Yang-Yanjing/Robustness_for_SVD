 *****************************************************************************/
void decoder_SynchroEnd( decoder_synchro_t * p_synchro, int i_coding_type,
                      bool b_garbage )
{
    mtime_t     tau;
    if( b_garbage )
        return;
    tau = mdate() - p_synchro->decoding_start;
    /* If duration too high, something happened (pause ?), so don't
     * take it into account. */
    if( tau < 3 * p_synchro->p_tau[i_coding_type] ||
          ( !p_synchro->pi_meaningful[i_coding_type] && tau < MAX_VALID_TAU ) )
    {
        /* Mean with average tau, to ensure stability. */
        p_synchro->p_tau[i_coding_type] =
            (p_synchro->pi_meaningful[i_coding_type]
             * p_synchro->p_tau[i_coding_type] + tau)
            / (p_synchro->pi_meaningful[i_coding_type] + 1);
        if( p_synchro->pi_meaningful[i_coding_type] < MAX_PIC_AVERAGE )
        {
            p_synchro->pi_meaningful[i_coding_type]++;
        }
    }
}
