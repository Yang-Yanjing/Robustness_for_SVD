}
static void ParseSei( decoder_t *p_dec, block_t *p_frag )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint8_t *pb_dec;
    int i_dec;
    /* */
    CreateDecodedNAL( &pb_dec, &i_dec, &p_frag->p_buffer[5], p_frag->i_buffer - 5 );
    if( !pb_dec )
        return;
    /* The +1 is for rbsp trailing bits */
    for( int i_used = 0; i_used+1 < i_dec; )
    {
        /* Read type */
        int i_type = 0;
        while( i_used+1 < i_dec )
        {
            const int i_byte = pb_dec[i_used++];
            i_type += i_byte;
            if( i_byte != 0xff )
                break;
        }
        /* Read size */
        int i_size = 0;
        while( i_used+1 < i_dec )
        {
            const int i_byte = pb_dec[i_used++];
            i_size += i_byte;
            if( i_byte != 0xff )
                break;
        }
        /* Check room */
        if( i_used + i_size + 1 > i_dec )
            break;
        /* Look for user_data_registered_itu_t_t35 */
        if( i_type == 4 )
        {
            static const uint8_t p_dvb1_data_start_code[] = {
                0xb5,
                0x00, 0x31,
                0x47, 0x41, 0x39, 0x34
            };
            const int      i_t35 = i_size;
            const uint8_t *p_t35 = &pb_dec[i_used];
            /* Check for we have DVB1_data() */
            if( i_t35 >= 5 &&
                !memcmp( p_t35, p_dvb1_data_start_code, sizeof(p_dvb1_data_start_code) ) )
            {
                cc_Extract( &p_sys->cc_next, true, &p_t35[3], i_t35 - 3 );
            }
        }
        /* Look for SEI recovery point */
        if( i_type == 6 )
        {
            bs_t s;
            const int      i_rec = i_size;
            const uint8_t *p_rec = &pb_dec[i_used];
            bs_init( &s, p_rec, i_rec );
            int i_recovery_frames = bs_read_ue( &s );
            //bool b_exact_match = bs_read( &s, 1 );
            //bool b_broken_link = bs_read( &s, 1 );
            //int i_changing_slice_group = bs_read( &s, 2 );
            if( !p_sys->b_header )
            {
                msg_Dbg( p_dec, "Seen SEI recovery point, %d recovery frames", i_recovery_frames );
                if ( p_sys->i_recovery_frames == -1 || i_recovery_frames < p_sys->i_recovery_frames )
                    p_sys->i_recovery_frames = i_recovery_frames;
            }
        }
        i_used += i_size;
    }
    free( pb_dec );
}
