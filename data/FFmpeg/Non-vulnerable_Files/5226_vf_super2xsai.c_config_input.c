static int config_input(AVFilterLink *inlink)
{
    Super2xSaIContext *sai = inlink->dst->priv;
    sai->hi_pixel_mask   = 0xFEFEFEFE;
    sai->lo_pixel_mask   = 0x01010101;
    sai->q_hi_pixel_mask = 0xFCFCFCFC;
    sai->q_lo_pixel_mask = 0x03030303;
    sai->bpp  = 4;
    switch (inlink->format) {
    case AV_PIX_FMT_RGB24:
    case AV_PIX_FMT_BGR24:
        sai->bpp = 3;
        break;
    case AV_PIX_FMT_RGB565BE:
    case AV_PIX_FMT_BGR565BE:
        sai->is_be = 1;
    case AV_PIX_FMT_RGB565LE:
    case AV_PIX_FMT_BGR565LE:
        sai->hi_pixel_mask   = 0xF7DEF7DE;
        sai->lo_pixel_mask   = 0x08210821;
        sai->q_hi_pixel_mask = 0xE79CE79C;
        sai->q_lo_pixel_mask = 0x18631863;
        sai->bpp = 2;
        break;
    case AV_PIX_FMT_BGR555BE:
    case AV_PIX_FMT_RGB555BE:
        sai->is_be = 1;
    case AV_PIX_FMT_BGR555LE:
    case AV_PIX_FMT_RGB555LE:
        sai->hi_pixel_mask   = 0x7BDE7BDE;
        sai->lo_pixel_mask   = 0x04210421;
        sai->q_hi_pixel_mask = 0x739C739C;
        sai->q_lo_pixel_mask = 0x0C630C63;
        sai->bpp = 2;
        break;
    }
    return 0;
}
