#ifndef _INV_CRC_H_
#define _INV_CRC_H_

namespace inv {
class INV_Crc {
public:
    static unsigned short crc16(const char *buf, int len);
};
}

#endif

