 */
static void BufferProcess( float * pf_buf, int i_channels, float f_gain,
                           float f_mug, lookahead * p_la )
{
    /* Loop through every channel */
    for( int i_chan = 0; i_chan < i_channels; i_chan++ )
    {
        float f_x = pf_buf[i_chan]; /* Current buffer value */
        /* Output the compressed delayed buffer value */
        pf_buf[i_chan] = p_la->p_buf[p_la->i_pos].pf_vals[i_chan]
                       * f_gain * f_mug;
        /* Update the delayed buffer value */
        p_la->p_buf[p_la->i_pos].pf_vals[i_chan] = f_x;
    }
    /* Go to the next delayed buffer value for the next run */
    p_la->i_pos = ( p_la->i_pos + 1 ) % ( p_la->i_count );
}
