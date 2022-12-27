/*
 *  alad - glad-like OpenAL-loader by Hypatia of Sva
 * 
 *  Usage:
 *
 *  Include this file once with
 *
 *          #define ALAD_IMPLEMENTATION
 *
 *  to define the functions and global function pointers, and include this file without this symbol defined in all other translation units.
 * 
 *  This header loads in all OpenAL symbols except the functions. 
 *  Initalize those with
 * 
 *          aladLoadAL(NULL);
 * 
 *  for default initialization, or with
 * 
 *          aladLoadAL(my_alGetProcAddress);
 * 
 *  where my_alGetProcAddress is a loader function of type LPALGETPROCADDRESS for custom initalization.
 *  Default initialization will pull in OpenAL32.dll / soft_oal.dll on Windows or libopenal.so / libopenal.so.1 on Unix respectively.
 *  Make sure one of these dynamic libraries are on path for LoadLibraryA / dlopen, change the code below in _alad_open or provide your own function loader.
 *  The Library will only be initalized once, you can call "aladLoadAL(NULL)" as often as you want to reload the pointer from the loaded shared library.
 *
 *  Update ALC pointers to those loaded with a specific ALCdevice with
 * 
 *          aladUpdateALCPointers(device);
 *
 *  If you want to remove this reference to the device, reload them to the nonspecific pointers by calling "aladLoadAL" again.
 * 
 *  Unload the library loaded with default initalization with
 * 
 *          aladTerminate();
 *
 *  This will not unload the function pointers or set them to NULL, but they will be probably invalid after unloading the shared library.
 * 
 *  
 *  alad will attempt to load all function pointers in all extensions as currently described in the openal-soft headers
 *  (repo version from 24.12.2022, commit https://github.com/kcat/openal-soft/commit/4fe6eba8c79a4c9cad91d6f6835506cde96a48c4),
 *  the ones not available will be NULL, you have to check all functions you want to use for that. you have to check all functions you want to use for that.
 *  The ALC_INVALID_VALUE-Error-State of alcGetProcAddress is not being checked.
 *  If a functionality you expect to be present isn't working, you might want to check the spelling of the
 *          strings in the internal _alad_load_alc_functions function.
 * 
 * 
 * 
 * 
 * Copyright notice from the original OpenAL header alext.h:
 * OpenAL cross platform audio library
 * Copyright (C) 2008 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#pragma once

//only include alad if the OpenAL headers are not used, because we redefine the function and type names
#if !(defined(ALAD_H)) && !(defined(AL_AL_H)) && !(defined(AL_ALC_H)) && !(defined(AL_ALEXT_H)) && !(defined(AL_EFX_H)) && !(defined(EFX_PRESETS_H))
#define ALAD_H

//we don't want to include the actual headers, so we define the header guards
#define AL_AL_H
#define AL_ALC_H
#define AL_ALEXT_H
#define AL_EFX_H
#define EFX_PRESETS_H

//condensed contents of the five header files without comments, function prototypes and ifdefs
#include <stddef.h>
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) ||             \
    (defined(__cplusplus) && __cplusplus >= 201103L)
#include <stdint.h>
typedef int64_t _alsoft_int64_t;
typedef uint64_t _alsoft_uint64_t;
#elif defined(_WIN32)
typedef __int64 _alsoft_int64_t;
typedef unsigned __int64 _alsoft_uint64_t;
#else
#include <stdint.h>
typedef int64_t _alsoft_int64_t;
typedef uint64_t _alsoft_uint64_t;
#endif
#include <float.h>

#if defined(AL_LIBTYPE_STATIC)
 #define AL_API
#elif defined(_WIN32)
 #define AL_API __declspec(dllimport)
#else
 #define AL_API extern
#endif
#define ALC_API AL_API

#if defined(_WIN32)
 #define AL_APIENTRY __cdecl
#else
 #define AL_APIENTRY
#endif
#define ALC_APIENTRY AL_APIENTRY

#define OPENAL
#define ALAPI                                    AL_API
#define ALAPIENTRY                               AL_APIENTRY
#define AL_INVALID                               (-1)
#define AL_ILLEGAL_ENUM                          AL_INVALID_ENUM
#define AL_ILLEGAL_COMMAND                       AL_INVALID_OPERATION
#define ALCAPI                                   ALC_API
#define ALCAPIENTRY                              ALC_APIENTRY
#define ALC_INVALID                              0

#define AL_VERSION_1_0
#define AL_VERSION_1_1
#define ALC_VERSION_0_1                          1

typedef char ALboolean;
typedef char ALchar;
typedef signed char ALbyte;
typedef unsigned char ALubyte;
typedef short ALshort;
typedef unsigned short ALushort;
typedef int ALint;
typedef unsigned int ALuint;
typedef int ALsizei;
typedef int ALenum;
typedef float ALfloat;
typedef double ALdouble;
typedef void ALvoid;

typedef struct ALCdevice ALCdevice;
typedef struct ALCcontext ALCcontext;
typedef char ALCboolean;
typedef char ALCchar;
typedef signed char ALCbyte;
typedef unsigned char ALCubyte;
typedef short ALCshort;
typedef unsigned short ALCushort;
typedef int ALCint;
typedef unsigned int ALCuint;
typedef int ALCsizei;
typedef int ALCenum;
typedef float ALCfloat;
typedef double ALCdouble;
typedef void ALCvoid;

typedef _alsoft_int64_t ALint64SOFT;
typedef _alsoft_uint64_t ALuint64SOFT;
typedef _alsoft_int64_t ALCint64SOFT;
typedef _alsoft_uint64_t ALCuint64SOFT;

#define EFXEAXREVERBPROPERTIES_DEFINED
typedef struct {
    float flDensity;
    float flDiffusion;
    float flGain;
    float flGainHF;
    float flGainLF;
    float flDecayTime;
    float flDecayHFRatio;
    float flDecayLFRatio;
    float flReflectionsGain;
    float flReflectionsDelay;
    float flReflectionsPan[3];
    float flLateReverbGain;
    float flLateReverbDelay;
    float flLateReverbPan[3];
    float flEchoTime;
    float flEchoDepth;
    float flModulationTime;
    float flModulationDepth;
    float flAirAbsorptionGainHF;
    float flHFReference;
    float flLFReference;
    float flRoomRolloffFactor;
    int   iDecayHFLimit;
} EFXEAXREVERBPROPERTIES, *LPEFXEAXREVERBPROPERTIES;

#define AL_NONE                                  0
#define AL_FALSE                                 0
#define AL_TRUE                                  1
#define AL_SOURCE_RELATIVE                       0x202
#define AL_CONE_INNER_ANGLE                      0x1001
#define AL_CONE_OUTER_ANGLE                      0x1002
#define AL_PITCH                                 0x1003
#define AL_POSITION                              0x1004
#define AL_DIRECTION                             0x1005
#define AL_VELOCITY                              0x1006
#define AL_LOOPING                               0x1007
#define AL_BUFFER                                0x1009
#define AL_GAIN                                  0x100A
#define AL_MIN_GAIN                              0x100D
#define AL_MAX_GAIN                              0x100E
#define AL_ORIENTATION                           0x100F
#define AL_SOURCE_STATE                          0x1010
#define AL_INITIAL                               0x1011
#define AL_PLAYING                               0x1012
#define AL_PAUSED                                0x1013
#define AL_STOPPED                               0x1014
#define AL_BUFFERS_QUEUED                        0x1015
#define AL_BUFFERS_PROCESSED                     0x1016
#define AL_REFERENCE_DISTANCE                    0x1020
#define AL_ROLLOFF_FACTOR                        0x1021
#define AL_CONE_OUTER_GAIN                       0x1022
#define AL_MAX_DISTANCE                          0x1023
#define AL_SEC_OFFSET                            0x1024
#define AL_SAMPLE_OFFSET                         0x1025
#define AL_BYTE_OFFSET                           0x1026
#define AL_SOURCE_TYPE                           0x1027
#define AL_STATIC                                0x1028
#define AL_STREAMING                             0x1029
#define AL_UNDETERMINED                          0x1030
#define AL_FORMAT_MONO8                          0x1100
#define AL_FORMAT_MONO16                         0x1101
#define AL_FORMAT_STEREO8                        0x1102
#define AL_FORMAT_STEREO16                       0x1103
#define AL_FREQUENCY                             0x2001
#define AL_BITS                                  0x2002
#define AL_CHANNELS                              0x2003
#define AL_SIZE                                  0x2004
#define AL_UNUSED                                0x2010
#define AL_PENDING                               0x2011
#define AL_PROCESSED                             0x2012
#define AL_NO_ERROR                              0
#define AL_INVALID_NAME                          0xA001
#define AL_INVALID_ENUM                          0xA002
#define AL_INVALID_VALUE                         0xA003
#define AL_INVALID_OPERATION                     0xA004
#define AL_OUT_OF_MEMORY                         0xA005
#define AL_VENDOR                                0xB001
#define AL_VERSION                               0xB002
#define AL_RENDERER                              0xB003
#define AL_EXTENSIONS                            0xB004
#define AL_DOPPLER_FACTOR                        0xC000
#define AL_DOPPLER_VELOCITY                      0xC001
#define AL_SPEED_OF_SOUND                        0xC003
#define AL_DISTANCE_MODEL                        0xD000
#define AL_INVERSE_DISTANCE                      0xD001
#define AL_INVERSE_DISTANCE_CLAMPED              0xD002
#define AL_LINEAR_DISTANCE                       0xD003
#define AL_LINEAR_DISTANCE_CLAMPED               0xD004
#define AL_EXPONENT_DISTANCE                     0xD005
#define AL_EXPONENT_DISTANCE_CLAMPED             0xD006

#define ALC_FALSE                                0
#define ALC_TRUE                                 1
#define ALC_FREQUENCY                            0x1007
#define ALC_REFRESH                              0x1008
#define ALC_SYNC                                 0x1009
#define ALC_MONO_SOURCES                         0x1010
#define ALC_STEREO_SOURCES                       0x1011
#define ALC_NO_ERROR                             0
#define ALC_INVALID_DEVICE                       0xA001
#define ALC_INVALID_CONTEXT                      0xA002
#define ALC_INVALID_ENUM                         0xA003
#define ALC_INVALID_VALUE                        0xA004
#define ALC_OUT_OF_MEMORY                        0xA005
#define ALC_MAJOR_VERSION                        0x1000
#define ALC_MINOR_VERSION                        0x1001
#define ALC_ATTRIBUTES_SIZE                      0x1002
#define ALC_ALL_ATTRIBUTES                       0x1003
#define ALC_DEFAULT_DEVICE_SPECIFIER             0x1004
#define ALC_DEVICE_SPECIFIER                     0x1005
#define ALC_EXTENSIONS                           0x1006
#define ALC_EXT_CAPTURE 1
#define ALC_CAPTURE_DEVICE_SPECIFIER             0x310
#define ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER     0x311
#define ALC_CAPTURE_SAMPLES                      0x312
#define ALC_ENUMERATE_ALL_EXT 1
#define ALC_DEFAULT_ALL_DEVICES_SPECIFIER        0x1012
#define ALC_ALL_DEVICES_SPECIFIER                0x1013

#define AL_LOKI_IMA_ADPCM_format 1
#define AL_FORMAT_IMA_ADPCM_MONO16_EXT           0x10000
#define AL_FORMAT_IMA_ADPCM_STEREO16_EXT         0x10001
#define AL_LOKI_WAVE_format 1
#define AL_FORMAT_WAVE_EXT                       0x10002
#define AL_EXT_vorbis 1
#define AL_FORMAT_VORBIS_EXT                     0x10003
#define AL_LOKI_quadriphonic 1
#define AL_FORMAT_QUAD8_LOKI                     0x10004
#define AL_FORMAT_QUAD16_LOKI                    0x10005
#define AL_EXT_float32 1
#define AL_FORMAT_MONO_FLOAT32                   0x10010
#define AL_FORMAT_STEREO_FLOAT32                 0x10011
#define AL_EXT_double 1
#define AL_FORMAT_MONO_DOUBLE_EXT                0x10012
#define AL_FORMAT_STEREO_DOUBLE_EXT              0x10013
#define AL_EXT_MULAW 1
#define AL_FORMAT_MONO_MULAW_EXT                 0x10014
#define AL_FORMAT_STEREO_MULAW_EXT               0x10015
#define AL_EXT_ALAW 1
#define AL_FORMAT_MONO_ALAW_EXT                  0x10016
#define AL_FORMAT_STEREO_ALAW_EXT                0x10017
#define ALC_LOKI_audio_channel 1
#define ALC_CHAN_MAIN_LOKI                       0x500001
#define ALC_CHAN_PCM_LOKI                        0x500002
#define ALC_CHAN_CD_LOKI                         0x500003
#define AL_EXT_MCFORMATS 1
#define AL_FORMAT_QUAD8                          0x1204
#define AL_FORMAT_QUAD16                         0x1205
#define AL_FORMAT_QUAD32                         0x1206
#define AL_FORMAT_REAR8                          0x1207
#define AL_FORMAT_REAR16                         0x1208
#define AL_FORMAT_REAR32                         0x1209
#define AL_FORMAT_51CHN8                         0x120A
#define AL_FORMAT_51CHN16                        0x120B
#define AL_FORMAT_51CHN32                        0x120C
#define AL_FORMAT_61CHN8                         0x120D
#define AL_FORMAT_61CHN16                        0x120E
#define AL_FORMAT_61CHN32                        0x120F
#define AL_FORMAT_71CHN8                         0x1210
#define AL_FORMAT_71CHN16                        0x1211
#define AL_FORMAT_71CHN32                        0x1212
#define AL_EXT_MULAW_MCFORMATS 1
#define AL_FORMAT_MONO_MULAW                     0x10014
#define AL_FORMAT_STEREO_MULAW                   0x10015
#define AL_FORMAT_QUAD_MULAW                     0x10021
#define AL_FORMAT_REAR_MULAW                     0x10022
#define AL_FORMAT_51CHN_MULAW                    0x10023
#define AL_FORMAT_61CHN_MULAW                    0x10024
#define AL_FORMAT_71CHN_MULAW                    0x10025
#define AL_EXT_IMA4 1
#define AL_FORMAT_MONO_IMA4                      0x1300
#define AL_FORMAT_STEREO_IMA4                    0x1301
#define AL_EXT_STATIC_BUFFER 1
#define ALC_EXT_EFX 1
#define ALC_EXT_disconnect 1
#define ALC_CONNECTED                            0x313
#define ALC_EXT_thread_local_context 1
#define AL_EXT_source_distance_model 1
#define AL_SOURCE_DISTANCE_MODEL                 0x200
#define AL_SOFT_buffer_sub_data 1
#define AL_BYTE_RW_OFFSETS_SOFT                  0x1031
#define AL_SAMPLE_RW_OFFSETS_SOFT                0x1032
#define AL_SOFT_loop_points 1
#define AL_LOOP_POINTS_SOFT                      0x2015
#define AL_EXT_FOLDBACK 1
#define AL_EXT_FOLDBACK_NAME                     "AL_EXT_FOLDBACK"
#define AL_FOLDBACK_EVENT_BLOCK                  0x4112
#define AL_FOLDBACK_EVENT_START                  0x4111
#define AL_FOLDBACK_EVENT_STOP                   0x4113
#define AL_FOLDBACK_MODE_MONO                    0x4101
#define AL_FOLDBACK_MODE_STEREO                  0x4102
#define ALC_EXT_DEDICATED 1
#define AL_DEDICATED_GAIN                        0x0001
#define AL_EFFECT_DEDICATED_DIALOGUE             0x9001
#define AL_EFFECT_DEDICATED_LOW_FREQUENCY_EFFECT 0x9000
#define AL_SOFT_buffer_samples 1
#define AL_MONO_SOFT                             0x1500
#define AL_STEREO_SOFT                           0x1501
#define AL_REAR_SOFT                             0x1502
#define AL_QUAD_SOFT                             0x1503
#define AL_5POINT1_SOFT                          0x1504
#define AL_6POINT1_SOFT                          0x1505
#define AL_7POINT1_SOFT                          0x1506
#define AL_BYTE_SOFT                             0x1400
#define AL_UNSIGNED_BYTE_SOFT                    0x1401
#define AL_SHORT_SOFT                            0x1402
#define AL_UNSIGNED_SHORT_SOFT                   0x1403
#define AL_INT_SOFT                              0x1404
#define AL_UNSIGNED_INT_SOFT                     0x1405
#define AL_FLOAT_SOFT                            0x1406
#define AL_DOUBLE_SOFT                           0x1407
#define AL_BYTE3_SOFT                            0x1408
#define AL_UNSIGNED_BYTE3_SOFT                   0x1409
#define AL_MONO8_SOFT                            0x1100
#define AL_MONO16_SOFT                           0x1101
#define AL_MONO32F_SOFT                          0x10010
#define AL_STEREO8_SOFT                          0x1102
#define AL_STEREO16_SOFT                         0x1103
#define AL_STEREO32F_SOFT                        0x10011
#define AL_QUAD8_SOFT                            0x1204
#define AL_QUAD16_SOFT                           0x1205
#define AL_QUAD32F_SOFT                          0x1206
#define AL_REAR8_SOFT                            0x1207
#define AL_REAR16_SOFT                           0x1208
#define AL_REAR32F_SOFT                          0x1209
#define AL_5POINT1_8_SOFT                        0x120A
#define AL_5POINT1_16_SOFT                       0x120B
#define AL_5POINT1_32F_SOFT                      0x120C
#define AL_6POINT1_8_SOFT                        0x120D
#define AL_6POINT1_16_SOFT                       0x120E
#define AL_6POINT1_32F_SOFT                      0x120F
#define AL_7POINT1_8_SOFT                        0x1210
#define AL_7POINT1_16_SOFT                       0x1211
#define AL_7POINT1_32F_SOFT                      0x1212
#define AL_INTERNAL_FORMAT_SOFT                  0x2008
#define AL_BYTE_LENGTH_SOFT                      0x2009
#define AL_SAMPLE_LENGTH_SOFT                    0x200A
#define AL_SEC_LENGTH_SOFT                       0x200B
#define AL_SOFT_direct_channels 1
#define AL_DIRECT_CHANNELS_SOFT                  0x1033
#define ALC_SOFT_loopback 1
#define ALC_FORMAT_CHANNELS_SOFT                 0x1990
#define ALC_FORMAT_TYPE_SOFT                     0x1991
#define ALC_BYTE_SOFT                            0x1400
#define ALC_UNSIGNED_BYTE_SOFT                   0x1401
#define ALC_SHORT_SOFT                           0x1402
#define ALC_UNSIGNED_SHORT_SOFT                  0x1403
#define ALC_INT_SOFT                             0x1404
#define ALC_UNSIGNED_INT_SOFT                    0x1405
#define ALC_FLOAT_SOFT                           0x1406
#define ALC_MONO_SOFT                            0x1500
#define ALC_STEREO_SOFT                          0x1501
#define ALC_QUAD_SOFT                            0x1503
#define ALC_5POINT1_SOFT                         0x1504
#define ALC_6POINT1_SOFT                         0x1505
#define ALC_7POINT1_SOFT                         0x1506
#define AL_EXT_STEREO_ANGLES 1
#define AL_STEREO_ANGLES                         0x1030
#define AL_EXT_SOURCE_RADIUS 1
#define AL_SOURCE_RADIUS                         0x1031
#define AL_SOFT_source_latency 1
#define AL_SAMPLE_OFFSET_LATENCY_SOFT            0x1200
#define AL_SEC_OFFSET_LATENCY_SOFT               0x1201
#define ALC_EXT_DEFAULT_FILTER_ORDER 1
#define ALC_DEFAULT_FILTER_ORDER                 0x1100
#define AL_SOFT_deferred_updates 1
#define AL_DEFERRED_UPDATES_SOFT                 0xC002
#define AL_SOFT_block_alignment 1
#define AL_UNPACK_BLOCK_ALIGNMENT_SOFT           0x200C
#define AL_PACK_BLOCK_ALIGNMENT_SOFT             0x200D
#define AL_SOFT_MSADPCM 1
#define AL_FORMAT_MONO_MSADPCM_SOFT              0x1302
#define AL_FORMAT_STEREO_MSADPCM_SOFT            0x1303
#define AL_SOFT_source_length 1
#define ALC_SOFT_pause_device 1
#define AL_EXT_BFORMAT 1
#define AL_FORMAT_BFORMAT2D_8                    0x20021
#define AL_FORMAT_BFORMAT2D_16                   0x20022
#define AL_FORMAT_BFORMAT2D_FLOAT32              0x20023
#define AL_FORMAT_BFORMAT3D_8                    0x20031
#define AL_FORMAT_BFORMAT3D_16                   0x20032
#define AL_FORMAT_BFORMAT3D_FLOAT32              0x20033
#define AL_EXT_MULAW_BFORMAT 1
#define AL_FORMAT_BFORMAT2D_MULAW                0x10031
#define AL_FORMAT_BFORMAT3D_MULAW                0x10032
#define ALC_SOFT_HRTF 1
#define ALC_HRTF_SOFT                            0x1992
#define ALC_DONT_CARE_SOFT                       0x0002
#define ALC_HRTF_STATUS_SOFT                     0x1993
#define ALC_HRTF_DISABLED_SOFT                   0x0000
#define ALC_HRTF_ENABLED_SOFT                    0x0001
#define ALC_HRTF_DENIED_SOFT                     0x0002
#define ALC_HRTF_REQUIRED_SOFT                   0x0003
#define ALC_HRTF_HEADPHONES_DETECTED_SOFT        0x0004
#define ALC_HRTF_UNSUPPORTED_FORMAT_SOFT         0x0005
#define ALC_NUM_HRTF_SPECIFIERS_SOFT             0x1994
#define ALC_HRTF_SPECIFIER_SOFT                  0x1995
#define ALC_HRTF_ID_SOFT                         0x1996
#define AL_SOFT_gain_clamp_ex 1
#define AL_GAIN_LIMIT_SOFT                       0x200E
#define AL_SOFT_source_resampler
#define AL_NUM_RESAMPLERS_SOFT                   0x1210
#define AL_DEFAULT_RESAMPLER_SOFT                0x1211
#define AL_SOURCE_RESAMPLER_SOFT                 0x1212
#define AL_RESAMPLER_NAME_SOFT                   0x1213
#define AL_SOFT_source_spatialize
#define AL_SOURCE_SPATIALIZE_SOFT                0x1214
#define AL_AUTO_SOFT                             0x0002
#define ALC_SOFT_output_limiter
#define ALC_OUTPUT_LIMITER_SOFT                  0x199A
#define ALC_SOFT_device_clock 1
#define ALC_DEVICE_CLOCK_SOFT                    0x1600
#define ALC_DEVICE_LATENCY_SOFT                  0x1601
#define ALC_DEVICE_CLOCK_LATENCY_SOFT            0x1602
#define AL_SAMPLE_OFFSET_CLOCK_SOFT              0x1202
#define AL_SEC_OFFSET_CLOCK_SOFT                 0x1203
#define AL_SOFT_direct_channels_remix 1
#define AL_DROP_UNMATCHED_SOFT                   0x0001
#define AL_REMIX_UNMATCHED_SOFT                  0x0002
#define AL_SOFT_bformat_ex 1
#define AL_AMBISONIC_LAYOUT_SOFT                 0x1997
#define AL_AMBISONIC_SCALING_SOFT                0x1998
#define AL_FUMA_SOFT                             0x0000
#define AL_ACN_SOFT                              0x0001
#define AL_SN3D_SOFT                             0x0001
#define AL_N3D_SOFT                              0x0002
#define ALC_SOFT_loopback_bformat 1
#define ALC_AMBISONIC_LAYOUT_SOFT                0x1997
#define ALC_AMBISONIC_SCALING_SOFT               0x1998
#define ALC_AMBISONIC_ORDER_SOFT                 0x1999
#define ALC_MAX_AMBISONIC_ORDER_SOFT             0x199B
#define ALC_BFORMAT3D_SOFT                       0x1507
#define ALC_FUMA_SOFT                            0x0000
#define ALC_ACN_SOFT                             0x0001
#define ALC_SN3D_SOFT                            0x0001
#define ALC_N3D_SOFT                             0x0002
#define AL_SOFT_effect_target
#define AL_EFFECTSLOT_TARGET_SOFT                0x199C
#define AL_SOFT_events 1
#define AL_EVENT_CALLBACK_FUNCTION_SOFT          0x19A2
#define AL_EVENT_CALLBACK_USER_PARAM_SOFT        0x19A3
#define AL_EVENT_TYPE_BUFFER_COMPLETED_SOFT      0x19A4
#define AL_EVENT_TYPE_SOURCE_STATE_CHANGED_SOFT  0x19A5
#define AL_EVENT_TYPE_DISCONNECTED_SOFT          0x19A6
#define ALC_SOFT_reopen_device
#define AL_SOFT_callback_buffer
#define AL_BUFFER_CALLBACK_FUNCTION_SOFT         0x19A0
#define AL_BUFFER_CALLBACK_USER_PARAM_SOFT       0x19A1
#define AL_SOFT_UHJ
#define AL_FORMAT_UHJ2CHN8_SOFT                  0x19A2
#define AL_FORMAT_UHJ2CHN16_SOFT                 0x19A3
#define AL_FORMAT_UHJ2CHN_FLOAT32_SOFT           0x19A4
#define AL_FORMAT_UHJ3CHN8_SOFT                  0x19A5
#define AL_FORMAT_UHJ3CHN16_SOFT                 0x19A6
#define AL_FORMAT_UHJ3CHN_FLOAT32_SOFT           0x19A7
#define AL_FORMAT_UHJ4CHN8_SOFT                  0x19A8
#define AL_FORMAT_UHJ4CHN16_SOFT                 0x19A9
#define AL_FORMAT_UHJ4CHN_FLOAT32_SOFT           0x19AA
#define AL_STEREO_MODE_SOFT                      0x19B0
#define AL_NORMAL_SOFT                           0x0000
#define AL_SUPER_STEREO_SOFT                     0x0001
#define AL_SUPER_STEREO_WIDTH_SOFT               0x19B1
#define ALC_SOFT_output_mode
#define ALC_OUTPUT_MODE_SOFT                     0x19AC
#define ALC_ANY_SOFT                             0x19AD
#define ALC_STEREO_BASIC_SOFT                    0x19AE
#define ALC_STEREO_UHJ_SOFT                      0x19AF
#define ALC_STEREO_HRTF_SOFT                     0x19B2
#define ALC_SURROUND_5_1_SOFT                    0x1504
#define ALC_SURROUND_6_1_SOFT                    0x1505
#define ALC_SURROUND_7_1_SOFT                    0x1506

#define ALC_EXT_EFX_NAME                         "ALC_EXT_EFX"
#define ALC_EFX_MAJOR_VERSION                    0x20001
#define ALC_EFX_MINOR_VERSION                    0x20002
#define ALC_MAX_AUXILIARY_SENDS                  0x20003
#define AL_METERS_PER_UNIT                       0x20004
#define AL_DIRECT_FILTER                         0x20005
#define AL_AUXILIARY_SEND_FILTER                 0x20006
#define AL_AIR_ABSORPTION_FACTOR                 0x20007
#define AL_ROOM_ROLLOFF_FACTOR                   0x20008
#define AL_CONE_OUTER_GAINHF                     0x20009
#define AL_DIRECT_FILTER_GAINHF_AUTO             0x2000A
#define AL_AUXILIARY_SEND_FILTER_GAIN_AUTO       0x2000B
#define AL_AUXILIARY_SEND_FILTER_GAINHF_AUTO     0x2000C
#define AL_REVERB_DENSITY                        0x0001
#define AL_REVERB_DIFFUSION                      0x0002
#define AL_REVERB_GAIN                           0x0003
#define AL_REVERB_GAINHF                         0x0004
#define AL_REVERB_DECAY_TIME                     0x0005
#define AL_REVERB_DECAY_HFRATIO                  0x0006
#define AL_REVERB_REFLECTIONS_GAIN               0x0007
#define AL_REVERB_REFLECTIONS_DELAY              0x0008
#define AL_REVERB_LATE_REVERB_GAIN               0x0009
#define AL_REVERB_LATE_REVERB_DELAY              0x000A
#define AL_REVERB_AIR_ABSORPTION_GAINHF          0x000B
#define AL_REVERB_ROOM_ROLLOFF_FACTOR            0x000C
#define AL_REVERB_DECAY_HFLIMIT                  0x000D
#define AL_EAXREVERB_DENSITY                     0x0001
#define AL_EAXREVERB_DIFFUSION                   0x0002
#define AL_EAXREVERB_GAIN                        0x0003
#define AL_EAXREVERB_GAINHF                      0x0004
#define AL_EAXREVERB_GAINLF                      0x0005
#define AL_EAXREVERB_DECAY_TIME                  0x0006
#define AL_EAXREVERB_DECAY_HFRATIO               0x0007
#define AL_EAXREVERB_DECAY_LFRATIO               0x0008
#define AL_EAXREVERB_REFLECTIONS_GAIN            0x0009
#define AL_EAXREVERB_REFLECTIONS_DELAY           0x000A
#define AL_EAXREVERB_REFLECTIONS_PAN             0x000B
#define AL_EAXREVERB_LATE_REVERB_GAIN            0x000C
#define AL_EAXREVERB_LATE_REVERB_DELAY           0x000D
#define AL_EAXREVERB_LATE_REVERB_PAN             0x000E
#define AL_EAXREVERB_ECHO_TIME                   0x000F
#define AL_EAXREVERB_ECHO_DEPTH                  0x0010
#define AL_EAXREVERB_MODULATION_TIME             0x0011
#define AL_EAXREVERB_MODULATION_DEPTH            0x0012
#define AL_EAXREVERB_AIR_ABSORPTION_GAINHF       0x0013
#define AL_EAXREVERB_HFREFERENCE                 0x0014
#define AL_EAXREVERB_LFREFERENCE                 0x0015
#define AL_EAXREVERB_ROOM_ROLLOFF_FACTOR         0x0016
#define AL_EAXREVERB_DECAY_HFLIMIT               0x0017
#define AL_CHORUS_WAVEFORM                       0x0001
#define AL_CHORUS_PHASE                          0x0002
#define AL_CHORUS_RATE                           0x0003
#define AL_CHORUS_DEPTH                          0x0004
#define AL_CHORUS_FEEDBACK                       0x0005
#define AL_CHORUS_DELAY                          0x0006
#define AL_DISTORTION_EDGE                       0x0001
#define AL_DISTORTION_GAIN                       0x0002
#define AL_DISTORTION_LOWPASS_CUTOFF             0x0003
#define AL_DISTORTION_EQCENTER                   0x0004
#define AL_DISTORTION_EQBANDWIDTH                0x0005
#define AL_ECHO_DELAY                            0x0001
#define AL_ECHO_LRDELAY                          0x0002
#define AL_ECHO_DAMPING                          0x0003
#define AL_ECHO_FEEDBACK                         0x0004
#define AL_ECHO_SPREAD                           0x0005
#define AL_FLANGER_WAVEFORM                      0x0001
#define AL_FLANGER_PHASE                         0x0002
#define AL_FLANGER_RATE                          0x0003
#define AL_FLANGER_DEPTH                         0x0004
#define AL_FLANGER_FEEDBACK                      0x0005
#define AL_FLANGER_DELAY                         0x0006
#define AL_FREQUENCY_SHIFTER_FREQUENCY           0x0001
#define AL_FREQUENCY_SHIFTER_LEFT_DIRECTION      0x0002
#define AL_FREQUENCY_SHIFTER_RIGHT_DIRECTION     0x0003
#define AL_VOCAL_MORPHER_PHONEMEA                0x0001
#define AL_VOCAL_MORPHER_PHONEMEA_COARSE_TUNING  0x0002
#define AL_VOCAL_MORPHER_PHONEMEB                0x0003
#define AL_VOCAL_MORPHER_PHONEMEB_COARSE_TUNING  0x0004
#define AL_VOCAL_MORPHER_WAVEFORM                0x0005
#define AL_VOCAL_MORPHER_RATE                    0x0006
#define AL_PITCH_SHIFTER_COARSE_TUNE             0x0001
#define AL_PITCH_SHIFTER_FINE_TUNE               0x0002
#define AL_RING_MODULATOR_FREQUENCY              0x0001
#define AL_RING_MODULATOR_HIGHPASS_CUTOFF        0x0002
#define AL_RING_MODULATOR_WAVEFORM               0x0003
#define AL_AUTOWAH_ATTACK_TIME                   0x0001
#define AL_AUTOWAH_RELEASE_TIME                  0x0002
#define AL_AUTOWAH_RESONANCE                     0x0003
#define AL_AUTOWAH_PEAK_GAIN                     0x0004
#define AL_COMPRESSOR_ONOFF                      0x0001
#define AL_EQUALIZER_LOW_GAIN                    0x0001
#define AL_EQUALIZER_LOW_CUTOFF                  0x0002
#define AL_EQUALIZER_MID1_GAIN                   0x0003
#define AL_EQUALIZER_MID1_CENTER                 0x0004
#define AL_EQUALIZER_MID1_WIDTH                  0x0005
#define AL_EQUALIZER_MID2_GAIN                   0x0006
#define AL_EQUALIZER_MID2_CENTER                 0x0007
#define AL_EQUALIZER_MID2_WIDTH                  0x0008
#define AL_EQUALIZER_HIGH_GAIN                   0x0009
#define AL_EQUALIZER_HIGH_CUTOFF                 0x000A
#define AL_EFFECT_FIRST_PARAMETER                0x0000
#define AL_EFFECT_LAST_PARAMETER                 0x8000
#define AL_EFFECT_TYPE                           0x8001
#define AL_EFFECT_NULL                           0x0000
#define AL_EFFECT_REVERB                         0x0001
#define AL_EFFECT_CHORUS                         0x0002
#define AL_EFFECT_DISTORTION                     0x0003
#define AL_EFFECT_ECHO                           0x0004
#define AL_EFFECT_FLANGER                        0x0005
#define AL_EFFECT_FREQUENCY_SHIFTER              0x0006
#define AL_EFFECT_VOCAL_MORPHER                  0x0007
#define AL_EFFECT_PITCH_SHIFTER                  0x0008
#define AL_EFFECT_RING_MODULATOR                 0x0009
#define AL_EFFECT_AUTOWAH                        0x000A
#define AL_EFFECT_COMPRESSOR                     0x000B
#define AL_EFFECT_EQUALIZER                      0x000C
#define AL_EFFECT_EAXREVERB                      0x8000
#define AL_EFFECTSLOT_EFFECT                     0x0001
#define AL_EFFECTSLOT_GAIN                       0x0002
#define AL_EFFECTSLOT_AUXILIARY_SEND_AUTO        0x0003
#define AL_EFFECTSLOT_NULL                       0x0000
#define AL_LOWPASS_GAIN                          0x0001
#define AL_LOWPASS_GAINHF                        0x0002
#define AL_HIGHPASS_GAIN                         0x0001
#define AL_HIGHPASS_GAINLF                       0x0002
#define AL_BANDPASS_GAIN                         0x0001
#define AL_BANDPASS_GAINLF                       0x0002
#define AL_BANDPASS_GAINHF                       0x0003
#define AL_FILTER_FIRST_PARAMETER                0x0000
#define AL_FILTER_LAST_PARAMETER                 0x8000
#define AL_FILTER_TYPE                           0x8001
#define AL_FILTER_NULL                           0x0000
#define AL_FILTER_LOWPASS                        0x0001
#define AL_FILTER_HIGHPASS                       0x0002
#define AL_FILTER_BANDPASS                       0x0003

#define AL_LOWPASS_MIN_GAIN                      (0.0f)
#define AL_LOWPASS_MAX_GAIN                      (1.0f)
#define AL_LOWPASS_DEFAULT_GAIN                  (1.0f)
#define AL_LOWPASS_MIN_GAINHF                    (0.0f)
#define AL_LOWPASS_MAX_GAINHF                    (1.0f)
#define AL_LOWPASS_DEFAULT_GAINHF                (1.0f)
#define AL_HIGHPASS_MIN_GAIN                     (0.0f)
#define AL_HIGHPASS_MAX_GAIN                     (1.0f)
#define AL_HIGHPASS_DEFAULT_GAIN                 (1.0f)
#define AL_HIGHPASS_MIN_GAINLF                   (0.0f)
#define AL_HIGHPASS_MAX_GAINLF                   (1.0f)
#define AL_HIGHPASS_DEFAULT_GAINLF               (1.0f)
#define AL_BANDPASS_MIN_GAIN                     (0.0f)
#define AL_BANDPASS_MAX_GAIN                     (1.0f)
#define AL_BANDPASS_DEFAULT_GAIN                 (1.0f)
#define AL_BANDPASS_MIN_GAINHF                   (0.0f)
#define AL_BANDPASS_MAX_GAINHF                   (1.0f)
#define AL_BANDPASS_DEFAULT_GAINHF               (1.0f)
#define AL_BANDPASS_MIN_GAINLF                   (0.0f)
#define AL_BANDPASS_MAX_GAINLF                   (1.0f)
#define AL_BANDPASS_DEFAULT_GAINLF               (1.0f)
#define AL_REVERB_MIN_DENSITY                    (0.0f)
#define AL_REVERB_MAX_DENSITY                    (1.0f)
#define AL_REVERB_DEFAULT_DENSITY                (1.0f)
#define AL_REVERB_MIN_DIFFUSION                  (0.0f)
#define AL_REVERB_MAX_DIFFUSION                  (1.0f)
#define AL_REVERB_DEFAULT_DIFFUSION              (1.0f)
#define AL_REVERB_MIN_GAIN                       (0.0f)
#define AL_REVERB_MAX_GAIN                       (1.0f)
#define AL_REVERB_DEFAULT_GAIN                   (0.32f)
#define AL_REVERB_MIN_GAINHF                     (0.0f)
#define AL_REVERB_MAX_GAINHF                     (1.0f)
#define AL_REVERB_DEFAULT_GAINHF                 (0.89f)
#define AL_REVERB_MIN_DECAY_TIME                 (0.1f)
#define AL_REVERB_MAX_DECAY_TIME                 (20.0f)
#define AL_REVERB_DEFAULT_DECAY_TIME             (1.49f)
#define AL_REVERB_MIN_DECAY_HFRATIO              (0.1f)
#define AL_REVERB_MAX_DECAY_HFRATIO              (2.0f)
#define AL_REVERB_DEFAULT_DECAY_HFRATIO          (0.83f)
#define AL_REVERB_MIN_REFLECTIONS_GAIN           (0.0f)
#define AL_REVERB_MAX_REFLECTIONS_GAIN           (3.16f)
#define AL_REVERB_DEFAULT_REFLECTIONS_GAIN       (0.05f)
#define AL_REVERB_MIN_REFLECTIONS_DELAY          (0.0f)
#define AL_REVERB_MAX_REFLECTIONS_DELAY          (0.3f)
#define AL_REVERB_DEFAULT_REFLECTIONS_DELAY      (0.007f)
#define AL_REVERB_MIN_LATE_REVERB_GAIN           (0.0f)
#define AL_REVERB_MAX_LATE_REVERB_GAIN           (10.0f)
#define AL_REVERB_DEFAULT_LATE_REVERB_GAIN       (1.26f)
#define AL_REVERB_MIN_LATE_REVERB_DELAY          (0.0f)
#define AL_REVERB_MAX_LATE_REVERB_DELAY          (0.1f)
#define AL_REVERB_DEFAULT_LATE_REVERB_DELAY      (0.011f)
#define AL_REVERB_MIN_AIR_ABSORPTION_GAINHF      (0.892f)
#define AL_REVERB_MAX_AIR_ABSORPTION_GAINHF      (1.0f)
#define AL_REVERB_DEFAULT_AIR_ABSORPTION_GAINHF  (0.994f)
#define AL_REVERB_MIN_ROOM_ROLLOFF_FACTOR        (0.0f)
#define AL_REVERB_MAX_ROOM_ROLLOFF_FACTOR        (10.0f)
#define AL_REVERB_DEFAULT_ROOM_ROLLOFF_FACTOR    (0.0f)
#define AL_REVERB_MIN_DECAY_HFLIMIT              AL_FALSE
#define AL_REVERB_MAX_DECAY_HFLIMIT              AL_TRUE
#define AL_REVERB_DEFAULT_DECAY_HFLIMIT          AL_TRUE
#define AL_EAXREVERB_MIN_DENSITY                 (0.0f)
#define AL_EAXREVERB_MAX_DENSITY                 (1.0f)
#define AL_EAXREVERB_DEFAULT_DENSITY             (1.0f)
#define AL_EAXREVERB_MIN_DIFFUSION               (0.0f)
#define AL_EAXREVERB_MAX_DIFFUSION               (1.0f)
#define AL_EAXREVERB_DEFAULT_DIFFUSION           (1.0f)
#define AL_EAXREVERB_MIN_GAIN                    (0.0f)
#define AL_EAXREVERB_MAX_GAIN                    (1.0f)
#define AL_EAXREVERB_DEFAULT_GAIN                (0.32f)
#define AL_EAXREVERB_MIN_GAINHF                  (0.0f)
#define AL_EAXREVERB_MAX_GAINHF                  (1.0f)
#define AL_EAXREVERB_DEFAULT_GAINHF              (0.89f)
#define AL_EAXREVERB_MIN_GAINLF                  (0.0f)
#define AL_EAXREVERB_MAX_GAINLF                  (1.0f)
#define AL_EAXREVERB_DEFAULT_GAINLF              (1.0f)
#define AL_EAXREVERB_MIN_DECAY_TIME              (0.1f)
#define AL_EAXREVERB_MAX_DECAY_TIME              (20.0f)
#define AL_EAXREVERB_DEFAULT_DECAY_TIME          (1.49f)
#define AL_EAXREVERB_MIN_DECAY_HFRATIO           (0.1f)
#define AL_EAXREVERB_MAX_DECAY_HFRATIO           (2.0f)
#define AL_EAXREVERB_DEFAULT_DECAY_HFRATIO       (0.83f)
#define AL_EAXREVERB_MIN_DECAY_LFRATIO           (0.1f)
#define AL_EAXREVERB_MAX_DECAY_LFRATIO           (2.0f)
#define AL_EAXREVERB_DEFAULT_DECAY_LFRATIO       (1.0f)
#define AL_EAXREVERB_MIN_REFLECTIONS_GAIN        (0.0f)
#define AL_EAXREVERB_MAX_REFLECTIONS_GAIN        (3.16f)
#define AL_EAXREVERB_DEFAULT_REFLECTIONS_GAIN    (0.05f)
#define AL_EAXREVERB_MIN_REFLECTIONS_DELAY       (0.0f)
#define AL_EAXREVERB_MAX_REFLECTIONS_DELAY       (0.3f)
#define AL_EAXREVERB_DEFAULT_REFLECTIONS_DELAY   (0.007f)
#define AL_EAXREVERB_DEFAULT_REFLECTIONS_PAN_XYZ (0.0f)
#define AL_EAXREVERB_MIN_LATE_REVERB_GAIN        (0.0f)
#define AL_EAXREVERB_MAX_LATE_REVERB_GAIN        (10.0f)
#define AL_EAXREVERB_DEFAULT_LATE_REVERB_GAIN    (1.26f)
#define AL_EAXREVERB_MIN_LATE_REVERB_DELAY       (0.0f)
#define AL_EAXREVERB_MAX_LATE_REVERB_DELAY       (0.1f)
#define AL_EAXREVERB_DEFAULT_LATE_REVERB_DELAY   (0.011f)
#define AL_EAXREVERB_DEFAULT_LATE_REVERB_PAN_XYZ (0.0f)
#define AL_EAXREVERB_MIN_ECHO_TIME               (0.075f)
#define AL_EAXREVERB_MAX_ECHO_TIME               (0.25f)
#define AL_EAXREVERB_DEFAULT_ECHO_TIME           (0.25f)
#define AL_EAXREVERB_MIN_ECHO_DEPTH              (0.0f)
#define AL_EAXREVERB_MAX_ECHO_DEPTH              (1.0f)
#define AL_EAXREVERB_DEFAULT_ECHO_DEPTH          (0.0f)
#define AL_EAXREVERB_MIN_MODULATION_TIME         (0.04f)
#define AL_EAXREVERB_MAX_MODULATION_TIME         (4.0f)
#define AL_EAXREVERB_DEFAULT_MODULATION_TIME     (0.25f)
#define AL_EAXREVERB_MIN_MODULATION_DEPTH        (0.0f)
#define AL_EAXREVERB_MAX_MODULATION_DEPTH        (1.0f)
#define AL_EAXREVERB_DEFAULT_MODULATION_DEPTH    (0.0f)
#define AL_EAXREVERB_MIN_AIR_ABSORPTION_GAINHF   (0.892f)
#define AL_EAXREVERB_MAX_AIR_ABSORPTION_GAINHF   (1.0f)
#define AL_EAXREVERB_DEFAULT_AIR_ABSORPTION_GAINHF (0.994f)
#define AL_EAXREVERB_MIN_HFREFERENCE             (1000.0f)
#define AL_EAXREVERB_MAX_HFREFERENCE             (20000.0f)
#define AL_EAXREVERB_DEFAULT_HFREFERENCE         (5000.0f)
#define AL_EAXREVERB_MIN_LFREFERENCE             (20.0f)
#define AL_EAXREVERB_MAX_LFREFERENCE             (1000.0f)
#define AL_EAXREVERB_DEFAULT_LFREFERENCE         (250.0f)
#define AL_EAXREVERB_MIN_ROOM_ROLLOFF_FACTOR     (0.0f)
#define AL_EAXREVERB_MAX_ROOM_ROLLOFF_FACTOR     (10.0f)
#define AL_EAXREVERB_DEFAULT_ROOM_ROLLOFF_FACTOR (0.0f)
#define AL_EAXREVERB_MIN_DECAY_HFLIMIT           AL_FALSE
#define AL_EAXREVERB_MAX_DECAY_HFLIMIT           AL_TRUE
#define AL_EAXREVERB_DEFAULT_DECAY_HFLIMIT       AL_TRUE
#define AL_CHORUS_WAVEFORM_SINUSOID              (0)
#define AL_CHORUS_WAVEFORM_TRIANGLE              (1)
#define AL_CHORUS_MIN_WAVEFORM                   (0)
#define AL_CHORUS_MAX_WAVEFORM                   (1)
#define AL_CHORUS_DEFAULT_WAVEFORM               (1)
#define AL_CHORUS_MIN_PHASE                      (-180)
#define AL_CHORUS_MAX_PHASE                      (180)
#define AL_CHORUS_DEFAULT_PHASE                  (90)
#define AL_CHORUS_MIN_RATE                       (0.0f)
#define AL_CHORUS_MAX_RATE                       (10.0f)
#define AL_CHORUS_DEFAULT_RATE                   (1.1f)
#define AL_CHORUS_MIN_DEPTH                      (0.0f)
#define AL_CHORUS_MAX_DEPTH                      (1.0f)
#define AL_CHORUS_DEFAULT_DEPTH                  (0.1f)
#define AL_CHORUS_MIN_FEEDBACK                   (-1.0f)
#define AL_CHORUS_MAX_FEEDBACK                   (1.0f)
#define AL_CHORUS_DEFAULT_FEEDBACK               (0.25f)
#define AL_CHORUS_MIN_DELAY                      (0.0f)
#define AL_CHORUS_MAX_DELAY                      (0.016f)
#define AL_CHORUS_DEFAULT_DELAY                  (0.016f)
#define AL_DISTORTION_MIN_EDGE                   (0.0f)
#define AL_DISTORTION_MAX_EDGE                   (1.0f)
#define AL_DISTORTION_DEFAULT_EDGE               (0.2f)
#define AL_DISTORTION_MIN_GAIN                   (0.01f)
#define AL_DISTORTION_MAX_GAIN                   (1.0f)
#define AL_DISTORTION_DEFAULT_GAIN               (0.05f)
#define AL_DISTORTION_MIN_LOWPASS_CUTOFF         (80.0f)
#define AL_DISTORTION_MAX_LOWPASS_CUTOFF         (24000.0f)
#define AL_DISTORTION_DEFAULT_LOWPASS_CUTOFF     (8000.0f)
#define AL_DISTORTION_MIN_EQCENTER               (80.0f)
#define AL_DISTORTION_MAX_EQCENTER               (24000.0f)
#define AL_DISTORTION_DEFAULT_EQCENTER           (3600.0f)
#define AL_DISTORTION_MIN_EQBANDWIDTH            (80.0f)
#define AL_DISTORTION_MAX_EQBANDWIDTH            (24000.0f)
#define AL_DISTORTION_DEFAULT_EQBANDWIDTH        (3600.0f)
#define AL_ECHO_MIN_DELAY                        (0.0f)
#define AL_ECHO_MAX_DELAY                        (0.207f)
#define AL_ECHO_DEFAULT_DELAY                    (0.1f)
#define AL_ECHO_MIN_LRDELAY                      (0.0f)
#define AL_ECHO_MAX_LRDELAY                      (0.404f)
#define AL_ECHO_DEFAULT_LRDELAY                  (0.1f)
#define AL_ECHO_MIN_DAMPING                      (0.0f)
#define AL_ECHO_MAX_DAMPING                      (0.99f)
#define AL_ECHO_DEFAULT_DAMPING                  (0.5f)
#define AL_ECHO_MIN_FEEDBACK                     (0.0f)
#define AL_ECHO_MAX_FEEDBACK                     (1.0f)
#define AL_ECHO_DEFAULT_FEEDBACK                 (0.5f)
#define AL_ECHO_MIN_SPREAD                       (-1.0f)
#define AL_ECHO_MAX_SPREAD                       (1.0f)
#define AL_ECHO_DEFAULT_SPREAD                   (-1.0f)
#define AL_FLANGER_WAVEFORM_SINUSOID             (0)
#define AL_FLANGER_WAVEFORM_TRIANGLE             (1)
#define AL_FLANGER_MIN_WAVEFORM                  (0)
#define AL_FLANGER_MAX_WAVEFORM                  (1)
#define AL_FLANGER_DEFAULT_WAVEFORM              (1)
#define AL_FLANGER_MIN_PHASE                     (-180)
#define AL_FLANGER_MAX_PHASE                     (180)
#define AL_FLANGER_DEFAULT_PHASE                 (0)
#define AL_FLANGER_MIN_RATE                      (0.0f)
#define AL_FLANGER_MAX_RATE                      (10.0f)
#define AL_FLANGER_DEFAULT_RATE                  (0.27f)
#define AL_FLANGER_MIN_DEPTH                     (0.0f)
#define AL_FLANGER_MAX_DEPTH                     (1.0f)
#define AL_FLANGER_DEFAULT_DEPTH                 (1.0f)
#define AL_FLANGER_MIN_FEEDBACK                  (-1.0f)
#define AL_FLANGER_MAX_FEEDBACK                  (1.0f)
#define AL_FLANGER_DEFAULT_FEEDBACK              (-0.5f)
#define AL_FLANGER_MIN_DELAY                     (0.0f)
#define AL_FLANGER_MAX_DELAY                     (0.004f)
#define AL_FLANGER_DEFAULT_DELAY                 (0.002f)
#define AL_FREQUENCY_SHIFTER_MIN_FREQUENCY       (0.0f)
#define AL_FREQUENCY_SHIFTER_MAX_FREQUENCY       (24000.0f)
#define AL_FREQUENCY_SHIFTER_DEFAULT_FREQUENCY   (0.0f)
#define AL_FREQUENCY_SHIFTER_MIN_LEFT_DIRECTION  (0)
#define AL_FREQUENCY_SHIFTER_MAX_LEFT_DIRECTION  (2)
#define AL_FREQUENCY_SHIFTER_DEFAULT_LEFT_DIRECTION (0)
#define AL_FREQUENCY_SHIFTER_DIRECTION_DOWN      (0)
#define AL_FREQUENCY_SHIFTER_DIRECTION_UP        (1)
#define AL_FREQUENCY_SHIFTER_DIRECTION_OFF       (2)
#define AL_FREQUENCY_SHIFTER_MIN_RIGHT_DIRECTION (0)
#define AL_FREQUENCY_SHIFTER_MAX_RIGHT_DIRECTION (2)
#define AL_FREQUENCY_SHIFTER_DEFAULT_RIGHT_DIRECTION (0)
#define AL_VOCAL_MORPHER_MIN_PHONEMEA            (0)
#define AL_VOCAL_MORPHER_MAX_PHONEMEA            (29)
#define AL_VOCAL_MORPHER_DEFAULT_PHONEMEA        (0)
#define AL_VOCAL_MORPHER_MIN_PHONEMEA_COARSE_TUNING (-24)
#define AL_VOCAL_MORPHER_MAX_PHONEMEA_COARSE_TUNING (24)
#define AL_VOCAL_MORPHER_DEFAULT_PHONEMEA_COARSE_TUNING (0)
#define AL_VOCAL_MORPHER_MIN_PHONEMEB            (0)
#define AL_VOCAL_MORPHER_MAX_PHONEMEB            (29)
#define AL_VOCAL_MORPHER_DEFAULT_PHONEMEB        (10)
#define AL_VOCAL_MORPHER_MIN_PHONEMEB_COARSE_TUNING (-24)
#define AL_VOCAL_MORPHER_MAX_PHONEMEB_COARSE_TUNING (24)
#define AL_VOCAL_MORPHER_DEFAULT_PHONEMEB_COARSE_TUNING (0)
#define AL_VOCAL_MORPHER_PHONEME_A               (0)
#define AL_VOCAL_MORPHER_PHONEME_E               (1)
#define AL_VOCAL_MORPHER_PHONEME_I               (2)
#define AL_VOCAL_MORPHER_PHONEME_O               (3)
#define AL_VOCAL_MORPHER_PHONEME_U               (4)
#define AL_VOCAL_MORPHER_PHONEME_AA              (5)
#define AL_VOCAL_MORPHER_PHONEME_AE              (6)
#define AL_VOCAL_MORPHER_PHONEME_AH              (7)
#define AL_VOCAL_MORPHER_PHONEME_AO              (8)
#define AL_VOCAL_MORPHER_PHONEME_EH              (9)
#define AL_VOCAL_MORPHER_PHONEME_ER              (10)
#define AL_VOCAL_MORPHER_PHONEME_IH              (11)
#define AL_VOCAL_MORPHER_PHONEME_IY              (12)
#define AL_VOCAL_MORPHER_PHONEME_UH              (13)
#define AL_VOCAL_MORPHER_PHONEME_UW              (14)
#define AL_VOCAL_MORPHER_PHONEME_B               (15)
#define AL_VOCAL_MORPHER_PHONEME_D               (16)
#define AL_VOCAL_MORPHER_PHONEME_F               (17)
#define AL_VOCAL_MORPHER_PHONEME_G               (18)
#define AL_VOCAL_MORPHER_PHONEME_J               (19)
#define AL_VOCAL_MORPHER_PHONEME_K               (20)
#define AL_VOCAL_MORPHER_PHONEME_L               (21)
#define AL_VOCAL_MORPHER_PHONEME_M               (22)
#define AL_VOCAL_MORPHER_PHONEME_N               (23)
#define AL_VOCAL_MORPHER_PHONEME_P               (24)
#define AL_VOCAL_MORPHER_PHONEME_R               (25)
#define AL_VOCAL_MORPHER_PHONEME_S               (26)
#define AL_VOCAL_MORPHER_PHONEME_T               (27)
#define AL_VOCAL_MORPHER_PHONEME_V               (28)
#define AL_VOCAL_MORPHER_PHONEME_Z               (29)
#define AL_VOCAL_MORPHER_WAVEFORM_SINUSOID       (0)
#define AL_VOCAL_MORPHER_WAVEFORM_TRIANGLE       (1)
#define AL_VOCAL_MORPHER_WAVEFORM_SAWTOOTH       (2)
#define AL_VOCAL_MORPHER_MIN_WAVEFORM            (0)
#define AL_VOCAL_MORPHER_MAX_WAVEFORM            (2)
#define AL_VOCAL_MORPHER_DEFAULT_WAVEFORM        (0)
#define AL_VOCAL_MORPHER_MIN_RATE                (0.0f)
#define AL_VOCAL_MORPHER_MAX_RATE                (10.0f)
#define AL_VOCAL_MORPHER_DEFAULT_RATE            (1.41f)
#define AL_PITCH_SHIFTER_MIN_COARSE_TUNE         (-12)
#define AL_PITCH_SHIFTER_MAX_COARSE_TUNE         (12)
#define AL_PITCH_SHIFTER_DEFAULT_COARSE_TUNE     (12)
#define AL_PITCH_SHIFTER_MIN_FINE_TUNE           (-50)
#define AL_PITCH_SHIFTER_MAX_FINE_TUNE           (50)
#define AL_PITCH_SHIFTER_DEFAULT_FINE_TUNE       (0)
#define AL_RING_MODULATOR_MIN_FREQUENCY          (0.0f)
#define AL_RING_MODULATOR_MAX_FREQUENCY          (8000.0f)
#define AL_RING_MODULATOR_DEFAULT_FREQUENCY      (440.0f)
#define AL_RING_MODULATOR_MIN_HIGHPASS_CUTOFF    (0.0f)
#define AL_RING_MODULATOR_MAX_HIGHPASS_CUTOFF    (24000.0f)
#define AL_RING_MODULATOR_DEFAULT_HIGHPASS_CUTOFF (800.0f)
#define AL_RING_MODULATOR_SINUSOID               (0)
#define AL_RING_MODULATOR_SAWTOOTH               (1)
#define AL_RING_MODULATOR_SQUARE                 (2)
#define AL_RING_MODULATOR_MIN_WAVEFORM           (0)
#define AL_RING_MODULATOR_MAX_WAVEFORM           (2)
#define AL_RING_MODULATOR_DEFAULT_WAVEFORM       (0)
#define AL_AUTOWAH_MIN_ATTACK_TIME               (0.0001f)
#define AL_AUTOWAH_MAX_ATTACK_TIME               (1.0f)
#define AL_AUTOWAH_DEFAULT_ATTACK_TIME           (0.06f)
#define AL_AUTOWAH_MIN_RELEASE_TIME              (0.0001f)
#define AL_AUTOWAH_MAX_RELEASE_TIME              (1.0f)
#define AL_AUTOWAH_DEFAULT_RELEASE_TIME          (0.06f)
#define AL_AUTOWAH_MIN_RESONANCE                 (2.0f)
#define AL_AUTOWAH_MAX_RESONANCE                 (1000.0f)
#define AL_AUTOWAH_DEFAULT_RESONANCE             (1000.0f)
#define AL_AUTOWAH_MIN_PEAK_GAIN                 (0.00003f)
#define AL_AUTOWAH_MAX_PEAK_GAIN                 (31621.0f)
#define AL_AUTOWAH_DEFAULT_PEAK_GAIN             (11.22f)
#define AL_COMPRESSOR_MIN_ONOFF                  (0)
#define AL_COMPRESSOR_MAX_ONOFF                  (1)
#define AL_COMPRESSOR_DEFAULT_ONOFF              (1)
#define AL_EQUALIZER_MIN_LOW_GAIN                (0.126f)
#define AL_EQUALIZER_MAX_LOW_GAIN                (7.943f)
#define AL_EQUALIZER_DEFAULT_LOW_GAIN            (1.0f)
#define AL_EQUALIZER_MIN_LOW_CUTOFF              (50.0f)
#define AL_EQUALIZER_MAX_LOW_CUTOFF              (800.0f)
#define AL_EQUALIZER_DEFAULT_LOW_CUTOFF          (200.0f)
#define AL_EQUALIZER_MIN_MID1_GAIN               (0.126f)
#define AL_EQUALIZER_MAX_MID1_GAIN               (7.943f)
#define AL_EQUALIZER_DEFAULT_MID1_GAIN           (1.0f)
#define AL_EQUALIZER_MIN_MID1_CENTER             (200.0f)
#define AL_EQUALIZER_MAX_MID1_CENTER             (3000.0f)
#define AL_EQUALIZER_DEFAULT_MID1_CENTER         (500.0f)
#define AL_EQUALIZER_MIN_MID1_WIDTH              (0.01f)
#define AL_EQUALIZER_MAX_MID1_WIDTH              (1.0f)
#define AL_EQUALIZER_DEFAULT_MID1_WIDTH          (1.0f)
#define AL_EQUALIZER_MIN_MID2_GAIN               (0.126f)
#define AL_EQUALIZER_MAX_MID2_GAIN               (7.943f)
#define AL_EQUALIZER_DEFAULT_MID2_GAIN           (1.0f)
#define AL_EQUALIZER_MIN_MID2_CENTER             (1000.0f)
#define AL_EQUALIZER_MAX_MID2_CENTER             (8000.0f)
#define AL_EQUALIZER_DEFAULT_MID2_CENTER         (3000.0f)
#define AL_EQUALIZER_MIN_MID2_WIDTH              (0.01f)
#define AL_EQUALIZER_MAX_MID2_WIDTH              (1.0f)
#define AL_EQUALIZER_DEFAULT_MID2_WIDTH          (1.0f)
#define AL_EQUALIZER_MIN_HIGH_GAIN               (0.126f)
#define AL_EQUALIZER_MAX_HIGH_GAIN               (7.943f)
#define AL_EQUALIZER_DEFAULT_HIGH_GAIN           (1.0f)
#define AL_EQUALIZER_MIN_HIGH_CUTOFF             (4000.0f)
#define AL_EQUALIZER_MAX_HIGH_CUTOFF             (16000.0f)
#define AL_EQUALIZER_DEFAULT_HIGH_CUTOFF         (6000.0f)
#define AL_MIN_AIR_ABSORPTION_FACTOR             (0.0f)
#define AL_MAX_AIR_ABSORPTION_FACTOR             (10.0f)
#define AL_DEFAULT_AIR_ABSORPTION_FACTOR         (0.0f)
#define AL_MIN_ROOM_ROLLOFF_FACTOR               (0.0f)
#define AL_MAX_ROOM_ROLLOFF_FACTOR               (10.0f)
#define AL_DEFAULT_ROOM_ROLLOFF_FACTOR           (0.0f)
#define AL_MIN_CONE_OUTER_GAINHF                 (0.0f)
#define AL_MAX_CONE_OUTER_GAINHF                 (1.0f)
#define AL_DEFAULT_CONE_OUTER_GAINHF             (1.0f)
#define AL_MIN_DIRECT_FILTER_GAINHF_AUTO         AL_FALSE
#define AL_MAX_DIRECT_FILTER_GAINHF_AUTO         AL_TRUE
#define AL_DEFAULT_DIRECT_FILTER_GAINHF_AUTO     AL_TRUE
#define AL_MIN_AUXILIARY_SEND_FILTER_GAIN_AUTO   AL_FALSE
#define AL_MAX_AUXILIARY_SEND_FILTER_GAIN_AUTO   AL_TRUE
#define AL_DEFAULT_AUXILIARY_SEND_FILTER_GAIN_AUTO AL_TRUE
#define AL_MIN_AUXILIARY_SEND_FILTER_GAINHF_AUTO AL_FALSE
#define AL_MAX_AUXILIARY_SEND_FILTER_GAINHF_AUTO AL_TRUE
#define AL_DEFAULT_AUXILIARY_SEND_FILTER_GAINHF_AUTO AL_TRUE
#define AL_MIN_METERS_PER_UNIT                   FLT_MIN
#define AL_MAX_METERS_PER_UNIT                   FLT_MAX
#define AL_DEFAULT_METERS_PER_UNIT               (1.0f)

//TODO format these better (they look really bad in the github preview, although passable in a local editor)
#define EFX_REVERB_PRESET_GENERIC \
    { 1.0000f, 1.0000f, 0.3162f, 0.8913f, 1.0000f, 1.4900f, 0.8300f, 1.0000f, 0.0500f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, \
    1.2589f, 0.0110f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_PADDEDCELL \
    { 0.1715f, 1.0000f, 0.3162f, 0.0010f, 1.0000f, 0.1700f, 0.1000f, 1.0000f, 0.2500f, 0.0010f, { 0.0000f, 0.0000f, 0.0000f }, \
    1.2691f, 0.0020f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_ROOM \
    { 0.4287f, 1.0000f, 0.3162f, 0.5929f, 1.0000f, 0.4000f, 0.8300f, 1.0000f, 0.1503f, 0.0020f, { 0.0000f, 0.0000f, 0.0000f }, \
    1.0629f, 0.0030f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_BATHROOM \
    { 0.1715f, 1.0000f, 0.3162f, 0.2512f, 1.0000f, 1.4900f, 0.5400f, 1.0000f, 0.6531f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, 3.2734f, 0.0110f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_LIVINGROOM { 0.9766f, 1.0000f, 0.3162f, 0.0010f, 1.0000f, 0.5000f, 0.1000f, 1.0000f, 0.2051f, 0.0030f, { 0.0000f, 0.0000f, 0.0000f }, 0.2805f, 0.0040f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_STONEROOM { 1.0000f, 1.0000f, 0.3162f, 0.7079f, 1.0000f, 2.3100f, 0.6400f, 1.0000f, 0.4411f, 0.0120f, { 0.0000f, 0.0000f, 0.0000f }, 1.1003f, 0.0170f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_AUDITORIUM { 1.0000f, 1.0000f, 0.3162f, 0.5781f, 1.0000f, 4.3200f, 0.5900f, 1.0000f, 0.4032f, 0.0200f, { 0.0000f, 0.0000f, 0.0000f }, 0.7170f, 0.0300f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CONCERTHALL { 1.0000f, 1.0000f, 0.3162f, 0.5623f, 1.0000f, 3.9200f, 0.7000f, 1.0000f, 0.2427f, 0.0200f, { 0.0000f, 0.0000f, 0.0000f }, 0.9977f, 0.0290f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CAVE { 1.0000f, 1.0000f, 0.3162f, 1.0000f, 1.0000f, 2.9100f, 1.3000f, 1.0000f, 0.5000f, 0.0150f, { 0.0000f, 0.0000f, 0.0000f }, 0.7063f, 0.0220f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_ARENA { 1.0000f, 1.0000f, 0.3162f, 0.4477f, 1.0000f, 7.2400f, 0.3300f, 1.0000f, 0.2612f, 0.0200f, { 0.0000f, 0.0000f, 0.0000f }, 1.0186f, 0.0300f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_HANGAR { 1.0000f, 1.0000f, 0.3162f, 0.3162f, 1.0000f, 10.0500f, 0.2300f, 1.0000f, 0.5000f, 0.0200f, { 0.0000f, 0.0000f, 0.0000f }, 1.2560f, 0.0300f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CARPETEDHALLWAY { 0.4287f, 1.0000f, 0.3162f, 0.0100f, 1.0000f, 0.3000f, 0.1000f, 1.0000f, 0.1215f, 0.0020f, { 0.0000f, 0.0000f, 0.0000f }, 0.1531f, 0.0300f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_HALLWAY { 0.3645f, 1.0000f, 0.3162f, 0.7079f, 1.0000f, 1.4900f, 0.5900f, 1.0000f, 0.2458f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, 1.6615f, 0.0110f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_STONECORRIDOR { 1.0000f, 1.0000f, 0.3162f, 0.7612f, 1.0000f, 2.7000f, 0.7900f, 1.0000f, 0.2472f, 0.0130f, { 0.0000f, 0.0000f, 0.0000f }, 1.5758f, 0.0200f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_ALLEY { 1.0000f, 0.3000f, 0.3162f, 0.7328f, 1.0000f, 1.4900f, 0.8600f, 1.0000f, 0.2500f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, 0.9954f, 0.0110f, { 0.0000f, 0.0000f, 0.0000f }, 0.1250f, 0.9500f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_FOREST { 1.0000f, 0.3000f, 0.3162f, 0.0224f, 1.0000f, 1.4900f, 0.5400f, 1.0000f, 0.0525f, 0.1620f, { 0.0000f, 0.0000f, 0.0000f }, 0.7682f, 0.0880f, { 0.0000f, 0.0000f, 0.0000f }, 0.1250f, 1.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CITY { 1.0000f, 0.5000f, 0.3162f, 0.3981f, 1.0000f, 1.4900f, 0.6700f, 1.0000f, 0.0730f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, 0.1427f, 0.0110f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_MOUNTAINS { 1.0000f, 0.2700f, 0.3162f, 0.0562f, 1.0000f, 1.4900f, 0.2100f, 1.0000f, 0.0407f, 0.3000f, { 0.0000f, 0.0000f, 0.0000f }, 0.1919f, 0.1000f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 1.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_QUARRY { 1.0000f, 1.0000f, 0.3162f, 0.3162f, 1.0000f, 1.4900f, 0.8300f, 1.0000f, 0.0000f, 0.0610f, { 0.0000f, 0.0000f, 0.0000f }, 1.7783f, 0.0250f, { 0.0000f, 0.0000f, 0.0000f }, 0.1250f, 0.7000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_PLAIN { 1.0000f, 0.2100f, 0.3162f, 0.1000f, 1.0000f, 1.4900f, 0.5000f, 1.0000f, 0.0585f, 0.1790f, { 0.0000f, 0.0000f, 0.0000f }, 0.1089f, 0.1000f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 1.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_PARKINGLOT { 1.0000f, 1.0000f, 0.3162f, 1.0000f, 1.0000f, 1.6500f, 1.5000f, 1.0000f, 0.2082f, 0.0080f, { 0.0000f, 0.0000f, 0.0000f }, 0.2652f, 0.0120f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_SEWERPIPE { 0.3071f, 0.8000f, 0.3162f, 0.3162f, 1.0000f, 2.8100f, 0.1400f, 1.0000f, 1.6387f, 0.0140f, { 0.0000f, 0.0000f, 0.0000f }, 3.2471f, 0.0210f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_UNDERWATER { 0.3645f, 1.0000f, 0.3162f, 0.0100f, 1.0000f, 1.4900f, 0.1000f, 1.0000f, 0.5963f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, 7.0795f, 0.0110f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 1.1800f, 0.3480f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_DRUGGED { 0.4287f, 0.5000f, 0.3162f, 1.0000f, 1.0000f, 8.3900f, 1.3900f, 1.0000f, 0.8760f, 0.0020f, { 0.0000f, 0.0000f, 0.0000f }, 3.1081f, 0.0300f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 1.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_DIZZY { 0.3645f, 0.6000f, 0.3162f, 0.6310f, 1.0000f, 17.2300f, 0.5600f, 1.0000f, 0.1392f, 0.0200f, { 0.0000f, 0.0000f, 0.0000f }, 0.4937f, 0.0300f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 1.0000f, 0.8100f, 0.3100f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_PSYCHOTIC { 0.0625f, 0.5000f, 0.3162f, 0.8404f, 1.0000f, 7.5600f, 0.9100f, 1.0000f, 0.4864f, 0.0200f, { 0.0000f, 0.0000f, 0.0000f }, 2.4378f, 0.0300f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 4.0000f, 1.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_CASTLE_SMALLROOM { 1.0000f, 0.8900f, 0.3162f, 0.3981f, 0.1000f, 1.2200f, 0.8300f, 0.3100f, 0.8913f, 0.0220f, { 0.0000f, 0.0000f, 0.0000f }, 1.9953f, 0.0110f, { 0.0000f, 0.0000f, 0.0000f }, 0.1380f, 0.0800f, 0.2500f, 0.0000f, 0.9943f, 5168.6001f, 139.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CASTLE_SHORTPASSAGE { 1.0000f, 0.8900f, 0.3162f, 0.3162f, 0.1000f, 2.3200f, 0.8300f, 0.3100f, 0.8913f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, 1.2589f, 0.0230f, { 0.0000f, 0.0000f, 0.0000f }, 0.1380f, 0.0800f, 0.2500f, 0.0000f, 0.9943f, 5168.6001f, 139.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CASTLE_MEDIUMROOM { 1.0000f, 0.9300f, 0.3162f, 0.2818f, 0.1000f, 2.0400f, 0.8300f, 0.4600f, 0.6310f, 0.0220f, { 0.0000f, 0.0000f, 0.0000f }, 1.5849f, 0.0110f, { 0.0000f, 0.0000f, 0.0000f }, 0.1550f, 0.0300f, 0.2500f, 0.0000f, 0.9943f, 5168.6001f, 139.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CASTLE_LARGEROOM { 1.0000f, 0.8200f, 0.3162f, 0.2818f, 0.1259f, 2.5300f, 0.8300f, 0.5000f, 0.4467f, 0.0340f, { 0.0000f, 0.0000f, 0.0000f }, 1.2589f, 0.0160f, { 0.0000f, 0.0000f, 0.0000f }, 0.1850f, 0.0700f, 0.2500f, 0.0000f, 0.9943f, 5168.6001f, 139.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CASTLE_LONGPASSAGE { 1.0000f, 0.8900f, 0.3162f, 0.3981f, 0.1000f, 3.4200f, 0.8300f, 0.3100f, 0.8913f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, 1.4125f, 0.0230f, { 0.0000f, 0.0000f, 0.0000f }, 0.1380f, 0.0800f, 0.2500f, 0.0000f, 0.9943f, 5168.6001f, 139.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CASTLE_HALL { 1.0000f, 0.8100f, 0.3162f, 0.2818f, 0.1778f, 3.1400f, 0.7900f, 0.6200f, 0.1778f, 0.0560f, { 0.0000f, 0.0000f, 0.0000f }, 1.1220f, 0.0240f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5168.6001f, 139.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CASTLE_CUPBOARD { 1.0000f, 0.8900f, 0.3162f, 0.2818f, 0.1000f, 0.6700f, 0.8700f, 0.3100f, 1.4125f, 0.0100f, { 0.0000f, 0.0000f, 0.0000f }, 3.5481f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, 0.1380f, 0.0800f, 0.2500f, 0.0000f, 0.9943f, 5168.6001f, 139.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CASTLE_COURTYARD { 1.0000f, 0.4200f, 0.3162f, 0.4467f, 0.1995f, 2.1300f, 0.6100f, 0.2300f, 0.2239f, 0.1600f, { 0.0000f, 0.0000f, 0.0000f }, 0.7079f, 0.0360f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.3700f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_CASTLE_ALCOVE { 1.0000f, 0.8900f, 0.3162f, 0.5012f, 0.1000f, 1.6400f, 0.8700f, 0.3100f, 1.0000f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, 1.4125f, 0.0340f, { 0.0000f, 0.0000f, 0.0000f }, 0.1380f, 0.0800f, 0.2500f, 0.0000f, 0.9943f, 5168.6001f, 139.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_FACTORY_SMALLROOM { 0.3645f, 0.8200f, 0.3162f, 0.7943f, 0.5012f, 1.7200f, 0.6500f, 1.3100f, 0.7079f, 0.0100f, { 0.0000f, 0.0000f, 0.0000f }, 1.7783f, 0.0240f, { 0.0000f, 0.0000f, 0.0000f }, 0.1190f, 0.0700f, 0.2500f, 0.0000f, 0.9943f, 3762.6001f, 362.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_FACTORY_SHORTPASSAGE { 0.3645f, 0.6400f, 0.2512f, 0.7943f, 0.5012f, 2.5300f, 0.6500f, 1.3100f, 1.0000f, 0.0100f, { 0.0000f, 0.0000f, 0.0000f }, 1.2589f, 0.0380f, { 0.0000f, 0.0000f, 0.0000f }, 0.1350f, 0.2300f, 0.2500f, 0.0000f, 0.9943f, 3762.6001f, 362.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_FACTORY_MEDIUMROOM { 0.4287f, 0.8200f, 0.2512f, 0.7943f, 0.5012f, 2.7600f, 0.6500f, 1.3100f, 0.2818f, 0.0220f, { 0.0000f, 0.0000f, 0.0000f }, 1.4125f, 0.0230f, { 0.0000f, 0.0000f, 0.0000f }, 0.1740f, 0.0700f, 0.2500f, 0.0000f, 0.9943f, 3762.6001f, 362.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_FACTORY_LARGEROOM { 0.4287f, 0.7500f, 0.2512f, 0.7079f, 0.6310f, 4.2400f, 0.5100f, 1.3100f, 0.1778f, 0.0390f, { 0.0000f, 0.0000f, 0.0000f }, 1.1220f, 0.0230f, { 0.0000f, 0.0000f, 0.0000f }, 0.2310f, 0.0700f, 0.2500f, 0.0000f, 0.9943f, 3762.6001f, 362.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_FACTORY_LONGPASSAGE { 0.3645f, 0.6400f, 0.2512f, 0.7943f, 0.5012f, 4.0600f, 0.6500f, 1.3100f, 1.0000f, 0.0200f, { 0.0000f, 0.0000f, 0.0000f }, 1.2589f, 0.0370f, { 0.0000f, 0.0000f, 0.0000f }, 0.1350f, 0.2300f, 0.2500f, 0.0000f, 0.9943f, 3762.6001f, 362.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_FACTORY_HALL { 0.4287f, 0.7500f, 0.3162f, 0.7079f, 0.6310f, 7.4300f, 0.5100f, 1.3100f, 0.0631f, 0.0730f, { 0.0000f, 0.0000f, 0.0000f }, 0.8913f, 0.0270f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0700f, 0.2500f, 0.0000f, 0.9943f, 3762.6001f, 362.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_FACTORY_CUPBOARD { 0.3071f, 0.6300f, 0.2512f, 0.7943f, 0.5012f, 0.4900f, 0.6500f, 1.3100f, 1.2589f, 0.0100f, { 0.0000f, 0.0000f, 0.0000f }, 1.9953f, 0.0320f, { 0.0000f, 0.0000f, 0.0000f }, 0.1070f, 0.0700f, 0.2500f, 0.0000f, 0.9943f, 3762.6001f, 362.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_FACTORY_COURTYARD { 0.3071f, 0.5700f, 0.3162f, 0.3162f, 0.6310f, 2.3200f, 0.2900f, 0.5600f, 0.2239f, 0.1400f, { 0.0000f, 0.0000f, 0.0000f }, 0.3981f, 0.0390f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.2900f, 0.2500f, 0.0000f, 0.9943f, 3762.6001f, 362.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_FACTORY_ALCOVE { 0.3645f, 0.5900f, 0.2512f, 0.7943f, 0.5012f, 3.1400f, 0.6500f, 1.3100f, 1.4125f, 0.0100f, { 0.0000f, 0.0000f, 0.0000f }, 1.0000f, 0.0380f, { 0.0000f, 0.0000f, 0.0000f }, 0.1140f, 0.1000f, 0.2500f, 0.0000f, 0.9943f, 3762.6001f, 362.5000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_ICEPALACE_SMALLROOM { 1.0000f, 0.8400f, 0.3162f, 0.5623f, 0.2818f, 1.5100f, 1.5300f, 0.2700f, 0.8913f, 0.0100f, { 0.0000f, 0.0000f, 0.0000f }, 1.4125f, 0.0110f, { 0.0000f, 0.0000f, 0.0000f }, 0.1640f, 0.1400f, 0.2500f, 0.0000f, 0.9943f, 12428.5000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_ICEPALACE_SHORTPASSAGE { 1.0000f, 0.7500f, 0.3162f, 0.5623f, 0.2818f, 1.7900f, 1.4600f, 0.2800f, 0.5012f, 0.0100f, { 0.0000f, 0.0000f, 0.0000f }, 1.1220f, 0.0190f, { 0.0000f, 0.0000f, 0.0000f }, 0.1770f, 0.0900f, 0.2500f, 0.0000f, 0.9943f, 12428.5000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_ICEPALACE_MEDIUMROOM { 1.0000f, 0.8700f, 0.3162f, 0.5623f, 0.4467f, 2.2200f, 1.5300f, 0.3200f, 0.3981f, 0.0390f, { 0.0000f, 0.0000f, 0.0000f }, 1.1220f, 0.0270f, { 0.0000f, 0.0000f, 0.0000f }, 0.1860f, 0.1200f, 0.2500f, 0.0000f, 0.9943f, 12428.5000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_ICEPALACE_LARGEROOM { 1.0000f, 0.8100f, 0.3162f, 0.5623f, 0.4467f, 3.1400f, 1.5300f, 0.3200f, 0.2512f, 0.0390f, { 0.0000f, 0.0000f, 0.0000f }, 1.0000f, 0.0270f, { 0.0000f, 0.0000f, 0.0000f }, 0.2140f, 0.1100f, 0.2500f, 0.0000f, 0.9943f, 12428.5000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_ICEPALACE_LONGPASSAGE { 1.0000f, 0.7700f, 0.3162f, 0.5623f, 0.3981f, 3.0100f, 1.4600f, 0.2800f, 0.7943f, 0.0120f, { 0.0000f, 0.0000f, 0.0000f }, 1.2589f, 0.0250f, { 0.0000f, 0.0000f, 0.0000f }, 0.1860f, 0.0400f, 0.2500f, 0.0000f, 0.9943f, 12428.5000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_ICEPALACE_HALL { 1.0000f, 0.7600f, 0.3162f, 0.4467f, 0.5623f, 5.4900f, 1.5300f, 0.3800f, 0.1122f, 0.0540f, { 0.0000f, 0.0000f, 0.0000f }, 0.6310f, 0.0520f, { 0.0000f, 0.0000f, 0.0000f }, 0.2260f, 0.1100f, 0.2500f, 0.0000f, 0.9943f, 12428.5000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_ICEPALACE_CUPBOARD { 1.0000f, 0.8300f, 0.3162f, 0.5012f, 0.2239f, 0.7600f, 1.5300f, 0.2600f, 1.1220f, 0.0120f, { 0.0000f, 0.0000f, 0.0000f }, 1.9953f, 0.0160f, { 0.0000f, 0.0000f, 0.0000f }, 0.1430f, 0.0800f, 0.2500f, 0.0000f, 0.9943f, 12428.5000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_ICEPALACE_COURTYARD { 1.0000f, 0.5900f, 0.3162f, 0.2818f, 0.3162f, 2.0400f, 1.2000f, 0.3800f, 0.3162f, 0.1730f, { 0.0000f, 0.0000f, 0.0000f }, 0.3162f, 0.0430f, { 0.0000f, 0.0000f, 0.0000f }, 0.2350f, 0.4800f, 0.2500f, 0.0000f, 0.9943f, 12428.5000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_ICEPALACE_ALCOVE { 1.0000f, 0.8400f, 0.3162f, 0.5623f, 0.2818f, 2.7600f, 1.4600f, 0.2800f, 1.1220f, 0.0100f, { 0.0000f, 0.0000f, 0.0000f }, 0.8913f, 0.0300f, { 0.0000f, 0.0000f, 0.0000f }, 0.1610f, 0.0900f, 0.2500f, 0.0000f, 0.9943f, 12428.5000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_SPACESTATION_SMALLROOM { 0.2109f, 0.7000f, 0.3162f, 0.7079f, 0.8913f, 1.7200f, 0.8200f, 0.5500f, 0.7943f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, 1.4125f, 0.0130f, { 0.0000f, 0.0000f, 0.0000f }, 0.1880f, 0.2600f, 0.2500f, 0.0000f, 0.9943f, 3316.1001f, 458.2000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_SPACESTATION_SHORTPASSAGE { 0.2109f, 0.8700f, 0.3162f, 0.6310f, 0.8913f, 3.5700f, 0.5000f, 0.5500f, 1.0000f, 0.0120f, { 0.0000f, 0.0000f, 0.0000f }, 1.1220f, 0.0160f, { 0.0000f, 0.0000f, 0.0000f }, 0.1720f, 0.2000f, 0.2500f, 0.0000f, 0.9943f, 3316.1001f, 458.2000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_SPACESTATION_MEDIUMROOM { 0.2109f, 0.7500f, 0.3162f, 0.6310f, 0.8913f, 3.0100f, 0.5000f, 0.5500f, 0.3981f, 0.0340f, { 0.0000f, 0.0000f, 0.0000f }, 1.1220f, 0.0350f, { 0.0000f, 0.0000f, 0.0000f }, 0.2090f, 0.3100f, 0.2500f, 0.0000f, 0.9943f, 3316.1001f, 458.2000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_SPACESTATION_LARGEROOM { 0.3645f, 0.8100f, 0.3162f, 0.6310f, 0.8913f, 3.8900f, 0.3800f, 0.6100f, 0.3162f, 0.0560f, { 0.0000f, 0.0000f, 0.0000f }, 0.8913f, 0.0350f, { 0.0000f, 0.0000f, 0.0000f }, 0.2330f, 0.2800f, 0.2500f, 0.0000f, 0.9943f, 3316.1001f, 458.2000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_SPACESTATION_LONGPASSAGE { 0.4287f, 0.8200f, 0.3162f, 0.6310f, 0.8913f, 4.6200f, 0.6200f, 0.5500f, 1.0000f, 0.0120f, { 0.0000f, 0.0000f, 0.0000f }, 1.2589f, 0.0310f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.2300f, 0.2500f, 0.0000f, 0.9943f, 3316.1001f, 458.2000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_SPACESTATION_HALL { 0.4287f, 0.8700f, 0.3162f, 0.6310f, 0.8913f, 7.1100f, 0.3800f, 0.6100f, 0.1778f, 0.1000f, { 0.0000f, 0.0000f, 0.0000f }, 0.6310f, 0.0470f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.2500f, 0.2500f, 0.0000f, 0.9943f, 3316.1001f, 458.2000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_SPACESTATION_CUPBOARD { 0.1715f, 0.5600f, 0.3162f, 0.7079f, 0.8913f, 0.7900f, 0.8100f, 0.5500f, 1.4125f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, 1.7783f, 0.0180f, { 0.0000f, 0.0000f, 0.0000f }, 0.1810f, 0.3100f, 0.2500f, 0.0000f, 0.9943f, 3316.1001f, 458.2000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_SPACESTATION_ALCOVE { 0.2109f, 0.7800f, 0.3162f, 0.7079f, 0.8913f, 1.1600f, 0.8100f, 0.5500f, 1.4125f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, 1.0000f, 0.0180f, { 0.0000f, 0.0000f, 0.0000f }, 0.1920f, 0.2100f, 0.2500f, 0.0000f, 0.9943f, 3316.1001f, 458.2000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_WOODEN_SMALLROOM { 1.0000f, 1.0000f, 0.3162f, 0.1122f, 0.3162f, 0.7900f, 0.3200f, 0.8700f, 1.0000f, 0.0320f, { 0.0000f, 0.0000f, 0.0000f }, 0.8913f, 0.0290f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 4705.0000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_WOODEN_SHORTPASSAGE { 1.0000f, 1.0000f, 0.3162f, 0.1259f, 0.3162f, 1.7500f, 0.5000f, 0.8700f, 0.8913f, 0.0120f, { 0.0000f, 0.0000f, 0.0000f }, 0.6310f, 0.0240f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 4705.0000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_WOODEN_MEDIUMROOM { 1.0000f, 1.0000f, 0.3162f, 0.1000f, 0.2818f, 1.4700f, 0.4200f, 0.8200f, 0.8913f, 0.0490f, { 0.0000f, 0.0000f, 0.0000f }, 0.8913f, 0.0290f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 4705.0000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_WOODEN_LARGEROOM { 1.0000f, 1.0000f, 0.3162f, 0.0891f, 0.2818f, 2.6500f, 0.3300f, 0.8200f, 0.8913f, 0.0660f, { 0.0000f, 0.0000f, 0.0000f }, 0.7943f, 0.0490f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 4705.0000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_WOODEN_LONGPASSAGE { 1.0000f, 1.0000f, 0.3162f, 0.1000f, 0.3162f, 1.9900f, 0.4000f, 0.7900f, 1.0000f, 0.0200f, { 0.0000f, 0.0000f, 0.0000f }, 0.4467f, 0.0360f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 4705.0000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_WOODEN_HALL { 1.0000f, 1.0000f, 0.3162f, 0.0794f, 0.2818f, 3.4500f, 0.3000f, 0.8200f, 0.8913f, 0.0880f, { 0.0000f, 0.0000f, 0.0000f }, 0.7943f, 0.0630f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 4705.0000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_WOODEN_CUPBOARD { 1.0000f, 1.0000f, 0.3162f, 0.1413f, 0.3162f, 0.5600f, 0.4600f, 0.9100f, 1.1220f, 0.0120f, { 0.0000f, 0.0000f, 0.0000f }, 1.1220f, 0.0280f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 4705.0000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_WOODEN_COURTYARD { 1.0000f, 0.6500f, 0.3162f, 0.0794f, 0.3162f, 1.7900f, 0.3500f, 0.7900f, 0.5623f, 0.1230f, { 0.0000f, 0.0000f, 0.0000f }, 0.1000f, 0.0320f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 4705.0000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_WOODEN_ALCOVE { 1.0000f, 1.0000f, 0.3162f, 0.1259f, 0.3162f, 1.2200f, 0.6200f, 0.9100f, 1.1220f, 0.0120f, { 0.0000f, 0.0000f, 0.0000f }, 0.7079f, 0.0240f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 4705.0000f, 99.6000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_SPORT_EMPTYSTADIUM { 1.0000f, 1.0000f, 0.3162f, 0.4467f, 0.7943f, 6.2600f, 0.5100f, 1.1000f, 0.0631f, 0.1830f, { 0.0000f, 0.0000f, 0.0000f }, 0.3981f, 0.0380f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_SPORT_SQUASHCOURT { 1.0000f, 0.7500f, 0.3162f, 0.3162f, 0.7943f, 2.2200f, 0.9100f, 1.1600f, 0.4467f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, 0.7943f, 0.0110f, { 0.0000f, 0.0000f, 0.0000f }, 0.1260f, 0.1900f, 0.2500f, 0.0000f, 0.9943f, 7176.8999f, 211.2000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_SPORT_SMALLSWIMMINGPOOL { 1.0000f, 0.7000f, 0.3162f, 0.7943f, 0.8913f, 2.7600f, 1.2500f, 1.1400f, 0.6310f, 0.0200f, { 0.0000f, 0.0000f, 0.0000f }, 0.7943f, 0.0300f, { 0.0000f, 0.0000f, 0.0000f }, 0.1790f, 0.1500f, 0.8950f, 0.1900f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_SPORT_LARGESWIMMINGPOOL { 1.0000f, 0.8200f, 0.3162f, 0.7943f, 1.0000f, 5.4900f, 1.3100f, 1.1400f, 0.4467f, 0.0390f, { 0.0000f, 0.0000f, 0.0000f }, 0.5012f, 0.0490f, { 0.0000f, 0.0000f, 0.0000f }, 0.2220f, 0.5500f, 1.1590f, 0.2100f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_SPORT_GYMNASIUM { 1.0000f, 0.8100f, 0.3162f, 0.4467f, 0.8913f, 3.1400f, 1.0600f, 1.3500f, 0.3981f, 0.0290f, { 0.0000f, 0.0000f, 0.0000f }, 0.5623f, 0.0450f, { 0.0000f, 0.0000f, 0.0000f }, 0.1460f, 0.1400f, 0.2500f, 0.0000f, 0.9943f, 7176.8999f, 211.2000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_SPORT_FULLSTADIUM { 1.0000f, 1.0000f, 0.3162f, 0.0708f, 0.7943f, 5.2500f, 0.1700f, 0.8000f, 0.1000f, 0.1880f, { 0.0000f, 0.0000f, 0.0000f }, 0.2818f, 0.0380f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_SPORT_STADIUMTANNOY { 1.0000f, 0.7800f, 0.3162f, 0.5623f, 0.5012f, 2.5300f, 0.8800f, 0.6800f, 0.2818f, 0.2300f, { 0.0000f, 0.0000f, 0.0000f }, 0.5012f, 0.0630f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.2000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_PREFAB_WORKSHOP { 0.4287f, 1.0000f, 0.3162f, 0.1413f, 0.3981f, 0.7600f, 1.0000f, 1.0000f, 1.0000f, 0.0120f, { 0.0000f, 0.0000f, 0.0000f }, 1.1220f, 0.0120f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_PREFAB_SCHOOLROOM { 0.4022f, 0.6900f, 0.3162f, 0.6310f, 0.5012f, 0.9800f, 0.4500f, 0.1800f, 1.4125f, 0.0170f, { 0.0000f, 0.0000f, 0.0000f }, 1.4125f, 0.0150f, { 0.0000f, 0.0000f, 0.0000f }, 0.0950f, 0.1400f, 0.2500f, 0.0000f, 0.9943f, 7176.8999f, 211.2000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_PREFAB_PRACTISEROOM { 0.4022f, 0.8700f, 0.3162f, 0.3981f, 0.5012f, 1.1200f, 0.5600f, 0.1800f, 1.2589f, 0.0100f, { 0.0000f, 0.0000f, 0.0000f }, 1.4125f, 0.0110f, { 0.0000f, 0.0000f, 0.0000f }, 0.0950f, 0.1400f, 0.2500f, 0.0000f, 0.9943f, 7176.8999f, 211.2000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_PREFAB_OUTHOUSE { 1.0000f, 0.8200f, 0.3162f, 0.1122f, 0.1585f, 1.3800f, 0.3800f, 0.3500f, 0.8913f, 0.0240f, { 0.0000f, 0.0000f, -0.0000f }, 0.6310f, 0.0440f, { 0.0000f, 0.0000f, 0.0000f }, 0.1210f, 0.1700f, 0.2500f, 0.0000f, 0.9943f, 2854.3999f, 107.5000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_PREFAB_CARAVAN { 1.0000f, 1.0000f, 0.3162f, 0.0891f, 0.1259f, 0.4300f, 1.5000f, 1.0000f, 1.0000f, 0.0120f, { 0.0000f, 0.0000f, 0.0000f }, 1.9953f, 0.0120f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_DOME_TOMB { 1.0000f, 0.7900f, 0.3162f, 0.3548f, 0.2239f, 4.1800f, 0.2100f, 0.1000f, 0.3868f, 0.0300f, { 0.0000f, 0.0000f, 0.0000f }, 1.6788f, 0.0220f, { 0.0000f, 0.0000f, 0.0000f }, 0.1770f, 0.1900f, 0.2500f, 0.0000f, 0.9943f, 2854.3999f, 20.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_PIPE_SMALL { 1.0000f, 1.0000f, 0.3162f, 0.3548f, 0.2239f, 5.0400f, 0.1000f, 0.1000f, 0.5012f, 0.0320f, { 0.0000f, 0.0000f, 0.0000f }, 2.5119f, 0.0150f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 2854.3999f, 20.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_DOME_SAINTPAULS { 1.0000f, 0.8700f, 0.3162f, 0.3548f, 0.2239f, 10.4800f, 0.1900f, 0.1000f, 0.1778f, 0.0900f, { 0.0000f, 0.0000f, 0.0000f }, 1.2589f, 0.0420f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.1200f, 0.2500f, 0.0000f, 0.9943f, 2854.3999f, 20.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_PIPE_LONGTHIN { 0.2560f, 0.9100f, 0.3162f, 0.4467f, 0.2818f, 9.2100f, 0.1800f, 0.1000f, 0.7079f, 0.0100f, { 0.0000f, 0.0000f, 0.0000f }, 0.7079f, 0.0220f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 2854.3999f, 20.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_PIPE_LARGE { 1.0000f, 1.0000f, 0.3162f, 0.3548f, 0.2239f, 8.4500f, 0.1000f, 0.1000f, 0.3981f, 0.0460f, { 0.0000f, 0.0000f, 0.0000f }, 1.5849f, 0.0320f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 2854.3999f, 20.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_PIPE_RESONANT { 0.1373f, 0.9100f, 0.3162f, 0.4467f, 0.2818f, 6.8100f, 0.1800f, 0.1000f, 0.7079f, 0.0100f, { 0.0000f, 0.0000f, 0.0000f }, 1.0000f, 0.0220f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 2854.3999f, 20.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_OUTDOORS_BACKYARD { 1.0000f, 0.4500f, 0.3162f, 0.2512f, 0.5012f, 1.1200f, 0.3400f, 0.4600f, 0.4467f, 0.0690f, { 0.0000f, 0.0000f, -0.0000f }, 0.7079f, 0.0230f, { 0.0000f, 0.0000f, 0.0000f }, 0.2180f, 0.3400f, 0.2500f, 0.0000f, 0.9943f, 4399.1001f, 242.9000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_OUTDOORS_ROLLINGPLAINS { 1.0000f, 0.0000f, 0.3162f, 0.0112f, 0.6310f, 2.1300f, 0.2100f, 0.4600f, 0.1778f, 0.3000f, { 0.0000f, 0.0000f, -0.0000f }, 0.4467f, 0.0190f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 1.0000f, 0.2500f, 0.0000f, 0.9943f, 4399.1001f, 242.9000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON { 1.0000f, 0.7400f, 0.3162f, 0.1778f, 0.6310f, 3.8900f, 0.2100f, 0.4600f, 0.3162f, 0.2230f, { 0.0000f, 0.0000f, -0.0000f }, 0.3548f, 0.0190f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 1.0000f, 0.2500f, 0.0000f, 0.9943f, 4399.1001f, 242.9000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_OUTDOORS_CREEK { 1.0000f, 0.3500f, 0.3162f, 0.1778f, 0.5012f, 2.1300f, 0.2100f, 0.4600f, 0.3981f, 0.1150f, { 0.0000f, 0.0000f, -0.0000f }, 0.1995f, 0.0310f, { 0.0000f, 0.0000f, 0.0000f }, 0.2180f, 0.3400f, 0.2500f, 0.0000f, 0.9943f, 4399.1001f, 242.9000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_OUTDOORS_VALLEY { 1.0000f, 0.2800f, 0.3162f, 0.0282f, 0.1585f, 2.8800f, 0.2600f, 0.3500f, 0.1413f, 0.2630f, { 0.0000f, 0.0000f, -0.0000f }, 0.3981f, 0.1000f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.3400f, 0.2500f, 0.0000f, 0.9943f, 2854.3999f, 107.5000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_MOOD_HEAVEN { 1.0000f, 0.9400f, 0.3162f, 0.7943f, 0.4467f, 5.0400f, 1.1200f, 0.5600f, 0.2427f, 0.0200f, { 0.0000f, 0.0000f, 0.0000f }, 1.2589f, 0.0290f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0800f, 2.7420f, 0.0500f, 0.9977f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_MOOD_HELL { 1.0000f, 0.5700f, 0.3162f, 0.3548f, 0.4467f, 3.5700f, 0.4900f, 2.0000f, 0.0000f, 0.0200f, { 0.0000f, 0.0000f, 0.0000f }, 1.4125f, 0.0300f, { 0.0000f, 0.0000f, 0.0000f }, 0.1100f, 0.0400f, 2.1090f, 0.5200f, 0.9943f, 5000.0000f, 139.5000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_MOOD_MEMORY { 1.0000f, 0.8500f, 0.3162f, 0.6310f, 0.3548f, 4.0600f, 0.8200f, 0.5600f, 0.0398f, 0.0000f, { 0.0000f, 0.0000f, 0.0000f }, 1.1220f, 0.0000f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.4740f, 0.4500f, 0.9886f, 5000.0000f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_DRIVING_COMMENTATOR { 1.0000f, 0.0000f, 0.3162f, 0.5623f, 0.5012f, 2.4200f, 0.8800f, 0.6800f, 0.1995f, 0.0930f, { 0.0000f, 0.0000f, 0.0000f }, 0.2512f, 0.0170f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 1.0000f, 0.2500f, 0.0000f, 0.9886f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_DRIVING_PITGARAGE { 0.4287f, 0.5900f, 0.3162f, 0.7079f, 0.5623f, 1.7200f, 0.9300f, 0.8700f, 0.5623f, 0.0000f, { 0.0000f, 0.0000f, 0.0000f }, 1.2589f, 0.0160f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.1100f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_DRIVING_INCAR_RACER { 0.0832f, 0.8000f, 0.3162f, 1.0000f, 0.7943f, 0.1700f, 2.0000f, 0.4100f, 1.7783f, 0.0070f, { 0.0000f, 0.0000f, 0.0000f }, 0.7079f, 0.0150f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 10268.2002f, 251.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_DRIVING_INCAR_SPORTS { 0.0832f, 0.8000f, 0.3162f, 0.6310f, 1.0000f, 0.1700f, 0.7500f, 0.4100f, 1.0000f, 0.0100f, { 0.0000f, 0.0000f, 0.0000f }, 0.5623f, 0.0000f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 10268.2002f, 251.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_DRIVING_INCAR_LUXURY { 0.2560f, 1.0000f, 0.3162f, 0.1000f, 0.5012f, 0.1300f, 0.4100f, 0.4600f, 0.7943f, 0.0100f, { 0.0000f, 0.0000f, 0.0000f }, 1.5849f, 0.0100f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 10268.2002f, 251.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_DRIVING_FULLGRANDSTAND { 1.0000f, 1.0000f, 0.3162f, 0.2818f, 0.6310f, 3.0100f, 1.3700f, 1.2800f, 0.3548f, 0.0900f, { 0.0000f, 0.0000f, 0.0000f }, 0.1778f, 0.0490f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 10420.2002f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_DRIVING_EMPTYGRANDSTAND { 1.0000f, 1.0000f, 0.3162f, 1.0000f, 0.7943f, 4.6200f, 1.7500f, 1.4000f, 0.2082f, 0.0900f, { 0.0000f, 0.0000f, 0.0000f }, 0.2512f, 0.0490f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.0000f, 0.9943f, 10420.2002f, 250.0000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_DRIVING_TUNNEL { 1.0000f, 0.8100f, 0.3162f, 0.3981f, 0.8913f, 3.4200f, 0.9400f, 1.3100f, 0.7079f, 0.0510f, { 0.0000f, 0.0000f, 0.0000f }, 0.7079f, 0.0470f, { 0.0000f, 0.0000f, 0.0000f }, 0.2140f, 0.0500f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 155.3000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CITY_STREETS { 1.0000f, 0.7800f, 0.3162f, 0.7079f, 0.8913f, 1.7900f, 1.1200f, 0.9100f, 0.2818f, 0.0460f, { 0.0000f, 0.0000f, 0.0000f }, 0.1995f, 0.0280f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.2000f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CITY_SUBWAY { 1.0000f, 0.7400f, 0.3162f, 0.7079f, 0.8913f, 3.0100f, 1.2300f, 0.9100f, 0.7079f, 0.0460f, { 0.0000f, 0.0000f, 0.0000f }, 1.2589f, 0.0280f, { 0.0000f, 0.0000f, 0.0000f }, 0.1250f, 0.2100f, 0.2500f, 0.0000f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CITY_MUSEUM { 1.0000f, 0.8200f, 0.3162f, 0.1778f, 0.1778f, 3.2800f, 1.4000f, 0.5700f, 0.2512f, 0.0390f, { 0.0000f, 0.0000f, -0.0000f }, 0.8913f, 0.0340f, { 0.0000f, 0.0000f, 0.0000f }, 0.1300f, 0.1700f, 0.2500f, 0.0000f, 0.9943f, 2854.3999f, 107.5000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_CITY_LIBRARY { 1.0000f, 0.8200f, 0.3162f, 0.2818f, 0.0891f, 2.7600f, 0.8900f, 0.4100f, 0.3548f, 0.0290f, { 0.0000f, 0.0000f, -0.0000f }, 0.8913f, 0.0200f, { 0.0000f, 0.0000f, 0.0000f }, 0.1300f, 0.1700f, 0.2500f, 0.0000f, 0.9943f, 2854.3999f, 107.5000f, 0.0000f, 0x0 }
#define EFX_REVERB_PRESET_CITY_UNDERPASS { 1.0000f, 0.8200f, 0.3162f, 0.4467f, 0.8913f, 3.5700f, 1.1200f, 0.9100f, 0.3981f, 0.0590f, { 0.0000f, 0.0000f, 0.0000f }, 0.8913f, 0.0370f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.1400f, 0.2500f, 0.0000f, 0.9920f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CITY_ABANDONED { 1.0000f, 0.6900f, 0.3162f, 0.7943f, 0.8913f, 3.2800f, 1.1700f, 0.9100f, 0.4467f, 0.0440f, { 0.0000f, 0.0000f, 0.0000f }, 0.2818f, 0.0240f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.2000f, 0.2500f, 0.0000f, 0.9966f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_DUSTYROOM { 0.3645f, 0.5600f, 0.3162f, 0.7943f, 0.7079f, 1.7900f, 0.3800f, 0.2100f, 0.5012f, 0.0020f, { 0.0000f, 0.0000f, 0.0000f }, 1.2589f, 0.0060f, { 0.0000f, 0.0000f, 0.0000f }, 0.2020f, 0.0500f, 0.2500f, 0.0000f, 0.9886f, 13046.0000f, 163.3000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_CHAPEL { 1.0000f, 0.8400f, 0.3162f, 0.5623f, 1.0000f, 4.6200f, 0.6400f, 1.2300f, 0.4467f, 0.0320f, { 0.0000f, 0.0000f, 0.0000f }, 0.7943f, 0.0490f, { 0.0000f, 0.0000f, 0.0000f }, 0.2500f, 0.0000f, 0.2500f, 0.1100f, 0.9943f, 5000.0000f, 250.0000f, 0.0000f, 0x1 }
#define EFX_REVERB_PRESET_SMALLWATERROOM { 1.0000f, 0.7000f, 0.3162f, 0.4477f, 1.0000f, 1.5100f, 1.2500f, 1.1400f, 0.8913f, 0.0200f, { 0.0000f, 0.0000f, 0.0000f }, 1.4125f, 0.0300f, { 0.0000f, 0.0000f, 0.0000f }, 0.1790f, 0.1500f, 0.8950f, 0.1900f, 0.9920f, 5000.0000f, 250.0000f, 0.0000f, 0x0 }


typedef void          (AL_APIENTRY *LPALENABLE)(ALenum capability);
typedef void          (AL_APIENTRY *LPALDISABLE)(ALenum capability);
typedef ALboolean     (AL_APIENTRY *LPALISENABLED)(ALenum capability);
typedef const ALchar* (AL_APIENTRY *LPALGETSTRING)(ALenum param);
typedef void          (AL_APIENTRY *LPALGETBOOLEANV)(ALenum param, ALboolean *values);
typedef void          (AL_APIENTRY *LPALGETINTEGERV)(ALenum param, ALint *values);
typedef void          (AL_APIENTRY *LPALGETFLOATV)(ALenum param, ALfloat *values);
typedef void          (AL_APIENTRY *LPALGETDOUBLEV)(ALenum param, ALdouble *values);
typedef ALboolean     (AL_APIENTRY *LPALGETBOOLEAN)(ALenum param);
typedef ALint         (AL_APIENTRY *LPALGETINTEGER)(ALenum param);
typedef ALfloat       (AL_APIENTRY *LPALGETFLOAT)(ALenum param);
typedef ALdouble      (AL_APIENTRY *LPALGETDOUBLE)(ALenum param);
typedef ALenum        (AL_APIENTRY *LPALGETERROR)(void);
typedef ALboolean     (AL_APIENTRY *LPALISEXTENSIONPRESENT)(const ALchar *extname);
typedef void*         (AL_APIENTRY *LPALGETPROCADDRESS)(const ALchar *fname);
typedef ALenum        (AL_APIENTRY *LPALGETENUMVALUE)(const ALchar *ename);
typedef void          (AL_APIENTRY *LPALLISTENERF)(ALenum param, ALfloat value);
typedef void          (AL_APIENTRY *LPALLISTENER3F)(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
typedef void          (AL_APIENTRY *LPALLISTENERFV)(ALenum param, const ALfloat *values);
typedef void          (AL_APIENTRY *LPALLISTENERI)(ALenum param, ALint value);
typedef void          (AL_APIENTRY *LPALLISTENER3I)(ALenum param, ALint value1, ALint value2, ALint value3);
typedef void          (AL_APIENTRY *LPALLISTENERIV)(ALenum param, const ALint *values);
typedef void          (AL_APIENTRY *LPALGETLISTENERF)(ALenum param, ALfloat *value);
typedef void          (AL_APIENTRY *LPALGETLISTENER3F)(ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
typedef void          (AL_APIENTRY *LPALGETLISTENERFV)(ALenum param, ALfloat *values);
typedef void          (AL_APIENTRY *LPALGETLISTENERI)(ALenum param, ALint *value);
typedef void          (AL_APIENTRY *LPALGETLISTENER3I)(ALenum param, ALint *value1, ALint *value2, ALint *value3);
typedef void          (AL_APIENTRY *LPALGETLISTENERIV)(ALenum param, ALint *values);
typedef void          (AL_APIENTRY *LPALGENSOURCES)(ALsizei n, ALuint *sources);
typedef void          (AL_APIENTRY *LPALDELETESOURCES)(ALsizei n, const ALuint *sources);
typedef ALboolean     (AL_APIENTRY *LPALISSOURCE)(ALuint source);
typedef void          (AL_APIENTRY *LPALSOURCEF)(ALuint source, ALenum param, ALfloat value);
typedef void          (AL_APIENTRY *LPALSOURCE3F)(ALuint source, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
typedef void          (AL_APIENTRY *LPALSOURCEFV)(ALuint source, ALenum param, const ALfloat *values);
typedef void          (AL_APIENTRY *LPALSOURCEI)(ALuint source, ALenum param, ALint value);
typedef void          (AL_APIENTRY *LPALSOURCE3I)(ALuint source, ALenum param, ALint value1, ALint value2, ALint value3);
typedef void          (AL_APIENTRY *LPALSOURCEIV)(ALuint source, ALenum param, const ALint *values);
typedef void          (AL_APIENTRY *LPALGETSOURCEF)(ALuint source, ALenum param, ALfloat *value);
typedef void          (AL_APIENTRY *LPALGETSOURCE3F)(ALuint source, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
typedef void          (AL_APIENTRY *LPALGETSOURCEFV)(ALuint source, ALenum param, ALfloat *values);
typedef void          (AL_APIENTRY *LPALGETSOURCEI)(ALuint source, ALenum param, ALint *value);
typedef void          (AL_APIENTRY *LPALGETSOURCE3I)(ALuint source, ALenum param, ALint *value1, ALint *value2, ALint *value3);
typedef void          (AL_APIENTRY *LPALGETSOURCEIV)(ALuint source, ALenum param, ALint *values);
typedef void          (AL_APIENTRY *LPALSOURCEPLAYV)(ALsizei n, const ALuint *sources);
typedef void          (AL_APIENTRY *LPALSOURCESTOPV)(ALsizei n, const ALuint *sources);
typedef void          (AL_APIENTRY *LPALSOURCEREWINDV)(ALsizei n, const ALuint *sources);
typedef void          (AL_APIENTRY *LPALSOURCEPAUSEV)(ALsizei n, const ALuint *sources);
typedef void          (AL_APIENTRY *LPALSOURCEPLAY)(ALuint source);
typedef void          (AL_APIENTRY *LPALSOURCESTOP)(ALuint source);
typedef void          (AL_APIENTRY *LPALSOURCEREWIND)(ALuint source);
typedef void          (AL_APIENTRY *LPALSOURCEPAUSE)(ALuint source);
typedef void          (AL_APIENTRY *LPALSOURCEQUEUEBUFFERS)(ALuint source, ALsizei nb, const ALuint *buffers);
typedef void          (AL_APIENTRY *LPALSOURCEUNQUEUEBUFFERS)(ALuint source, ALsizei nb, ALuint *buffers);
typedef void          (AL_APIENTRY *LPALGENBUFFERS)(ALsizei n, ALuint *buffers);
typedef void          (AL_APIENTRY *LPALDELETEBUFFERS)(ALsizei n, const ALuint *buffers);
typedef ALboolean     (AL_APIENTRY *LPALISBUFFER)(ALuint buffer);
typedef void          (AL_APIENTRY *LPALBUFFERDATA)(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq);
typedef void          (AL_APIENTRY *LPALBUFFERF)(ALuint buffer, ALenum param, ALfloat value);
typedef void          (AL_APIENTRY *LPALBUFFER3F)(ALuint buffer, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
typedef void          (AL_APIENTRY *LPALBUFFERFV)(ALuint buffer, ALenum param, const ALfloat *values);
typedef void          (AL_APIENTRY *LPALBUFFERI)(ALuint buffer, ALenum param, ALint value);
typedef void          (AL_APIENTRY *LPALBUFFER3I)(ALuint buffer, ALenum param, ALint value1, ALint value2, ALint value3);
typedef void          (AL_APIENTRY *LPALBUFFERIV)(ALuint buffer, ALenum param, const ALint *values);
typedef void          (AL_APIENTRY *LPALGETBUFFERF)(ALuint buffer, ALenum param, ALfloat *value);
typedef void          (AL_APIENTRY *LPALGETBUFFER3F)(ALuint buffer, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
typedef void          (AL_APIENTRY *LPALGETBUFFERFV)(ALuint buffer, ALenum param, ALfloat *values);
typedef void          (AL_APIENTRY *LPALGETBUFFERI)(ALuint buffer, ALenum param, ALint *value);
typedef void          (AL_APIENTRY *LPALGETBUFFER3I)(ALuint buffer, ALenum param, ALint *value1, ALint *value2, ALint *value3);
typedef void          (AL_APIENTRY *LPALGETBUFFERIV)(ALuint buffer, ALenum param, ALint *values);
typedef void          (AL_APIENTRY *LPALDOPPLERFACTOR)(ALfloat value);
typedef void          (AL_APIENTRY *LPALDOPPLERVELOCITY)(ALfloat value);
typedef void          (AL_APIENTRY *LPALSPEEDOFSOUND)(ALfloat value);
typedef void          (AL_APIENTRY *LPALDISTANCEMODEL)(ALenum distanceModel);

typedef ALCcontext*    (ALC_APIENTRY *LPALCCREATECONTEXT)(ALCdevice *device, const ALCint *attrlist);
typedef ALCboolean     (ALC_APIENTRY *LPALCMAKECONTEXTCURRENT)(ALCcontext *context);
typedef void           (ALC_APIENTRY *LPALCPROCESSCONTEXT)(ALCcontext *context);
typedef void           (ALC_APIENTRY *LPALCSUSPENDCONTEXT)(ALCcontext *context);
typedef void           (ALC_APIENTRY *LPALCDESTROYCONTEXT)(ALCcontext *context);
typedef ALCcontext*    (ALC_APIENTRY *LPALCGETCURRENTCONTEXT)(void);
typedef ALCdevice*     (ALC_APIENTRY *LPALCGETCONTEXTSDEVICE)(ALCcontext *context);
typedef ALCdevice*     (ALC_APIENTRY *LPALCOPENDEVICE)(const ALCchar *devicename);
typedef ALCboolean     (ALC_APIENTRY *LPALCCLOSEDEVICE)(ALCdevice *device);
typedef ALCenum        (ALC_APIENTRY *LPALCGETERROR)(ALCdevice *device);
typedef ALCboolean     (ALC_APIENTRY *LPALCISEXTENSIONPRESENT)(ALCdevice *device, const ALCchar *extname);
typedef ALCvoid*       (ALC_APIENTRY *LPALCGETPROCADDRESS)(ALCdevice *device, const ALCchar *funcname);
typedef ALCenum        (ALC_APIENTRY *LPALCGETENUMVALUE)(ALCdevice *device, const ALCchar *enumname);
typedef const ALCchar* (ALC_APIENTRY *LPALCGETSTRING)(ALCdevice *device, ALCenum param);
typedef void           (ALC_APIENTRY *LPALCGETINTEGERV)(ALCdevice *device, ALCenum param, ALCsizei size, ALCint *values);
typedef ALCdevice*     (ALC_APIENTRY *LPALCCAPTUREOPENDEVICE)(const ALCchar *devicename, ALCuint frequency, ALCenum format, ALCsizei buffersize);
typedef ALCboolean     (ALC_APIENTRY *LPALCCAPTURECLOSEDEVICE)(ALCdevice *device);
typedef void           (ALC_APIENTRY *LPALCCAPTURESTART)(ALCdevice *device);
typedef void           (ALC_APIENTRY *LPALCCAPTURESTOP)(ALCdevice *device);
typedef void           (ALC_APIENTRY *LPALCCAPTURESAMPLES)(ALCdevice *device, ALCvoid *buffer, ALCsizei samples);

typedef void (AL_APIENTRY*PFNALBUFFERDATASTATICPROC)(const ALint,ALenum,ALvoid*,ALsizei,ALsizei);
typedef ALCboolean  (ALC_APIENTRY*PFNALCSETTHREADCONTEXTPROC)(ALCcontext *context);
typedef ALCcontext* (ALC_APIENTRY*PFNALCGETTHREADCONTEXTPROC)(void);
typedef void (AL_APIENTRY*PFNALBUFFERSUBDATASOFTPROC)(ALuint,ALenum,const ALvoid*,ALsizei,ALsizei);
typedef void (AL_APIENTRY*LPALFOLDBACKCALLBACK)(ALenum,ALsizei);
typedef void (AL_APIENTRY*LPALREQUESTFOLDBACKSTART)(ALenum,ALsizei,ALsizei,ALfloat*,LPALFOLDBACKCALLBACK);
typedef void (AL_APIENTRY*LPALREQUESTFOLDBACKSTOP)(void);
typedef void (AL_APIENTRY*LPALBUFFERSAMPLESSOFT)(ALuint,ALuint,ALenum,ALsizei,ALenum,ALenum,const ALvoid*);
typedef void (AL_APIENTRY*LPALBUFFERSUBSAMPLESSOFT)(ALuint,ALsizei,ALsizei,ALenum,ALenum,const ALvoid*);
typedef void (AL_APIENTRY*LPALGETBUFFERSAMPLESSOFT)(ALuint,ALsizei,ALsizei,ALenum,ALenum,ALvoid*);
typedef ALboolean (AL_APIENTRY*LPALISBUFFERFORMATSUPPORTEDSOFT)(ALenum);
typedef ALCdevice* (ALC_APIENTRY*LPALCLOOPBACKOPENDEVICESOFT)(const ALCchar*);
typedef ALCboolean (ALC_APIENTRY*LPALCISRENDERFORMATSUPPORTEDSOFT)(ALCdevice*,ALCsizei,ALCenum,ALCenum);
typedef void (ALC_APIENTRY*LPALCRENDERSAMPLESSOFT)(ALCdevice*,ALCvoid*,ALCsizei);
typedef void (AL_APIENTRY*LPALSOURCEDSOFT)(ALuint,ALenum,ALdouble);
typedef void (AL_APIENTRY*LPALSOURCE3DSOFT)(ALuint,ALenum,ALdouble,ALdouble,ALdouble);
typedef void (AL_APIENTRY*LPALSOURCEDVSOFT)(ALuint,ALenum,const ALdouble*);
typedef void (AL_APIENTRY*LPALGETSOURCEDSOFT)(ALuint,ALenum,ALdouble*);
typedef void (AL_APIENTRY*LPALGETSOURCE3DSOFT)(ALuint,ALenum,ALdouble*,ALdouble*,ALdouble*);
typedef void (AL_APIENTRY*LPALGETSOURCEDVSOFT)(ALuint,ALenum,ALdouble*);
typedef void (AL_APIENTRY*LPALSOURCEI64SOFT)(ALuint,ALenum,ALint64SOFT);
typedef void (AL_APIENTRY*LPALSOURCE3I64SOFT)(ALuint,ALenum,ALint64SOFT,ALint64SOFT,ALint64SOFT);
typedef void (AL_APIENTRY*LPALSOURCEI64VSOFT)(ALuint,ALenum,const ALint64SOFT*);
typedef void (AL_APIENTRY*LPALGETSOURCEI64SOFT)(ALuint,ALenum,ALint64SOFT*);
typedef void (AL_APIENTRY*LPALGETSOURCE3I64SOFT)(ALuint,ALenum,ALint64SOFT*,ALint64SOFT*,ALint64SOFT*);
typedef void (AL_APIENTRY*LPALGETSOURCEI64VSOFT)(ALuint,ALenum,ALint64SOFT*);
typedef void (AL_APIENTRY*LPALDEFERUPDATESSOFT)(void);
typedef void (AL_APIENTRY*LPALPROCESSUPDATESSOFT)(void);
typedef void (ALC_APIENTRY*LPALCDEVICEPAUSESOFT)(ALCdevice *device);
typedef void (ALC_APIENTRY*LPALCDEVICERESUMESOFT)(ALCdevice *device);
typedef const ALCchar* (ALC_APIENTRY*LPALCGETSTRINGISOFT)(ALCdevice *device, ALCenum paramName, ALCsizei index);
typedef ALCboolean (ALC_APIENTRY*LPALCRESETDEVICESOFT)(ALCdevice *device, const ALCint *attribs);
typedef const ALchar* (AL_APIENTRY*LPALGETSTRINGISOFT)(ALenum pname, ALsizei index);
typedef void (ALC_APIENTRY*LPALCGETINTEGER64VSOFT)(ALCdevice *device, ALCenum pname, ALsizei size, ALCint64SOFT *values);
typedef void (AL_APIENTRY*ALEVENTPROCSOFT)(ALenum eventType, ALuint object, ALuint param, ALsizei length, const ALchar *message, void *userParam);
typedef void (AL_APIENTRY*LPALEVENTCONTROLSOFT)(ALsizei count, const ALenum *types, ALboolean enable);
typedef void (AL_APIENTRY*LPALEVENTCALLBACKSOFT)(ALEVENTPROCSOFT callback, void *userParam);
typedef void* (AL_APIENTRY*LPALGETPOINTERSOFT)(ALenum pname);
typedef void (AL_APIENTRY*LPALGETPOINTERVSOFT)(ALenum pname, void **values);
typedef ALCboolean (ALC_APIENTRY*LPALCREOPENDEVICESOFT)(ALCdevice *device, const ALCchar *deviceName, const ALCint *attribs);
typedef ALsizei (AL_APIENTRY*ALBUFFERCALLBACKTYPESOFT)(ALvoid *userptr, ALvoid *sampledata, ALsizei numbytes);
typedef void (AL_APIENTRY*LPALBUFFERCALLBACKSOFT)(ALuint buffer, ALenum format, ALsizei freq, ALBUFFERCALLBACKTYPESOFT callback, ALvoid *userptr);
typedef void (AL_APIENTRY*LPALGETBUFFERPTRSOFT)(ALuint buffer, ALenum param, ALvoid **value);
typedef void (AL_APIENTRY*LPALGETBUFFER3PTRSOFT)(ALuint buffer, ALenum param, ALvoid **value1, ALvoid **value2, ALvoid **value3);
typedef void (AL_APIENTRY*LPALGETBUFFERPTRVSOFT)(ALuint buffer, ALenum param, ALvoid **values);

typedef void (AL_APIENTRY *LPALGENEFFECTS)(ALsizei, ALuint*);
typedef void (AL_APIENTRY *LPALDELETEEFFECTS)(ALsizei, const ALuint*);
typedef ALboolean (AL_APIENTRY *LPALISEFFECT)(ALuint);
typedef void (AL_APIENTRY *LPALEFFECTI)(ALuint, ALenum, ALint);
typedef void (AL_APIENTRY *LPALEFFECTIV)(ALuint, ALenum, const ALint*);
typedef void (AL_APIENTRY *LPALEFFECTF)(ALuint, ALenum, ALfloat);
typedef void (AL_APIENTRY *LPALEFFECTFV)(ALuint, ALenum, const ALfloat*);
typedef void (AL_APIENTRY *LPALGETEFFECTI)(ALuint, ALenum, ALint*);
typedef void (AL_APIENTRY *LPALGETEFFECTIV)(ALuint, ALenum, ALint*);
typedef void (AL_APIENTRY *LPALGETEFFECTF)(ALuint, ALenum, ALfloat*);
typedef void (AL_APIENTRY *LPALGETEFFECTFV)(ALuint, ALenum, ALfloat*);
typedef void (AL_APIENTRY *LPALGENFILTERS)(ALsizei, ALuint*);
typedef void (AL_APIENTRY *LPALDELETEFILTERS)(ALsizei, const ALuint*);
typedef ALboolean (AL_APIENTRY *LPALISFILTER)(ALuint);
typedef void (AL_APIENTRY *LPALFILTERI)(ALuint, ALenum, ALint);
typedef void (AL_APIENTRY *LPALFILTERIV)(ALuint, ALenum, const ALint*);
typedef void (AL_APIENTRY *LPALFILTERF)(ALuint, ALenum, ALfloat);
typedef void (AL_APIENTRY *LPALFILTERFV)(ALuint, ALenum, const ALfloat*);
typedef void (AL_APIENTRY *LPALGETFILTERI)(ALuint, ALenum, ALint*);
typedef void (AL_APIENTRY *LPALGETFILTERIV)(ALuint, ALenum, ALint*);
typedef void (AL_APIENTRY *LPALGETFILTERF)(ALuint, ALenum, ALfloat*);
typedef void (AL_APIENTRY *LPALGETFILTERFV)(ALuint, ALenum, ALfloat*);
typedef void (AL_APIENTRY *LPALGENAUXILIARYEFFECTSLOTS)(ALsizei, ALuint*);
typedef void (AL_APIENTRY *LPALDELETEAUXILIARYEFFECTSLOTS)(ALsizei, const ALuint*);
typedef ALboolean (AL_APIENTRY *LPALISAUXILIARYEFFECTSLOT)(ALuint);
typedef void (AL_APIENTRY *LPALAUXILIARYEFFECTSLOTI)(ALuint, ALenum, ALint);
typedef void (AL_APIENTRY *LPALAUXILIARYEFFECTSLOTIV)(ALuint, ALenum, const ALint*);
typedef void (AL_APIENTRY *LPALAUXILIARYEFFECTSLOTF)(ALuint, ALenum, ALfloat);
typedef void (AL_APIENTRY *LPALAUXILIARYEFFECTSLOTFV)(ALuint, ALenum, const ALfloat*);
typedef void (AL_APIENTRY *LPALGETAUXILIARYEFFECTSLOTI)(ALuint, ALenum, ALint*);
typedef void (AL_APIENTRY *LPALGETAUXILIARYEFFECTSLOTIV)(ALuint, ALenum, ALint*);
typedef void (AL_APIENTRY *LPALGETAUXILIARYEFFECTSLOTF)(ALuint, ALenum, ALfloat*);
typedef void (AL_APIENTRY *LPALGETAUXILIARYEFFECTSLOTFV)(ALuint, ALenum, ALfloat*);





#ifdef ALAD_IMPLEMENTATION

//Core AL
LPALDOPPLERFACTOR alDopplerFactor = NULL;
LPALDOPPLERVELOCITY alDopplerVelocity = NULL;
LPALSPEEDOFSOUND alSpeedOfSound = NULL;
LPALDISTANCEMODEL alDistanceModel = NULL;
LPALENABLE alEnable = NULL;
LPALDISABLE alDisable = NULL;
LPALISENABLED alIsEnabled = NULL;
LPALGETSTRING alGetString = NULL;
LPALGETBOOLEANV alGetBooleanv = NULL;
LPALGETINTEGERV alGetIntegerv = NULL;
LPALGETFLOATV alGetFloatv = NULL;
LPALGETDOUBLEV alGetDoublev = NULL;
LPALGETBOOLEAN alGetBoolean = NULL;
LPALGETINTEGER alGetInteger = NULL;
LPALGETFLOAT alGetFloat = NULL;
LPALGETDOUBLE alGetDouble = NULL;
LPALGETERROR alGetError = NULL;
LPALISEXTENSIONPRESENT alIsExtensionPresent = NULL;
LPALGETPROCADDRESS alGetProcAddress = NULL;
LPALGETENUMVALUE alGetEnumValue = NULL;
LPALLISTENERF alListenerf = NULL;
LPALLISTENER3F alListener3f = NULL;
LPALLISTENERFV alListenerfv = NULL;
LPALLISTENERI alListeneri = NULL;
LPALLISTENER3I alListener3i = NULL;
LPALLISTENERIV alListeneriv = NULL;
LPALGETLISTENERF alGetListenerf = NULL;
LPALGETLISTENER3F alGetListener3f = NULL;
LPALGETLISTENERFV alGetListenerfv = NULL;
LPALGETLISTENERI alGetListeneri = NULL;
LPALGETLISTENER3I alGetListener3i = NULL;
LPALGETLISTENERIV alGetListeneriv = NULL;
LPALGENSOURCES alGenSources = NULL;
LPALDELETESOURCES alDeleteSources = NULL;
LPALISSOURCE alIsSource = NULL;
LPALSOURCEF alSourcef = NULL;
LPALSOURCE3F alSource3f = NULL;
LPALSOURCEFV alSourcefv = NULL;
LPALSOURCEI alSourcei = NULL;
LPALSOURCE3I alSource3i = NULL;
LPALSOURCEIV alSourceiv = NULL;
LPALGETSOURCEF alGetSourcef = NULL;
LPALGETSOURCE3F alGetSource3f = NULL;
LPALGETSOURCEFV alGetSourcefv = NULL;
LPALGETSOURCEI alGetSourcei = NULL;
LPALGETSOURCE3I alGetSource3i = NULL;
LPALGETSOURCEIV alGetSourceiv = NULL;
LPALSOURCEPLAYV alSourcePlayv = NULL;
LPALSOURCESTOPV alSourceStopv = NULL;
LPALSOURCEREWINDV alSourceRewindv = NULL;
LPALSOURCEPAUSEV alSourcePausev = NULL;
LPALSOURCEPLAY alSourcePlay = NULL;
LPALSOURCESTOP alSourceStop = NULL;
LPALSOURCEREWIND alSourceRewind = NULL;
LPALSOURCEPAUSE alSourcePause = NULL;
LPALSOURCEQUEUEBUFFERS alSourceQueueBuffers = NULL;
LPALSOURCEUNQUEUEBUFFERS alSourceUnqueueBuffers = NULL;
LPALGENBUFFERS alGenBuffers = NULL;
LPALDELETEBUFFERS alDeleteBuffers = NULL;
LPALISBUFFER alIsBuffer = NULL;
LPALBUFFERDATA alBufferData = NULL;
LPALBUFFERF alBufferf = NULL;
LPALBUFFER3F alBuffer3f = NULL;
LPALBUFFERFV alBufferfv = NULL;
LPALBUFFERI alBufferi = NULL;
LPALBUFFER3I alBuffer3i = NULL;
LPALBUFFERIV alBufferiv = NULL;
LPALGETBUFFERF alGetBufferf = NULL;
LPALGETBUFFER3F alGetBuffer3f = NULL;
LPALGETBUFFERFV alGetBufferfv = NULL;
LPALGETBUFFERI alGetBufferi = NULL;
LPALGETBUFFER3I alGetBuffer3i = NULL;
LPALGETBUFFERIV alGetBufferiv = NULL;

//EFX
LPALGENEFFECTS alGenEffects = NULL;
LPALDELETEEFFECTS alDeleteEffects = NULL;
LPALISEFFECT alIsEffect = NULL;
LPALEFFECTI alEffecti = NULL;
LPALEFFECTIV alEffectiv = NULL;
LPALEFFECTF alEffectf = NULL;
LPALEFFECTFV alEffectfv = NULL;
LPALGETEFFECTI alGetEffecti = NULL;
LPALGETEFFECTIV alGetEffectiv = NULL;
LPALGETEFFECTF alGetEffectf = NULL;
LPALGETEFFECTFV alGetEffectfv = NULL;
LPALGENFILTERS alGenFilters = NULL;
LPALDELETEFILTERS alDeleteFilters = NULL;
LPALISFILTER alIsFilter = NULL;
LPALFILTERI alFilteri = NULL;
LPALFILTERIV alFilteriv = NULL;
LPALFILTERF alFilterf = NULL;
LPALFILTERFV alFilterfv = NULL;
LPALGETFILTERI alGetFilteri = NULL;
LPALGETFILTERIV alGetFilteriv = NULL;
LPALGETFILTERF alGetFilterf = NULL;
LPALGETFILTERFV alGetFilterfv = NULL;
LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots = NULL;
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots = NULL;
LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot = NULL;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti = NULL;
LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv = NULL;
LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf = NULL;
LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv = NULL;
LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti = NULL;
LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv = NULL;
LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf = NULL;
LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv = NULL;

//AL extensions
//AL_EXT_STATIC_BUFFER
PFNALBUFFERDATASTATICPROC alBufferDataStatic = NULL;
//AL_SOFT_buffer_sub_data
PFNALBUFFERSUBDATASOFTPROC alBufferSubDataSOFT = NULL;
//AL_EXT_FOLDBACK
LPALREQUESTFOLDBACKSTART alRequestFoldbackStart = NULL;
LPALREQUESTFOLDBACKSTOP alRequestFoldbackStop = NULL;
//AL_SOFT_buffer_samples
LPALBUFFERSAMPLESSOFT alBufferSamplesSOFT = NULL;
LPALBUFFERSUBSAMPLESSOFT alBufferSubSamplesSOFT = NULL;
LPALGETBUFFERSAMPLESSOFT alGetBufferSamplesSOFT = NULL;
LPALISBUFFERFORMATSUPPORTEDSOFT alIsBufferFormatSupportedSOFT = NULL;
//AL_SOFT_source_latency
LPALSOURCEDSOFT alSourcedSOFT = NULL;
LPALSOURCE3DSOFT alSource3dSOFT = NULL;
LPALSOURCEDVSOFT alSourcedvSOFT = NULL;
LPALGETSOURCEDSOFT alGetSourcedSOFT = NULL;
LPALGETSOURCE3DSOFT alGetSource3dSOFT = NULL;
LPALGETSOURCEDVSOFT alGetSourcedvSOFT = NULL;
LPALSOURCEI64SOFT alSourcei64SOFT = NULL;
LPALSOURCE3I64SOFT alSource3i64SOFT = NULL;
LPALSOURCEI64VSOFT alSourcei64vSOFT = NULL;
LPALGETSOURCEI64SOFT alGetSourcei64SOFT = NULL;
LPALGETSOURCE3I64SOFT alGetSource3i64SOFT = NULL;
LPALGETSOURCEI64VSOFT alGetSourcei64vSOFT = NULL;
//AL_SOFT_deferred_updates
LPALDEFERUPDATESSOFT alDeferUpdatesSOFT = NULL;
LPALPROCESSUPDATESSOFT alProcessUpdatesSOFT = NULL;
//AL_SOFT_source_resampler
LPALGETSTRINGISOFT alGetStringiSOFT = NULL;
//AL_SOFT_events
LPALEVENTCONTROLSOFT alEventControlSOFT = NULL;
LPALEVENTCALLBACKSOFT alEventCallbackSOFT = NULL;
LPALGETPOINTERSOFT alGetPointerSOFT = NULL;
LPALGETPOINTERVSOFT alGetPointervSOFT = NULL;
//AL_SOFT_callback_buffer
LPALBUFFERCALLBACKSOFT alBufferCallbackSOFT = NULL;
LPALGETBUFFERPTRSOFT alGetBufferPtrSOFT = NULL;
LPALGETBUFFER3PTRSOFT alGetBuffer3PtrSOFT = NULL;
LPALGETBUFFERPTRVSOFT alGetBufferPtrvSOFT = NULL;

//Core ALC
LPALCCREATECONTEXT alcCreateContext = NULL;
LPALCMAKECONTEXTCURRENT alcMakeContextCurrent = NULL;
LPALCPROCESSCONTEXT alcProcessContext = NULL;
LPALCSUSPENDCONTEXT alcSuspendContext = NULL;
LPALCDESTROYCONTEXT alcDestroyContext = NULL;
LPALCGETCURRENTCONTEXT alcGetCurrentContext = NULL;
LPALCGETCONTEXTSDEVICE alcGetContextsDevice = NULL;
LPALCOPENDEVICE alcOpenDevice = NULL;
LPALCCLOSEDEVICE alcCloseDevice = NULL;
LPALCGETERROR alcGetError = NULL;
LPALCISEXTENSIONPRESENT alcIsExtensionPresent = NULL;
LPALCGETPROCADDRESS alcGetProcAddress = NULL;
LPALCGETENUMVALUE alcGetEnumValue = NULL;
LPALCGETSTRING alcGetString = NULL;
LPALCGETINTEGERV alcGetIntegerv = NULL;
LPALCCAPTUREOPENDEVICE alcCaptureOpenDevice = NULL;
LPALCCAPTURECLOSEDEVICE alcCaptureCloseDevice = NULL;
LPALCCAPTURESTART alcCaptureStart = NULL;
LPALCCAPTURESTOP alcCaptureStop = NULL;
LPALCCAPTURESAMPLES alcCaptureSamples = NULL;

//ALC extensions
//ALC_EXT_thread_local_context
PFNALCSETTHREADCONTEXTPROC alcSetThreadContext = NULL;
PFNALCGETTHREADCONTEXTPROC alcGetThreadContext = NULL;
//ALC_SOFT_loopback
LPALCLOOPBACKOPENDEVICESOFT alcLoopbackOpenDeviceSOFT = NULL;
LPALCISRENDERFORMATSUPPORTEDSOFT alcIsRenderFormatSupportedSOFT = NULL;
LPALCRENDERSAMPLESSOFT alcRenderSamplesSOFT = NULL;
//ALC_SOFT_pause_device
LPALCDEVICEPAUSESOFT alcDevicePauseSOFT = NULL;
LPALCDEVICERESUMESOFT alcDeviceResumeSOFT = NULL;
//ALC_SOFT_HRTF
LPALCGETSTRINGISOFT alcGetStringiSOFT = NULL;
LPALCRESETDEVICESOFT alcResetDeviceSOFT = NULL;
//ALC_SOFT_device_clock
LPALCGETINTEGER64VSOFT alcGetInteger64vSOFT = NULL;
//ALC_SOFT_reopen_device
LPALCREOPENDEVICESOFT alcReopenDeviceSOFT = NULL;



void _alad_load_al_functions() {
    if(alGetProcAddress == NULL) return;
    //Core AL
    alDopplerFactor                 = alGetProcAddress("alDopplerFactor");
    alDopplerVelocity               = alGetProcAddress("alDopplerVelocity");
    alSpeedOfSound                  = alGetProcAddress("alSpeedOfSound");
    alDistanceModel                 = alGetProcAddress("alDistanceModel");
    alEnable                        = alGetProcAddress("alEnable");
    alDisable                       = alGetProcAddress("alDisable");
    alIsEnabled                     = alGetProcAddress("alIsEnabled");
    alGetString                     = alGetProcAddress("alGetString");
    alGetBooleanv                   = alGetProcAddress("alGetBooleanv");
    alGetIntegerv                   = alGetProcAddress("alGetIntegerv");
    alGetFloatv                     = alGetProcAddress("alGetFloatv");
    alGetDoublev                    = alGetProcAddress("alGetDoublev");
    alGetBoolean                    = alGetProcAddress("alGetBoolean");
    alGetInteger                    = alGetProcAddress("alGetInteger");
    alGetFloat                      = alGetProcAddress("alGetFloat");
    alGetDouble                     = alGetProcAddress("alGetDouble");
    alGetError                      = alGetProcAddress("alGetError");
    alIsExtensionPresent            = alGetProcAddress("alIsExtensionPresent");
    alGetEnumValue                  = alGetProcAddress("alGetEnumValue");
    alListenerf                     = alGetProcAddress("alListenerf");
    alListener3f                    = alGetProcAddress("alListener3f");
    alListenerfv                    = alGetProcAddress("alListenerfv");
    alListeneri                     = alGetProcAddress("alListeneri");
    alListener3i                    = alGetProcAddress("alListener3i");
    alListeneriv                    = alGetProcAddress("alListeneriv");
    alGetListenerf                  = alGetProcAddress("alGetListenerf");
    alGetListener3f                 = alGetProcAddress("alGetListener3f");
    alGetListenerfv                 = alGetProcAddress("alGetListenerfv");
    alGetListeneri                  = alGetProcAddress("alGetListeneri");
    alGetListener3i                 = alGetProcAddress("alGetListener3i");
    alGetListeneriv                 = alGetProcAddress("alGetListeneriv");
    alGenSources                    = alGetProcAddress("alGenSources");
    alDeleteSources                 = alGetProcAddress("alDeleteSources");
    alIsSource                      = alGetProcAddress("alIsSource");
    alSourcef                       = alGetProcAddress("alSourcef");
    alSource3f                      = alGetProcAddress("alSource3f");
    alSourcefv                      = alGetProcAddress("alSourcefv");
    alSourcei                       = alGetProcAddress("alSourcei");
    alSource3i                      = alGetProcAddress("alSource3i");
    alSourceiv                      = alGetProcAddress("alSourceiv");
    alGetSourcef                    = alGetProcAddress("alGetSourcef");
    alGetSource3f                   = alGetProcAddress("alGetSource3f");
    alGetSourcefv                   = alGetProcAddress("alGetSourcefv");
    alGetSourcei                    = alGetProcAddress("alGetSourcei");
    alGetSource3i                   = alGetProcAddress("alGetSource3i");
    alGetSourceiv                   = alGetProcAddress("alGetSourceiv");
    alSourcePlayv                   = alGetProcAddress("alSourcePlayv");
    alSourceStopv                   = alGetProcAddress("alSourceStopv");
    alSourceRewindv                 = alGetProcAddress("alSourceRewindv");
    alSourcePausev                  = alGetProcAddress("alSourcePausev");
    alSourcePlay                    = alGetProcAddress("alSourcePlay");
    alSourceStop                    = alGetProcAddress("alSourceStop");
    alSourceRewind                  = alGetProcAddress("alSourceRewind");
    alSourcePause                   = alGetProcAddress("alSourcePause");
    alSourceQueueBuffers            = alGetProcAddress("alSourceQueueBuffers");
    alSourceUnqueueBuffers          = alGetProcAddress("alSourceUnqueueBuffers");
    alGenBuffers                    = alGetProcAddress("alGenBuffers");
    alDeleteBuffers                 = alGetProcAddress("alDeleteBuffers");
    alIsBuffer                      = alGetProcAddress("alIsBuffer");
    alBufferData                    = alGetProcAddress("alBufferData");
    alBufferf                       = alGetProcAddress("alBufferf");
    alBuffer3f                      = alGetProcAddress("alBuffer3f");
    alBufferfv                      = alGetProcAddress("alBufferfv");
    alBufferi                       = alGetProcAddress("alBufferi");
    alBuffer3i                      = alGetProcAddress("alBuffer3i");
    alBufferiv                      = alGetProcAddress("alBufferiv");
    alGetBufferf                    = alGetProcAddress("alGetBufferf");
    alGetBuffer3f                   = alGetProcAddress("alGetBuffer3f");
    alGetBufferfv                   = alGetProcAddress("alGetBufferfv");
    alGetBufferi                    = alGetProcAddress("alGetBufferi");
    alGetBuffer3i                   = alGetProcAddress("alGetBuffer3i");
    alGetBufferiv                   = alGetProcAddress("alGetBufferiv");

    //EFX
    alGenEffects                    = alGetProcAddress("alGenEffects");
    alDeleteEffects                 = alGetProcAddress("alDeleteEffects");
    alIsEffect                      = alGetProcAddress("alIsEffect");
    alEffecti                       = alGetProcAddress("alEffecti");
    alEffectiv                      = alGetProcAddress("alEffectiv");
    alEffectf                       = alGetProcAddress("alEffectf");
    alEffectfv                      = alGetProcAddress("alEffectfv");
    alGetEffecti                    = alGetProcAddress("alGetEffecti");
    alGetEffectiv                   = alGetProcAddress("alGetEffectiv");
    alGetEffectf                    = alGetProcAddress("alGetEffectf");
    alGetEffectfv                   = alGetProcAddress("alGetEffectfv");
    alGenFilters                    = alGetProcAddress("alGenFilters");
    alDeleteFilters                 = alGetProcAddress("alDeleteFilters");
    alIsFilter                      = alGetProcAddress("alIsFilter");
    alFilteri                       = alGetProcAddress("alFilteri");
    alFilteriv                      = alGetProcAddress("alFilteriv");
    alFilterf                       = alGetProcAddress("alFilterf");
    alFilterfv                      = alGetProcAddress("alFilterfv");
    alGetFilteri                    = alGetProcAddress("alGetFilteri");
    alGetFilteriv                   = alGetProcAddress("alGetFilteriv");
    alGetFilterf                    = alGetProcAddress("alGetFilterf");
    alGetFilterfv                   = alGetProcAddress("alGetFilterfv");
    alGenAuxiliaryEffectSlots       = alGetProcAddress("alGenAuxiliaryEffectSlots");
    alDeleteAuxiliaryEffectSlots    = alGetProcAddress("alDeleteAuxiliaryEffectSlots");
    alIsAuxiliaryEffectSlot         = alGetProcAddress("alIsAuxiliaryEffectSlot");
    alAuxiliaryEffectSloti          = alGetProcAddress("alAuxiliaryEffectSloti");
    alAuxiliaryEffectSlotiv         = alGetProcAddress("alAuxiliaryEffectSlotiv");
    alAuxiliaryEffectSlotf          = alGetProcAddress("alAuxiliaryEffectSlotf");
    alAuxiliaryEffectSlotfv         = alGetProcAddress("alAuxiliaryEffectSlotfv");
    alGetAuxiliaryEffectSloti       = alGetProcAddress("alGetAuxiliaryEffectSloti");
    alGetAuxiliaryEffectSlotiv      = alGetProcAddress("alGetAuxiliaryEffectSlotiv");
    alGetAuxiliaryEffectSlotf       = alGetProcAddress("alGetAuxiliaryEffectSlotf");
    alGetAuxiliaryEffectSlotfv      = alGetProcAddress("alGetAuxiliaryEffectSlotfv");

    //AL extensions
    //AL_EXT_STATIC_BUFFER
    alBufferDataStatic              = alGetProcAddress("alBufferDataStatic");
    //AL_SOFT_buffer_sub_data
    alBufferSubDataSOFT             = alGetProcAddress("alBufferSubDataSOFT");
    //AL_EXT_FOLDBACK
    alRequestFoldbackStart          = alGetProcAddress("alRequestFoldbackStart");
    alRequestFoldbackStop           = alGetProcAddress("alRequestFoldbackStop");
    //AL_SOFT_buffer_samples
    alBufferSamplesSOFT             = alGetProcAddress("alBufferSamplesSOFT");
    alBufferSubSamplesSOFT          = alGetProcAddress("alBufferSubSamplesSOFT");
    alGetBufferSamplesSOFT          = alGetProcAddress("alGetBufferSamplesSOFT");
    alIsBufferFormatSupportedSOFT   = alGetProcAddress("alIsBufferFormatSupportedSOFT");
    //AL_SOFT_source_latency
    alSourcedSOFT                   = alGetProcAddress("alSourcedSOFT");
    alSource3dSOFT                  = alGetProcAddress("alSource3dSOFT");
    alSourcedvSOFT                  = alGetProcAddress("alSourcedvSOFT");
    alGetSourcedSOFT                = alGetProcAddress("alGetSourcedSOFT");
    alGetSource3dSOFT               = alGetProcAddress("alGetSource3dSOFT");
    alGetSourcedvSOFT               = alGetProcAddress("alGetSourcedvSOFT");
    alSourcei64SOFT                 = alGetProcAddress("alSourcei64SOFT");
    alSource3i64SOFT                = alGetProcAddress("alSource3i64SOFT");
    alSourcei64vSOFT                = alGetProcAddress("alSourcei64vSOFT");
    alGetSourcei64SOFT              = alGetProcAddress("alGetSourcei64SOFT");
    alGetSource3i64SOFT             = alGetProcAddress("alGetSource3i64SOFT");
    alGetSourcei64vSOFT             = alGetProcAddress("alGetSourcei64vSOFT");
    //AL_SOFT_deferred_updates
    alDeferUpdatesSOFT              = alGetProcAddress("alDeferUpdatesSOFT");
    alProcessUpdatesSOFT            = alGetProcAddress("alProcessUpdatesSOFT");
    //AL_SOFT_source_resampler
    alGetStringiSOFT                = alGetProcAddress("alGetStringiSOFT");
    //AL_SOFT_events
    alEventControlSOFT              = alGetProcAddress("alEventControlSOFT");
    alEventCallbackSOFT             = alGetProcAddress("alEventCallbackSOFT");
    alGetPointerSOFT                = alGetProcAddress("alGetPointerSOFT");
    alGetPointervSOFT               = alGetProcAddress("alGetPointervSOFT");
    //AL_SOFT_callback_buffer
    alBufferCallbackSOFT            = alGetProcAddress("alBufferCallbackSOFT");
    alGetBufferPtrSOFT              = alGetProcAddress("alGetBufferPtrSOFT");
    alGetBuffer3PtrSOFT             = alGetProcAddress("alGetBuffer3PtrSOFT");
    alGetBufferPtrvSOFT             = alGetProcAddress("alGetBufferPtrvSOFT");
}

void _alad_load_alc_functions(ALCdevice *device) {
    if((alGetProcAddress == NULL) && (alcGetProcAddress == NULL)) return;
    if(alcGetProcAddress == NULL) {
        alcGetProcAddress = alGetProcAddress("alcGetProcAddress");
    }
    //Core ALC
    alcCreateContext                = alcGetProcAddress(device, "alcCreateContext");
    alcMakeContextCurrent           = alcGetProcAddress(device, "alcMakeContextCurrent");
    alcProcessContext               = alcGetProcAddress(device, "alcProcessContext");
    alcSuspendContext               = alcGetProcAddress(device, "alcSuspendContext");
    alcDestroyContext               = alcGetProcAddress(device, "alcDestroyContext");
    alcGetCurrentContext            = alcGetProcAddress(device, "alcGetCurrentContext");
    alcGetContextsDevice            = alcGetProcAddress(device, "alcGetContextsDevice");
    alcOpenDevice                   = alcGetProcAddress(device, "alcOpenDevice");
    alcCloseDevice                  = alcGetProcAddress(device, "alcCloseDevice");
    alcGetError                     = alcGetProcAddress(device, "alcGetError");
    alcIsExtensionPresent           = alcGetProcAddress(device, "alcIsExtensionPresent");
    alcGetEnumValue                 = alcGetProcAddress(device, "alcGetEnumValue");
    alcGetString                    = alcGetProcAddress(device, "alcGetString");
    alcGetIntegerv                  = alcGetProcAddress(device, "alcGetIntegerv");
    alcCaptureOpenDevice            = alcGetProcAddress(device, "alcCaptureOpenDevice");
    alcCaptureCloseDevice           = alcGetProcAddress(device, "alcCaptureCloseDevice");
    alcCaptureStart                 = alcGetProcAddress(device, "alcCaptureStart");
    alcCaptureStop                  = alcGetProcAddress(device, "alcCaptureStop");
    alcCaptureSamples               = alcGetProcAddress(device, "alcCaptureSamples");
    //ALC extensions
    //ALC_EXT_thread_local_context
    alcSetThreadContext             = alcGetProcAddress(device, "alcSetThreadContext");
    alcGetThreadContext             = alcGetProcAddress(device, "alcGetThreadContext");
    //ALC_SOFT_loopback
    alcLoopbackOpenDeviceSOFT       = alcGetProcAddress(device, "alcLoopbackOpenDeviceSOFT");
    alcIsRenderFormatSupportedSOFT  = alcGetProcAddress(device, "alcIsRenderFormatSupportedSOFT");
    alcRenderSamplesSOFT            = alcGetProcAddress(device, "alcRenderSamplesSOFT");
    //ALC_SOFT_pause_device
    alcDevicePauseSOFT              = alcGetProcAddress(device, "alcDevicePauseSOFT");
    alcDeviceResumeSOFT             = alcGetProcAddress(device, "alcDeviceResumeSOFT");
    //ALC_SOFT_HRTF
    alcGetStringiSOFT               = alcGetProcAddress(device, "alcGetStringiSOFT");
    alcResetDeviceSOFT              = alcGetProcAddress(device, "alcResetDeviceSOFT");
    //ALC_SOFT_device_clock
    alcGetInteger64vSOFT            = alcGetProcAddress(device, "alcGetInteger64vSOFT");
    //ALC_SOFT_reopen_device
    alcReopenDeviceSOFT             = alcGetProcAddress(device, "alcReopenDeviceSOFT");
}






typedef void (*_alad_proc)(void);

void* _alad_open(const char* path);
_alad_proc _alad_load(void* module, const char* name);
void _alad_close(void* module);

//modelled after GLFW, see win32_module.c and posix_module.c specifically
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__)
#define NOMINMAX
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
    void* _alad_open(const char* path) {
        return LoadLibraryA(path);
    }
    _alad_proc _alad_load(void* module, const char* name) {
        return (_alad_proc) GetProcAddress((HMODULE) module, name);
    }
    void _alad_close(void* module) {
        FreeLibrary((HMODULE) module);
    }
#define _alad_LIB_NAME "OpenAL32.dll"
#define _alad_SECONDARY_LIB_NAME "soft_oal.dll"
#else /* Unix defaults otherwise */
#include <dlfcn.h>
    void* _alad_open(const char* path) {
        return dlopen(path, RTLD_LAZY | RTLD_LOCAL);
    }
    _alad_proc _alad_load(void* module, const char* name) {
        return dlsym(module, name);
    }
    void _alad_close(void* module) {
        dlclose(module);
    }
