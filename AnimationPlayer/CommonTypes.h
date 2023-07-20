// author: Geonho Shin (icefin@pearlabyss.com)
#pragma once
//Signed integer types
typedef signed char			int8;
typedef signed short		int16;
typedef signed int			int32;
typedef signed long long	int64;

//Unsigned integer types
typedef unsigned char		byte;
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint;		//synonym of uint32 for HLSL/C++ compatibility
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

typedef char				utf8;
typedef wchar_t				utf16;

struct QuantizedQuaternion
{
    uint16 largest    : 2;  // The largest component of the quaternion.
    uint16 a          : 14;
    uint16 sign_a     : 1;
    uint16 sign_b     : 1;
    uint16 b          : 14;
    uint16 sign_c     : 1;
    uint16 sign       : 1;  // The sign of the largest component. 1 for negative.
    uint16 c          : 14;
};