/* Calculate current level from root-mean-squared of circular buffer ("RMS") */
static float RmsEnvProcess( rms_env * p_r, const float f_x )
{
    /* Remove the old term from the sum */
    p_r->f_sum -= p_r->pf_buf[p_r->i_pos];
    /* Add the new term to the sum */
    p_r->f_sum += f_x;
    /* If the sum is small enough, make it zero */
    if( p_r->f_sum < 1.0e-6 )
    {
        p_r->f_sum = 0.0f;
    }
    /* Replace the old term in the array with the new one */
    p_r->pf_buf[p_r->i_pos] = f_x;
    /* Go to the next position for the next RMS calculation */
    p_r->i_pos = ( p_r->i_pos + 1 ) % ( p_r->i_count );
    /* Return the RMS value */
    return sqrt( p_r->f_sum / p_r->i_count );
}
