#include <wtypes.h>
#include <crc.h>

/**************************************************************************************************
 *  FUNCTION PROTOTYPES                                                                           *
 **************************************************************************************************/
//static wres_t Saej_CRC( uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t poly, uint32_t startvalue, uint32_t *crc )
static uint8_t Crc_Reflect8( uint8_t byteIn, bool reflect );
static uint32_t Crc_Reflect32( uint32_t longIn, bool reflect );

//------------------------------------------------------------------------------
static uint8_t Crc_Reflect8( uint8_t byteIn, bool reflect )
{
    uint8_t aux;
    uint8_t i;
    
    aux = 0U;
    if ( true == reflect )
    {
        for ( i = 0U; i<8U; i++ )
        {
            aux  = aux | ( ( ( byteIn >> ( 7u - i ) ) & 0x01u ) << i );
        }
    }
    else
    {
        aux = byteIn;
    }
    
    return ( aux );
}

//------------------------------------------------------------------------------
static uint32_t Crc_Reflect32( uint32_t longIn, bool reflect )
{
    uint32_t aux;
    uint8_t i;
    
    aux = 0U;
    if ( true == reflect )
    {
        for ( i = 0U; i<32U; i++ )
        {
            aux  = aux | ( ( ( longIn >> ( 31U - i ) ) & 0x01U ) << i );
        }
    }
    else
    {
        aux = longIn;
    }
    
    return ( aux );
}

//------------------------------------------------------------------------------
wres_t Crc_Eval32( uint8_t *byteIn, uint8_t len, uint32_t poly, uint32_t startValue, uint32_t xorValue, bool inReflected, bool outReflected, uint32_t *crc )
{
    wres_t res;
    bool toDiv;
    uint32_t shiftRegister;
    uint8_t usedLen;
    
    uint16_t i;
    uint16_t j;
    
    res = WRES_OK;
    
    i = 0U;
    
    shiftRegister = startValue;
    usedLen = 4U;
    if ( len < 4U )
    {
        usedLen = len;
    }
    for ( i=0U; i<usedLen; i++ )
    {
        shiftRegister = shiftRegister ^ ( (uint32_t)Crc_Reflect8( *( byteIn + i ), inReflected ) << (8U*( usedLen - i - 1U )) );
    }
    
    if ( len > 4U )
    {
        for ( j=4U; j<len; j++ )
        {
            for ( i=0U; i<8U; i++ )
            {
                toDiv = false;
                if ( (shiftRegister & 0x80000000UL)!=0u )
                {
                    toDiv = true;
                }
                shiftRegister = shiftRegister << 1U;
                shiftRegister |= (uint32_t)( (uint32_t)( Crc_Reflect8( *( byteIn + j ), inReflected ) >> ( 7u - i ) ) & 0x00000001UL );
                if ( true == toDiv )
                {
                    shiftRegister = shiftRegister ^ poly;
                }
            }
        }
    }
    for ( j=0U; j<32U; j++ )
    {
        toDiv = false;
        if ( (shiftRegister & 0x80000000UL) !=0u )
        {
            toDiv = true;
        }
        shiftRegister = shiftRegister << 1U;
        if ( true == toDiv )
        {
            shiftRegister = shiftRegister ^ poly;
        }
    }
    
    shiftRegister = shiftRegister ^ xorValue; 
    
    
    *crc = Crc_Reflect32( shiftRegister, outReflected );    
    
    return res;
}