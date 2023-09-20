#ifndef _CRC_H
#define _CRC_H

extern wres_t Crc_Eval32( uint8_t *byteIn, uint8_t len, uint32_t poly, uint32_t startValue, uint32_t xorValue, bool inReflected, bool outReflected, uint32_t *crc );

#endif