static int find_tracked_method(URLContext *s, RTMPPacket *pkt, int offset,
                               char **tracked_method)
{
    RTMPContext *rt = s->priv_data;
    GetByteContext gbc;
    double pkt_id;
    int ret;
    int i;
    bytestream2_init(&gbc, pkt->data + offset, pkt->size - offset);
    if ((ret = ff_amf_read_number(&gbc, &pkt_id)) < 0)
        return ret;
    for (i = 0; i < rt->nb_tracked_methods; i++) {
        if (rt->tracked_methods[i].id != pkt_id)
            continue;
        *tracked_method = rt->tracked_methods[i].name;
        del_tracked_method(rt, i);
        break;
    }
    return 0;
}