#define _alad_LIB_NAME "libopenal.so"
#define _alad_SECONDARY_LIB_NAME "libopenal.so.1"
#endif /* _WIN32 */



void* _alad_module = NULL;

LPALGETPROCADDRESS _alad_initalloader() {
    _alad_module = _alad_open(_alad_LIB_NAME);
    if(_alad_module == NULL) {
        _alad_module = _alad_open(_alad_SECONDARY_LIB_NAME);
        if(_alad_module == NULL) {
            return NULL;
        }
    }
    return (LPALGETPROCADDRESS) _alad_load(_alad_module, "alGetProcAddress");
}
void _alad_unload_lib() {
    if(_alad_module != NULL) _alad_close(_alad_module);
}




void aladLoadAL(LPALGETPROCADDRESS inital_loader) {
    if(inital_loader != NULL) {
        alGetProcAddress = inital_loader;
    } else if (alGetProcAddress == NULL)  {
        alGetProcAddress = _alad_initalloader();
    }
    _alad_load_al_functions();
    _alad_load_alc_functions(NULL);
}

void aladUpdateALCPointers(ALCdevice *device) {
    _alad_load_alc_functions(device);
}

void aladTerminate() {
    _alad_unload_lib();
}


#else // that is when !defined(ALAD_IMPLEMENTATION)


