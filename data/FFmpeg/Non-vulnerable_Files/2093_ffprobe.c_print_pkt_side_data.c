static void print_pkt_side_data(WriterContext *w,
                                AVCodecParameters *par,
                                const AVPacketSideData *side_data,
                                int nb_side_data,
                                SectionID id_data_list,
                                SectionID id_data)
{
    int i;
    writer_print_section_header(w, id_data_list);
    for (i = 0; i < nb_side_data; i++) {
        const AVPacketSideData *sd = &side_data[i];
        const char *name = av_packet_side_data_name(sd->type);
        writer_print_section_header(w, id_data);
        print_str("side_data_type", name ? name : "unknown");
        if (sd->type == AV_PKT_DATA_DISPLAYMATRIX && sd->size >= 9*4) {
            writer_print_integers(w, "displaymatrix", sd->data, 9, " %11d", 3, 4, 1);
            print_int("rotation", av_display_rotation_get((int32_t *)sd->data));
        } else if (sd->type == AV_PKT_DATA_STEREO3D) {
            const AVStereo3D *stereo = (AVStereo3D *)sd->data;
            print_str("type", av_stereo3d_type_name(stereo->type));
            print_int("inverted", !!(stereo->flags & AV_STEREO3D_FLAG_INVERT));
        } else if (sd->type == AV_PKT_DATA_SPHERICAL) {
            const AVSphericalMapping *spherical = (AVSphericalMapping *)sd->data;
            print_str("projection", av_spherical_projection_name(spherical->projection));
            if (spherical->projection == AV_SPHERICAL_CUBEMAP) {
                print_int("padding", spherical->padding);
            } else if (spherical->projection == AV_SPHERICAL_EQUIRECTANGULAR_TILE) {
                size_t l, t, r, b;
                av_spherical_tile_bounds(spherical, par->width, par->height,
                                         &l, &t, &r, &b);
                print_int("bound_left", l);
                print_int("bound_top", t);
                print_int("bound_right", r);
                print_int("bound_bottom", b);
            }
            print_int("yaw", (double) spherical->yaw / (1 << 16));
            print_int("pitch", (double) spherical->pitch / (1 << 16));
            print_int("roll", (double) spherical->roll / (1 << 16));
        } else if (sd->type == AV_PKT_DATA_SKIP_SAMPLES && sd->size == 10) {
            print_int("skip_samples",    AV_RL32(sd->data));
            print_int("discard_padding", AV_RL32(sd->data + 4));
            print_int("skip_reason",     AV_RL8(sd->data + 8));
            print_int("discard_reason",  AV_RL8(sd->data + 9));
        }
        writer_print_section_footer(w);
    }
    writer_print_section_footer(w);
}
