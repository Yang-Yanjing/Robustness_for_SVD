static void write_styp(AVIOContext *pb)
{
    avio_wb32(pb, 24);
    ffio_wfourcc(pb, "styp");
    ffio_wfourcc(pb, "msdh");
    avio_wb32(pb, 0); 
    ffio_wfourcc(pb, "msdh");
    ffio_wfourcc(pb, "msix");
}