extern void aladLoadAL(LPALGETPROCADDRESS inital_loader);
extern void aladUpdateALCPointers(ALCdevice *device);
extern void aladTerminate();

//Core AL
extern LPALDOPPLERFACTOR alDopplerFactor;
extern LPALDOPPLERVELOCITY alDopplerVelocity;
extern LPALSPEEDOFSOUND alSpeedOfSound;
extern LPALDISTANCEMODEL alDistanceModel;
extern LPALENABLE alEnable;
extern LPALDISABLE alDisable;
extern LPALISENABLED alIsEnabled;
extern LPALGETSTRING alGetString;
extern LPALGETBOOLEANV alGetBooleanv;
extern LPALGETINTEGERV alGetIntegerv;
extern LPALGETFLOATV alGetFloatv;
extern LPALGETDOUBLEV alGetDoublev;
extern LPALGETBOOLEAN alGetBoolean;
extern LPALGETINTEGER alGetInteger;
extern LPALGETFLOAT alGetFloat;
extern LPALGETDOUBLE alGetDouble;
extern LPALGETERROR alGetError;
extern LPALISEXTENSIONPRESENT alIsExtensionPresent;
extern LPALGETPROCADDRESS alGetProcAddress;
extern LPALGETENUMVALUE alGetEnumValue;
extern LPALLISTENERF alListenerf;
extern LPALLISTENER3F alListener3f;
extern LPALLISTENERFV alListenerfv;
extern LPALLISTENERI alListeneri;
extern LPALLISTENER3I alListener3i;
extern LPALLISTENERIV alListeneriv;
extern LPALGETLISTENERF alGetListenerf;
extern LPALGETLISTENER3F alGetListener3f;
extern LPALGETLISTENERFV alGetListenerfv;
extern LPALGETLISTENERI alGetListeneri;
extern LPALGETLISTENER3I alGetListener3i;
extern LPALGETLISTENERIV alGetListeneriv;
extern LPALGENSOURCES alGenSources;
extern LPALDELETESOURCES alDeleteSources;
extern LPALISSOURCE alIsSource;
extern LPALSOURCEF alSourcef;
extern LPALSOURCE3F alSource3f;
extern LPALSOURCEFV alSourcefv;
extern LPALSOURCEI alSourcei;
extern LPALSOURCE3I alSource3i;
extern LPALSOURCEIV alSourceiv;
extern LPALGETSOURCEF alGetSourcef;
extern LPALGETSOURCE3F alGetSource3f;
extern LPALGETSOURCEFV alGetSourcefv;
extern LPALGETSOURCEI alGetSourcei;
extern LPALGETSOURCE3I alGetSource3i;
extern LPALGETSOURCEIV alGetSourceiv;
extern LPALSOURCEPLAYV alSourcePlayv;
extern LPALSOURCESTOPV alSourceStopv;
extern LPALSOURCEREWINDV alSourceRewindv;
extern LPALSOURCEPAUSEV alSourcePausev;
extern LPALSOURCEPLAY alSourcePlay;
extern LPALSOURCESTOP alSourceStop;
extern LPALSOURCEREWIND alSourceRewind;
extern LPALSOURCEPAUSE alSourcePause;
extern LPALSOURCEQUEUEBUFFERS alSourceQueueBuffers;
extern LPALSOURCEUNQUEUEBUFFERS alSourceUnqueueBuffers;
extern LPALGENBUFFERS alGenBuffers;
extern LPALDELETEBUFFERS alDeleteBuffers;
extern LPALISBUFFER alIsBuffer;
extern LPALBUFFERDATA alBufferData;
extern LPALBUFFERF alBufferf;
extern LPALBUFFER3F alBuffer3f;
extern LPALBUFFERFV alBufferfv;
extern LPALBUFFERI alBufferi;
extern LPALBUFFER3I alBuffer3i;
extern LPALBUFFERIV alBufferiv;
extern LPALGETBUFFERF alGetBufferf;
extern LPALGETBUFFER3F alGetBuffer3f;
extern LPALGETBUFFERFV alGetBufferfv;
extern LPALGETBUFFERI alGetBufferi;
extern LPALGETBUFFER3I alGetBuffer3i;
extern LPALGETBUFFERIV alGetBufferiv;

