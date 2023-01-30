static int mxf_parse_structural_metadata(MXFContext *mxf)
{
    MXFPackage *material_package = NULL;
    int i, j, k, ret;
    av_log(mxf->fc, AV_LOG_TRACE, "metadata sets count %d\n", mxf->metadata_sets_count);
    
    for (i = 0; i < mxf->packages_count; i++) {
        material_package = mxf_resolve_strong_ref(mxf, &mxf->packages_refs[i], MaterialPackage);
        if (material_package) break;
    }
    if (!material_package) {
        av_log(mxf->fc, AV_LOG_ERROR, "no material package found\n");
        return AVERROR_INVALIDDATA;
    }
    mxf_add_umid_metadata(&mxf->fc->metadata, "material_package_umid", material_package);
    if (material_package->name && material_package->name[0])
        av_dict_set(&mxf->fc->metadata, "material_package_name", material_package->name, 0);
    mxf_parse_package_comments(mxf, &mxf->fc->metadata, material_package);
    for (i = 0; i < material_package->tracks_count; i++) {
        MXFPackage *source_package = NULL;
        MXFTrack *material_track = NULL;
        MXFTrack *source_track = NULL;
        MXFTrack *temp_track = NULL;
        MXFDescriptor *descriptor = NULL;
        MXFStructuralComponent *component = NULL;
        MXFTimecodeComponent *mxf_tc = NULL;
        UID *essence_container_ul = NULL;
        const MXFCodecUL *codec_ul = NULL;
        const MXFCodecUL *container_ul = NULL;
        const MXFCodecUL *pix_fmt_ul = NULL;
        AVStream *st;
        AVTimecode tc;
        int flags;
        if (!(material_track = mxf_resolve_strong_ref(mxf, &material_package->tracks_refs[i], Track))) {
            av_log(mxf->fc, AV_LOG_ERROR, "could not resolve material track strong ref\n");
            continue;
        }
        if ((component = mxf_resolve_strong_ref(mxf, &material_track->sequence_ref, TimecodeComponent))) {
            mxf_tc = (MXFTimecodeComponent*)component;
            flags = mxf_tc->drop_frame == 1 ? AV_TIMECODE_FLAG_DROPFRAME : 0;
            if (av_timecode_init(&tc, mxf_tc->rate, flags, mxf_tc->start_frame, mxf->fc) == 0) {
                mxf_add_timecode_metadata(&mxf->fc->metadata, "timecode", &tc);
            }
        }
        if (!(material_track->sequence = mxf_resolve_strong_ref(mxf, &material_track->sequence_ref, Sequence))) {
            av_log(mxf->fc, AV_LOG_ERROR, "could not resolve material track sequence strong ref\n");
            continue;
        }
        for (j = 0; j < material_track->sequence->structural_components_count; j++) {
            component = mxf_resolve_strong_ref(mxf, &material_track->sequence->structural_components_refs[j], TimecodeComponent);
            if (!component)
                continue;
            mxf_tc = (MXFTimecodeComponent*)component;
            flags = mxf_tc->drop_frame == 1 ? AV_TIMECODE_FLAG_DROPFRAME : 0;
            if (av_timecode_init(&tc, mxf_tc->rate, flags, mxf_tc->start_frame, mxf->fc) == 0) {
                mxf_add_timecode_metadata(&mxf->fc->metadata, "timecode", &tc);
                break;
            }
        }
        
        for (j = 0; j < material_track->sequence->structural_components_count; j++) {
            component = mxf_resolve_sourceclip(mxf, &material_track->sequence->structural_components_refs[j]);
            if (!component)
                continue;
            source_package = mxf_resolve_source_package(mxf, component->source_package_uid);
            if (!source_package) {
                av_log(mxf->fc, AV_LOG_TRACE, "material track %d: no corresponding source package found\n", material_track->track_id);
                break;
            }
            for (k = 0; k < source_package->tracks_count; k++) {
                if (!(temp_track = mxf_resolve_strong_ref(mxf, &source_package->tracks_refs[k], Track))) {
                    av_log(mxf->fc, AV_LOG_ERROR, "could not resolve source track strong ref\n");
                    ret = AVERROR_INVALIDDATA;
                    goto fail_and_free;
                }
                if (temp_track->track_id == component->source_track_id) {
                    source_track = temp_track;
                    break;
                }
            }
            if (!source_track) {
                av_log(mxf->fc, AV_LOG_ERROR, "material track %d: no corresponding source track found\n", material_track->track_id);
                break;
            }
        }
        if (!source_track || !component)
            continue;
        if (!(source_track->sequence = mxf_resolve_strong_ref(mxf, &source_track->sequence_ref, Sequence))) {
            av_log(mxf->fc, AV_LOG_ERROR, "could not resolve source track sequence strong ref\n");
            ret = AVERROR_INVALIDDATA;
            goto fail_and_free;
        }
        

        if (memcmp(material_track->sequence->data_definition_ul, source_track->sequence->data_definition_ul, 16)) {
            av_log(mxf->fc, AV_LOG_ERROR, "material track %d: DataDefinition mismatch\n", material_track->track_id);
            continue;
        }
        st = avformat_new_stream(mxf->fc, NULL);
        if (!st) {
            av_log(mxf->fc, AV_LOG_ERROR, "could not allocate stream\n");
            ret = AVERROR(ENOMEM);
            goto fail_and_free;
        }
        st->id = source_track->track_id;
        st->priv_data = source_track;
        source_package->descriptor = mxf_resolve_strong_ref(mxf, &source_package->descriptor_ref, AnyType);
        descriptor = mxf_resolve_multidescriptor(mxf, source_package->descriptor, source_track->track_id);
        

        if (descriptor && descriptor->duration != AV_NOPTS_VALUE)
            source_track->original_duration = st->duration = FFMIN(descriptor->duration, component->duration);
        else
            source_track->original_duration = st->duration = component->duration;
        if (st->duration == -1)
            st->duration = AV_NOPTS_VALUE;
        st->start_time = component->start_position;
        if (material_track->edit_rate.num <= 0 ||
            material_track->edit_rate.den <= 0) {
            av_log(mxf->fc, AV_LOG_WARNING,
                   "Invalid edit rate (%d/%d) found on stream #%d, "
                   "defaulting to 25/1\n",
                   material_track->edit_rate.num,
                   material_track->edit_rate.den, st->index);
            material_track->edit_rate = (AVRational){25, 1};
        }
        avpriv_set_pts_info(st, 64, material_track->edit_rate.den, material_track->edit_rate.num);
        

        source_track->edit_rate = material_track->edit_rate;
        PRINT_KEY(mxf->fc, "data definition   ul", source_track->sequence->data_definition_ul);
        codec_ul = mxf_get_codec_ul(ff_mxf_data_definition_uls, &source_track->sequence->data_definition_ul);
        st->codec->codec_type = codec_ul->id;
        if (!descriptor) {
            av_log(mxf->fc, AV_LOG_INFO, "source track %d: stream %d, no descriptor found\n", source_track->track_id, st->index);
            continue;
        }
        PRINT_KEY(mxf->fc, "essence codec     ul", descriptor->essence_codec_ul);
        PRINT_KEY(mxf->fc, "essence container ul", descriptor->essence_container_ul);
        essence_container_ul = &descriptor->essence_container_ul;
        

        if (IS_KLV_KEY(essence_container_ul, mxf_encrypted_essence_container)) {
            av_log(mxf->fc, AV_LOG_INFO, "broken encrypted mxf file\n");
            for (k = 0; k < mxf->metadata_sets_count; k++) {
                MXFMetadataSet *metadata = mxf->metadata_sets[k];
                if (metadata->type == CryptoContext) {
                    essence_container_ul = &((MXFCryptoContext *)metadata)->source_container_ul;
                    break;
                }
            }
        }
        
        codec_ul = mxf_get_codec_ul(ff_mxf_codec_uls, &descriptor->essence_codec_ul);
        st->codec->codec_id = (enum AVCodecID)codec_ul->id;
        if (st->codec->codec_id == AV_CODEC_ID_NONE) {
            codec_ul = mxf_get_codec_ul(ff_mxf_codec_uls, &descriptor->codec_ul);
            st->codec->codec_id = (enum AVCodecID)codec_ul->id;
        }
        av_log(mxf->fc, AV_LOG_VERBOSE, "%s: Universal Label: ",
               avcodec_get_name(st->codec->codec_id));
        for (k = 0; k < 16; k++) {
            av_log(mxf->fc, AV_LOG_VERBOSE, "%.2x",
                   descriptor->essence_codec_ul[k]);
            if (!(k+1 & 19) || k == 5)
                av_log(mxf->fc, AV_LOG_VERBOSE, ".");
        }
        av_log(mxf->fc, AV_LOG_VERBOSE, "\n");
        mxf_add_umid_metadata(&st->metadata, "file_package_umid", source_package);
        if (source_package->name && source_package->name[0])
            av_dict_set(&st->metadata, "file_package_name", source_package->name, 0);
        mxf_parse_physical_source_package(mxf, source_track, st);
        if (st->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            source_track->intra_only = mxf_is_intra_only(descriptor);
            container_ul = mxf_get_codec_ul(mxf_picture_essence_container_uls, essence_container_ul);
            if (st->codec->codec_id == AV_CODEC_ID_NONE)
                st->codec->codec_id = container_ul->id;
            st->codec->width = descriptor->width;
            st->codec->height = descriptor->height; 
            switch (descriptor->frame_layout) {
                case FullFrame:
                    st->codec->field_order = AV_FIELD_PROGRESSIVE;
                    break;
                case OneField:
                    
                    av_log(mxf->fc, AV_LOG_INFO, "OneField frame layout isn't currently supported\n");
                    break; 


                case MixedFields:
                    break;
                case SegmentedFrame:
                    st->codec->field_order = AV_FIELD_PROGRESSIVE;
                case SeparateFields:
                    switch (descriptor->field_dominance) {
                    case MXF_TFF:
                        st->codec->field_order = AV_FIELD_TT;
                        break;
                    case MXF_BFF:
                        st->codec->field_order = AV_FIELD_BB;
                        break;
                    default:
                        avpriv_request_sample(mxf->fc,
                                              "Field dominance %d support",
                                              descriptor->field_dominance);
                    case 0: 
                        break;
                    }
                    
                    st->codec->height *= 2;
                    break;
                default:
                    av_log(mxf->fc, AV_LOG_INFO, "Unknown frame layout type: %d\n", descriptor->frame_layout);
            }
            if (st->codec->codec_id == AV_CODEC_ID_RAWVIDEO) {
                st->codec->pix_fmt = descriptor->pix_fmt;
                if (st->codec->pix_fmt == AV_PIX_FMT_NONE) {
                    pix_fmt_ul = mxf_get_codec_ul(ff_mxf_pixel_format_uls,
                                                  &descriptor->essence_codec_ul);
                    st->codec->pix_fmt = (enum AVPixelFormat)pix_fmt_ul->id;
                    if (st->codec->pix_fmt == AV_PIX_FMT_NONE) {
                        st->codec->codec_tag = mxf_get_codec_ul(ff_mxf_codec_tag_uls,
                                                                &descriptor->essence_codec_ul)->id;
                        if (!st->codec->codec_tag) {
                            

                            if (descriptor->horiz_subsampling == 2 &&
                                descriptor->vert_subsampling == 1 &&
                                descriptor->component_depth == 8) {
                                st->codec->pix_fmt = AV_PIX_FMT_UYVY422;
                            }
                        }
                    }
                }
            }
            st->need_parsing = AVSTREAM_PARSE_HEADERS;
            if (material_track->sequence->origin) {
                av_dict_set_int(&st->metadata, "material_track_origin", material_track->sequence->origin, 0);
            }
            if (source_track->sequence->origin) {
                av_dict_set_int(&st->metadata, "source_track_origin", source_track->sequence->origin, 0);
            }
            if (descriptor->aspect_ratio.num && descriptor->aspect_ratio.den)
                st->display_aspect_ratio = descriptor->aspect_ratio;
        } else if (st->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            container_ul = mxf_get_codec_ul(mxf_sound_essence_container_uls, essence_container_ul);
            
            if (st->codec->codec_id == AV_CODEC_ID_NONE || (st->codec->codec_id == AV_CODEC_ID_PCM_ALAW && (enum AVCodecID)container_ul->id != AV_CODEC_ID_NONE))
                st->codec->codec_id = (enum AVCodecID)container_ul->id;
            st->codec->channels = descriptor->channels;
            st->codec->bits_per_coded_sample = descriptor->bits_per_sample;
            if (descriptor->sample_rate.den > 0) {
                st->codec->sample_rate = descriptor->sample_rate.num / descriptor->sample_rate.den;
                avpriv_set_pts_info(st, 64, descriptor->sample_rate.den, descriptor->sample_rate.num);
            } else {
                av_log(mxf->fc, AV_LOG_WARNING, "invalid sample rate (%d/%d) "
                       "found for stream #%d, time base forced to 1/48000\n",
                       descriptor->sample_rate.num, descriptor->sample_rate.den,
                       st->index);
                avpriv_set_pts_info(st, 64, 1, 48000);
            }
            
            if (st->duration != AV_NOPTS_VALUE)
                st->duration = av_rescale_q(st->duration,
                                            av_inv_q(material_track->edit_rate),
                                            st->time_base);
            
            if (st->codec->codec_id == AV_CODEC_ID_PCM_S16LE) {
                if (descriptor->bits_per_sample > 16 && descriptor->bits_per_sample <= 24)
                    st->codec->codec_id = AV_CODEC_ID_PCM_S24LE;
                else if (descriptor->bits_per_sample == 32)
                    st->codec->codec_id = AV_CODEC_ID_PCM_S32LE;
            } else if (st->codec->codec_id == AV_CODEC_ID_PCM_S16BE) {
                if (descriptor->bits_per_sample > 16 && descriptor->bits_per_sample <= 24)
                    st->codec->codec_id = AV_CODEC_ID_PCM_S24BE;
                else if (descriptor->bits_per_sample == 32)
                    st->codec->codec_id = AV_CODEC_ID_PCM_S32BE;
            } else if (st->codec->codec_id == AV_CODEC_ID_MP2) {
                st->need_parsing = AVSTREAM_PARSE_FULL;
            }
        } else if (st->codec->codec_type == AVMEDIA_TYPE_DATA) {
            int codec_id = mxf_get_codec_ul(mxf_data_essence_container_uls,
                                            essence_container_ul)->id;
            if (codec_id >= 0 &&
                codec_id < FF_ARRAY_ELEMS(mxf_data_essence_descriptor)) {
                av_dict_set(&st->metadata, "data_type",
                            mxf_data_essence_descriptor[codec_id], 0);
            }
        }
        if (descriptor->extradata) {
            if (!ff_alloc_extradata(st->codec, descriptor->extradata_size)) {
                memcpy(st->codec->extradata, descriptor->extradata, descriptor->extradata_size);
            }
        } else if (st->codec->codec_id == AV_CODEC_ID_H264) {
            ret = ff_generate_avci_extradata(st);
            if (ret < 0)
                return ret;
        }
        if (st->codec->codec_type != AVMEDIA_TYPE_DATA && (*essence_container_ul)[15] > 0x01) {
            
            st->need_parsing = AVSTREAM_PARSE_TIMESTAMPS;
        }
    }
    ret = 0;
fail_and_free:
    return ret;
}
