unsigned LowToHigh(unsigned char* addr,unsigned int len)    /*低字节在前高字节在后*/
{
    unsigned i;
    unsigned out = 0;

    if( len > 4 )
    {   
        return -1;
    }

    for( i = 0; i < len; i++ )
    {
        out += ( *(addr+i) ) << ( 8*i );
    }

    return out;    
} 

unsigned HighToLow(unsigned char* addr,unsigned int len)    /*高字节在前低字节在后*/
{
    unsigned int i;
    unsigned int out = 0;

    if( len > 4 )
    {
        return -1;
    }
    
    for( i = 0; i < len; i++ )
    {
        out += ( *(addr+i) ) << ( (8*(len-i-1)) );
    }
    
    return out;    
}