//EFX
extern LPALGENEFFECTS alGenEffects;
extern LPALDELETEEFFECTS alDeleteEffects;
extern LPALISEFFECT alIsEffect;
extern LPALEFFECTI alEffecti;
extern LPALEFFECTIV alEffectiv;
extern LPALEFFECTF alEffectf;
extern LPALEFFECTFV alEffectfv;
extern LPALGETEFFECTI alGetEffecti;
extern LPALGETEFFECTIV alGetEffectiv;
extern LPALGETEFFECTF alGetEffectf;
extern LPALGETEFFECTFV alGetEffectfv;
extern LPALGENFILTERS alGenFilters;
extern LPALDELETEFILTERS alDeleteFilters;
extern LPALISFILTER alIsFilter;
extern LPALFILTERI alFilteri;
extern LPALFILTERIV alFilteriv;
extern LPALFILTERF alFilterf;
extern LPALFILTERFV alFilterfv;
extern LPALGETFILTERI alGetFilteri;
extern LPALGETFILTERIV alGetFilteriv;
extern LPALGETFILTERF alGetFilterf;
extern LPALGETFILTERFV alGetFilterfv;
extern LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
extern LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
extern LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
extern LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
extern LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
extern LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
extern LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
extern LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
extern LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
extern LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
extern LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

