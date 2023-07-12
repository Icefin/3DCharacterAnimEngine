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
	bool isPositive = true;
	int16 qy, qz, qw;
};