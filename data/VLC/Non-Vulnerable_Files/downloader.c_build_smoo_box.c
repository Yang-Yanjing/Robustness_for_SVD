 * simplicity of the design is better than compactness */
static int build_smoo_box( stream_t *s, uint8_t *smoo_box )
{
    stream_sys_t *p_sys = s->p_sys;
    sms_stream_t *sms = NULL;
    uint32_t FourCC;
    /* smoo */
    memset( smoo_box, 0, SMOO_SIZE );
    smoo_box[2] = (SMOO_SIZE & 0xff00)>>8;
    smoo_box[3] = SMOO_SIZE & 0xff;
    smoo_box[4] = 'u';
    smoo_box[5] = 'u';
    smoo_box[6] = 'i';
    smoo_box[7] = 'd';
    /* UUID is e1da72ba-24d7-43c3-a6a5-1b5759a1a92c */
    ((uint32_t *)smoo_box)[2] = bswap32( 0xe1da72ba );
    ((uint32_t *)smoo_box)[3] = bswap32( 0x24d743c3 );
    ((uint32_t *)smoo_box)[4] = bswap32( 0xa6a51b57 );
    ((uint32_t *)smoo_box)[5] = bswap32( 0x59a1a92c );
    uint8_t *stra_box;
    for( int i = 0; i < 3; i++ )
    {
        sms = NULL;
        int cat = UNKNOWN_ES;
        stra_box = smoo_box + i * STRA_SIZE;
        stra_box[26] = (STRA_SIZE & 0xff00)>>8;
        stra_box[27] = STRA_SIZE & 0xff;
        stra_box[28] = 'u';
        stra_box[29] = 'u';
        stra_box[30] = 'i';
        stra_box[31] = 'd';
        /* UUID is b03ef770-33bd-4bac-96c7-bf25f97e2447 */
        ((uint32_t *)stra_box)[8] = bswap32( 0xb03ef770 );
        ((uint32_t *)stra_box)[9] = bswap32( 0x33bd4bac );
        ((uint32_t *)stra_box)[10] = bswap32( 0x96c7bf25 );
        ((uint32_t *)stra_box)[11] = bswap32( 0xf97e2447 );
        cat = index_to_es_cat( i );
        stra_box[48] = cat;
        sms = SMS_GET_SELECTED_ST( cat );
        stra_box[49] = 0; /* reserved */
        if( sms == NULL )
            continue;
        stra_box[50] = (sms->id & 0xff00)>>8;
        stra_box[51] = sms->id & 0xff;
        ((uint32_t *)stra_box)[13] = bswap32( sms->timescale );
        ((uint64_t *)stra_box)[7] = bswap64( p_sys->vod_duration );
        quality_level_t * qlvl = get_qlevel( sms, sms->download_qlvl );
        if ( qlvl )
        {
            FourCC = qlvl->FourCC ? qlvl->FourCC : sms->default_FourCC;
            ((uint32_t *)stra_box)[16] = bswap32( FourCC );
            ((uint32_t *)stra_box)[17] = bswap32( qlvl->Bitrate );
            ((uint32_t *)stra_box)[18] = bswap32( qlvl->MaxWidth );
            ((uint32_t *)stra_box)[19] = bswap32( qlvl->MaxHeight );
            ((uint32_t *)stra_box)[20] = bswap32( qlvl->SamplingRate );
            ((uint32_t *)stra_box)[21] = bswap32( qlvl->Channels );
            ((uint32_t *)stra_box)[22] = bswap32( qlvl->BitsPerSample );
            ((uint32_t *)stra_box)[23] = bswap32( qlvl->AudioTag );
            ((uint16_t *)stra_box)[48] = bswap16( qlvl->nBlockAlign );
            if( !qlvl->CodecPrivateData )
                continue;
            stra_box[98] = stra_box[99] = stra_box[100] = 0; /* reserved */
            stra_box[101] = strlen( qlvl->CodecPrivateData ) / 2;
            if ( stra_box[101] > STRA_SIZE - 102 )
                stra_box[101] = STRA_SIZE - 102;
            uint8_t *binary_cpd = decode_string_hex_to_binary( qlvl->CodecPrivateData );
            memcpy( stra_box + 102, binary_cpd, stra_box[101] );
            free( binary_cpd );
        }
    }
    return VLC_SUCCESS;
}
