static void mxf_write_index_table_segment(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    AVIOContext *pb = s->pb;
    int i, j, temporal_reordering = 0;
    int key_index = mxf->last_key_index;
    av_log(s, AV_LOG_DEBUG, "edit units count %d\n", mxf->edit_units_count);
    if (!mxf->edit_units_count && !mxf->edit_unit_byte_count)
        return;
    avio_write(pb, index_table_segment_key, 16);
    if (mxf->edit_unit_byte_count) {
        klv_encode_ber_length(pb, 80);
    } else {
        klv_encode_ber_length(pb, 85 + 12+(s->nb_streams+1LL)*6 +
                              12+mxf->edit_units_count*(11+mxf->slice_count*4LL));
    }
    
    mxf_write_local_tag(pb, 16, 0x3C0A);
    mxf_write_uuid(pb, IndexTableSegment, 0);
    
    mxf_write_local_tag(pb, 8, 0x3F0B);
    avio_wb32(pb, mxf->time_base.den);
    avio_wb32(pb, mxf->time_base.num);
    
    mxf_write_local_tag(pb, 8, 0x3F0C);
    avio_wb64(pb, mxf->last_indexed_edit_unit);
    
    mxf_write_local_tag(pb, 8, 0x3F0D);
    if (mxf->edit_unit_byte_count)
        avio_wb64(pb, 0); 
    else
        avio_wb64(pb, mxf->edit_units_count);
    
    mxf_write_local_tag(pb, 4, 0x3F05);
    avio_wb32(pb, mxf->edit_unit_byte_count);
    
    mxf_write_local_tag(pb, 4, 0x3F06);
    avio_wb32(pb, 2);
    
    mxf_write_local_tag(pb, 4, 0x3F07);
    avio_wb32(pb, 1);
    if (!mxf->edit_unit_byte_count) {
        
        mxf_write_local_tag(pb, 1, 0x3F08);
        avio_w8(pb, mxf->slice_count);
        
        mxf_write_local_tag(pb, 8 + (s->nb_streams+1)*6, 0x3F09);
        avio_wb32(pb, s->nb_streams+1); 
        avio_wb32(pb, 6);               
        
        avio_w8(pb, 0);
        avio_w8(pb, 0); 
        avio_wb32(pb, 0); 
        for (i = 0; i < s->nb_streams; i++) {
            AVStream *st = s->streams[i];
            MXFStreamContext *sc = st->priv_data;
            avio_w8(pb, sc->temporal_reordering);
            if (sc->temporal_reordering)
                temporal_reordering = 1;
            if (i == 0) { 
                avio_w8(pb, 0); 
                avio_wb32(pb, KAG_SIZE); 
            } else { 
                unsigned audio_frame_size = sc->aic.samples[0]*sc->aic.sample_size;
                audio_frame_size += klv_fill_size(audio_frame_size);
                avio_w8(pb, 1);
                avio_wb32(pb, (i-1)*audio_frame_size); 
            }
        }
        mxf_write_local_tag(pb, 8 + mxf->edit_units_count*(11+mxf->slice_count*4), 0x3F0A);
        avio_wb32(pb, mxf->edit_units_count);  
        avio_wb32(pb, 11+mxf->slice_count*4);  
        for (i = 0; i < mxf->edit_units_count; i++) {
            int temporal_offset = 0;
            if (!(mxf->index_entries[i].flags & 0x33)) { 
                mxf->last_key_index = key_index;
                key_index = i;
            }
            if (temporal_reordering) {
                int pic_num_in_gop = i - key_index;
                if (pic_num_in_gop != mxf->index_entries[i].temporal_ref) {
                    for (j = key_index; j < mxf->edit_units_count; j++) {
                        if (pic_num_in_gop == mxf->index_entries[j].temporal_ref)
                            break;
                    }
                    if (j == mxf->edit_units_count)
                        av_log(s, AV_LOG_WARNING, "missing frames\n");
                    temporal_offset = j - key_index - pic_num_in_gop;
                }
            }
            avio_w8(pb, temporal_offset);
            if ((mxf->index_entries[i].flags & 0x30) == 0x30) { 
                avio_w8(pb, mxf->last_key_index - i);
            } else {
                avio_w8(pb, key_index - i); 
                if ((mxf->index_entries[i].flags & 0x20) == 0x20) 
                    mxf->last_key_index = key_index;
            }
            if (!(mxf->index_entries[i].flags & 0x33) && 
                mxf->index_entries[i].flags & 0x40 && !temporal_offset)
                mxf->index_entries[i].flags |= 0x80; 
            avio_w8(pb, mxf->index_entries[i].flags);
            
            avio_wb64(pb, mxf->index_entries[i].offset);
            if (s->nb_streams > 1)
                avio_wb32(pb, mxf->index_entries[i].slice_offset);
        }
        mxf->last_key_index = key_index - mxf->edit_units_count;
        mxf->last_indexed_edit_unit += mxf->edit_units_count;
        mxf->edit_units_count = 0;
    }
}
