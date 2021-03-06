#ifndef _RTP_H_
#define _RTP_H_
#include <stdint.h>
#include <stdlib.h>

#define RTP_VESION              2

#define RTP_PAYLOAD_TYPE_H264   96
#define RTP_PAYLOAD_TYPE_AAC    0

#define RTP_HEADER_SIZE         12
#define RTP_MAX_PKT_SIZE        1400
#define RTP_EXT_SIZE 204
typedef struct Extern_Data{
		uint8_t info_type;
		uint8_t notuse;
		uint8_t length_h;
		uint8_t length_l;
		char xbuf[200];
}extern_data;

struct Ext_RtpHeader
{
    /* byte 0 */
    uint8_t csrcLen:4;
    uint8_t extension:1;
    uint8_t padding:1;
    uint8_t version:2;

    /* byte 1 */
    uint8_t payloadType:7;
    uint8_t marker:1;
    
    /* bytes 2,3 */
    uint16_t seq;
    
    /* bytes 4-7 */
    uint32_t timestamp;
    
    /* bytes 8-11 */
    uint32_t ssrc;
	
	extern_data xdata;

	/* data */
    uint8_t payload[0];
};


struct RtpHeader
{
    /* byte 0 */
    uint8_t csrcLen:4;
    uint8_t extension:1;
    uint8_t padding:1;
    uint8_t version:2;

    /* byte 1 */
    uint8_t payloadType:7;
    uint8_t marker:1;
    
    /* bytes 2,3 */
    uint16_t seq;
    
    /* bytes 4-7 */
    uint32_t timestamp;
    
    /* bytes 8-11 */
    uint32_t ssrc;
	/* data */
    uint8_t payload[0];
};

class RtpPacket
{
public:
    RtpPacket() :
        _mBuffer(new uint8_t[RTP_MAX_PKT_SIZE+RTP_HEADER_SIZE+RTP_EXT_SIZE+100]),
        mBuffer(_mBuffer+4),
        mRtpHeadr((RtpHeader*)mBuffer),
        mExtRtpHeadr((Ext_RtpHeader*)mBuffer),
        mSize(0)
    {
        
    }

    ~RtpPacket()
    {
        delete _mBuffer;
    }

    uint8_t* _mBuffer;
    uint8_t* mBuffer;
    RtpHeader* const mRtpHeadr;
	Ext_RtpHeader* const mExtRtpHeadr;
    int mSize;
};

#endif //_RTP_H_