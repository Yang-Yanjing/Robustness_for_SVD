static int interpol(MBContext *mb, uint32_t *color, int x, int y, int linesize)
{
    uint32_t a,b,c,d, i;
    uint32_t ipol=0xFF000000;
    int dist;
    if(!x || !y || x+1==mb->w || y+1==mb->h)
        return 0;
    dist= FFMAX(FFABS(x-(mb->w>>1))*mb->h, FFABS(y-(mb->h>>1))*mb->w);
    if(dist<(mb->w*mb->h>>3))
        return 0;
    a=color[(x+1) + (y+0)*linesize];
    b=color[(x-1) + (y+1)*linesize];
    c=color[(x+0) + (y+1)*linesize];
    d=color[(x+1) + (y+1)*linesize];
    if(a&&c){
        b= color[(x-1) + (y+0)*linesize];
        d= color[(x+0) + (y-1)*linesize];
    }else if(b&&d){
        a= color[(x+1) + (y-1)*linesize];
        c= color[(x-1) + (y-1)*linesize];
    }else if(c){
        d= color[(x+0) + (y-1)*linesize];
        a= color[(x-1) + (y+0)*linesize];
        b= color[(x+1) + (y-1)*linesize];
    }else if(d){
        c= color[(x-1) + (y-1)*linesize];
        a= color[(x-1) + (y+0)*linesize];
        b= color[(x+1) + (y-1)*linesize];
    }else
        return 0;
    for(i=0; i<3; i++){
        int s= 8*i;
        uint8_t ac= a>>s;
        uint8_t bc= b>>s;
        uint8_t cc= c>>s;
        uint8_t dc= d>>s;
        int ipolab= (ac + bc);
        int ipolcd= (cc + dc);
        if(FFABS(ipolab - ipolcd) > 5)
            return 0;
        if(FFABS(ac-bc)+FFABS(cc-dc) > 20)
            return 0;
        ipol |= ((ipolab + ipolcd + 2)/4)<<s;
    }
    color[x + y*linesize]= ipol;
    return 1;
}