//AL extensions
//AL_EXT_STATIC_BUFFER
extern PFNALBUFFERDATASTATICPROC alBufferDataStatic;
//AL_SOFT_buffer_sub_data
extern PFNALBUFFERSUBDATASOFTPROC alBufferSubDataSOFT;
//AL_EXT_FOLDBACK
extern LPALREQUESTFOLDBACKSTART alRequestFoldbackStart;
extern LPALREQUESTFOLDBACKSTOP alRequestFoldbackStop;
//AL_SOFT_buffer_samples
extern LPALBUFFERSAMPLESSOFT alBufferSamplesSOFT;
extern LPALBUFFERSUBSAMPLESSOFT alBufferSubSamplesSOFT;
extern LPALGETBUFFERSAMPLESSOFT alGetBufferSamplesSOFT;
extern LPALISBUFFERFORMATSUPPORTEDSOFT alIsBufferFormatSupportedSOFT;
//AL_SOFT_source_latency
extern LPALSOURCEDSOFT alSourcedSOFT;
extern LPALSOURCE3DSOFT alSource3dSOFT;
extern LPALSOURCEDVSOFT alSourcedvSOFT;
extern LPALGETSOURCEDSOFT alGetSourcedSOFT;
extern LPALGETSOURCE3DSOFT alGetSource3dSOFT;
extern LPALGETSOURCEDVSOFT alGetSourcedvSOFT;
extern LPALSOURCEI64SOFT alSourcei64SOFT;
extern LPALSOURCE3I64SOFT alSource3i64SOFT;
extern LPALSOURCEI64VSOFT alSourcei64vSOFT;
extern LPALGETSOURCEI64SOFT alGetSourcei64SOFT;
extern LPALGETSOURCE3I64SOFT alGetSource3i64SOFT;
extern LPALGETSOURCEI64VSOFT alGetSourcei64vSOFT;
//AL_SOFT_deferred_updates
extern LPALDEFERUPDATESSOFT alDeferUpdatesSOFT;
extern LPALPROCESSUPDATESSOFT alProcessUpdatesSOFT;
//AL_SOFT_source_resampler
extern LPALGETSTRINGISOFT alGetStringiSOFT;
//AL_SOFT_events
extern LPALEVENTCONTROLSOFT alEventControlSOFT;
extern LPALEVENTCALLBACKSOFT alEventCallbackSOFT;
extern LPALGETPOINTERSOFT alGetPointerSOFT;
extern LPALGETPOINTERVSOFT alGetPointervSOFT;
//AL_SOFT_callback_buffer
extern LPALBUFFERCALLBACKSOFT alBufferCallbackSOFT;
extern LPALGETBUFFERPTRSOFT alGetBufferPtrSOFT;
extern LPALGETBUFFER3PTRSOFT alGetBuffer3PtrSOFT;
extern LPALGETBUFFERPTRVSOFT alGetBufferPtrvSOFT;

