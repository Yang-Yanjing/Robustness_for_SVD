static const struct rtp_pt_t *
rtp_find_ptype (const rtp_session_t *session, rtp_source_t *source,
                const block_t *block, void **pt_data)
{
    uint8_t ptype = rtp_ptype (block);
    for (unsigned i = 0; i < session->ptc; i++)
    {
        if (session->ptv[i].number == ptype)
        {
            if (pt_data != NULL)
                *pt_data = source->opaque[i];
            return &session->ptv[i];
        }
    }
    return NULL;
}
