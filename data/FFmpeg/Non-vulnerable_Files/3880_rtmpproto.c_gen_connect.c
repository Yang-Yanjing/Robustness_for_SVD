static int gen_connect(URLContext *s, RTMPContext *rt)
{
    RTMPPacket pkt;
    uint8_t *p;
    int ret;
    if ((ret = ff_rtmp_packet_create(&pkt, RTMP_SYSTEM_CHANNEL, RTMP_PT_INVOKE,
                                     0, 4096 + APP_MAX_LENGTH)) < 0)
        return ret;
    p = pkt.data;
    ff_amf_write_string(&p, "connect");
    ff_amf_write_number(&p, ++rt->nb_invokes);
    ff_amf_write_object_start(&p);
    ff_amf_write_field_name(&p, "app");
    ff_amf_write_string2(&p, rt->app, rt->auth_params);
    if (!rt->is_input) {
        ff_amf_write_field_name(&p, "type");
        ff_amf_write_string(&p, "nonprivate");
    }
    ff_amf_write_field_name(&p, "flashVer");
    ff_amf_write_string(&p, rt->flashver);
    if (rt->swfurl) {
        ff_amf_write_field_name(&p, "swfUrl");
        ff_amf_write_string(&p, rt->swfurl);
    }
    ff_amf_write_field_name(&p, "tcUrl");
    ff_amf_write_string2(&p, rt->tcurl, rt->auth_params);
    if (rt->is_input) {
        ff_amf_write_field_name(&p, "fpad");
        ff_amf_write_bool(&p, 0);
        ff_amf_write_field_name(&p, "capabilities");
        ff_amf_write_number(&p, 15.0);
        


        ff_amf_write_field_name(&p, "audioCodecs");
        ff_amf_write_number(&p, 4071.0);
        ff_amf_write_field_name(&p, "videoCodecs");
        ff_amf_write_number(&p, 252.0);
        ff_amf_write_field_name(&p, "videoFunction");
        ff_amf_write_number(&p, 1.0);
        if (rt->pageurl) {
            ff_amf_write_field_name(&p, "pageUrl");
            ff_amf_write_string(&p, rt->pageurl);
        }
    }
    ff_amf_write_object_end(&p);
    if (rt->conn) {
        char *param = rt->conn;
        
        while (param) {
            char *sep;
            param += strspn(param, " ");
            if (!*param)
                break;
            sep = strchr(param, ' ');
            if (sep)
                *sep = '\0';
            if ((ret = rtmp_write_amf_data(s, param, &p)) < 0) {
                
                ff_rtmp_packet_destroy(&pkt);
                return ret;
            }
            if (sep)
                param = sep + 1;
            else
                break;
        }
    }
    pkt.size = p - pkt.data;
    return rtmp_send_packet(rt, &pkt, 1);
}