//Core ALC
extern LPALCCREATECONTEXT alcCreateContext;
extern LPALCMAKECONTEXTCURRENT alcMakeContextCurrent;
extern LPALCPROCESSCONTEXT alcProcessContext;
extern LPALCSUSPENDCONTEXT alcSuspendContext;
extern LPALCDESTROYCONTEXT alcDestroyContext;
extern LPALCGETCURRENTCONTEXT alcGetCurrentContext;
extern LPALCGETCONTEXTSDEVICE alcGetContextsDevice;
extern LPALCOPENDEVICE alcOpenDevice;
extern LPALCCLOSEDEVICE alcCloseDevice;
extern LPALCGETERROR alcGetError;
extern LPALCISEXTENSIONPRESENT alcIsExtensionPresent;
extern LPALCGETPROCADDRESS alcGetProcAddress;
extern LPALCGETENUMVALUE alcGetEnumValue;
extern LPALCGETSTRING alcGetString;
extern LPALCGETINTEGERV alcGetIntegerv;
extern LPALCCAPTUREOPENDEVICE alcCaptureOpenDevice;
extern LPALCCAPTURECLOSEDEVICE alcCaptureCloseDevice;
extern LPALCCAPTURESTART alcCaptureStart;
extern LPALCCAPTURESTOP alcCaptureStop;
extern LPALCCAPTURESAMPLES alcCaptureSamples;

