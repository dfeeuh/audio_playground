#pragma once

/*
typedef char AUDIO_FORMAT_TYPE;
#define FORMAT RTAUDIO_SINT8
#define SCALE  127.0
*/

/*
typedef signed short AUDIO_FORMAT_TYPE;
#define FORMAT RTAUDIO_SINT16
#define SCALE 32767.0
*/

/*
typedef S24 AUDIO_FORMAT_TYPE;
#define FORMAT RTAUDIO_SINT24
#define SCALE  8388607.0

typedef signed long AUDIO_FORMAT_TYPE;
#define FORMAT RTAUDIO_SINT32
#define SCALE  2147483647.0
*/
typedef float AUDIO_FORMAT_TYPE;
#define FORMAT RTAUDIO_FLOAT32
#define SCALE  1.0f
/*
typedef double AUDIO_FORMAT_TYPE;
#define FORMAT RTAUDIO_FLOAT64
#define SCALE  1.0
*/
