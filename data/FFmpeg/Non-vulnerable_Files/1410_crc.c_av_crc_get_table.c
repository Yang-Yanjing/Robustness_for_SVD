const AVCRC *av_crc_get_table(AVCRCId crc_id)
{
#if !CONFIG_HARDCODED_TABLES
    if (!av_crc_table[crc_id][FF_ARRAY_ELEMS(av_crc_table[crc_id]) - 1])
        if (av_crc_init(av_crc_table[crc_id],
                        av_crc_table_params[crc_id].le,
                        av_crc_table_params[crc_id].bits,
                        av_crc_table_params[crc_id].poly,
                        sizeof(av_crc_table[crc_id])) < 0)
            return NULL;
#endif
    return av_crc_table[crc_id];
}