//ALC extensions
//ALC_EXT_thread_local_context
extern PFNALCSETTHREADCONTEXTPROC alcSetThreadContext;
extern PFNALCGETTHREADCONTEXTPROC alcGetThreadContext;
//ALC_SOFT_loopback
extern LPALCLOOPBACKOPENDEVICESOFT alcLoopbackOpenDeviceSOFT;
extern LPALCISRENDERFORMATSUPPORTEDSOFT alcIsRenderFormatSupportedSOFT;
extern LPALCRENDERSAMPLESSOFT alcRenderSamplesSOFT;
//ALC_SOFT_pause_device
extern LPALCDEVICEPAUSESOFT alcDevicePauseSOFT;
extern LPALCDEVICERESUMESOFT alcDeviceResumeSOFT;
//ALC_SOFT_HRTF
extern LPALCGETSTRINGISOFT alcGetStringiSOFT;
extern LPALCRESETDEVICESOFT alcResetDeviceSOFT;
//ALC_SOFT_device_clock
extern LPALCGETINTEGER64VSOFT alcGetInteger64vSOFT;
//ALC_SOFT_reopen_device
extern LPALCREOPENDEVICESOFT alcReopenDeviceSOFT;

#endif // ALAD_IMPLEMENTATION

#endif // ALAD_H
