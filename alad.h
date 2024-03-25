/*
 *  alad - a glad-like header-only C/C++ OpenAL-loader by Hypatia of Sva. Look for the current version at: https://github.com/hypatia-of-sva/alad
 *
 *  Dependencies for compilation:
 *      - Kernel32.dll on Windows and -ldl on Unix for loading DLLs / shared libraries.
 *      - Current OpenAL header files; download here: https://github.com/kcat/openal-soft/tree/master/include
 * 
 *  Usage:
 *  
 *  Include this file once with
 *
 *          #define ALAD_IMPLEMENTATION
 *
 *  to define the functions and global function pointers, and include this file without this symbol defined in all other translation units.
 * 
 *  This header loads in all OpenAL symbols except the functions, by loading in the AL headers with AL_NO_PROTOTYPES and ALC_NO_PROTOTYPES defined.
 *  In order for this to work, you need up to date header files, download them from the master branch of openal-soft:
 *      https://github.com/kcat/openal-soft/tree/master/include
 *  (Currently to 1.23.1)
 *  alad can't currently check on whether or not the headers work properly, but compilation will fail due to double definition or undefined types.
 *  Keep that in mind if you have compilation issues and put those headers under <AL/alext.h> and <AL/efx-presets.h> (the others are included in alext.h).
 *
 *  There are two interfaces to initialize the API, the (recomended) simplified interface and the manual interface with more options. 
 *
 *  The simplified interface initializes the library first with
 *  
 *          aladLoadAL();
 *  
 *  which loads in all the core functions from the shared library. This should be enough to select a device and create a context. Once you've done that, call
 *
 *          aladUpdateAL();
 *
 *  which will load all extensions with the current context and its device. If an extension is not present, its functions will be NULL, check for that.
 *  The ALC functions are only valid for the current context's device (and technically the AL functions also for the context). If you change the device,
 *  call aladUpdateAL(); again. (If you make another context current, you can also do that, but most implementations of OpenAL will not differentiate
 *  between contexts on the same device for functionality, although it is allowed by the specification.)
 *
 *  Once you're done using OpenAL, terminate the library with
 *
 *          aladTerminate();
 *  
 *  this will unload the shared library.
 *  The library should be named
 *      - OpenAL32.dll / soft_oal.dll on Windows
 *      - libopenal.so.1 / libopenal.so on Linux/BSD
 *      - libopenal.1.dylib / libopenal.dylib on Mac OS.
 *  and should be on path for LoadLibraryA / dlopen.
 *
 *
 *
 *  Manual interface (not recommended, only use for troubleshooting like debugging the DLL / shared object):
 *
 *  The manual interface initializes the function pointers first with
 * 
 *          aladLoadALFromLoaderFunction(NULL);
 * 
 *  for default initialization, or with
 * 
 *          aladLoadALFromLoaderFunction(my_alGetProcAddress);
 * 
 *  where my_alGetProcAddress is a loader function of type LPALGETPROCADDRESS for custom initalization, to load all the function via alGetProcAddress.
 *
 *  Default initialization will pull in OpenAL32.dll / soft_oal.dll on Windows or libopenal.so.1 / libopenal.so on Unix respectively,
 *  and libopenal.1.dylib / libopenal.dylib on Mac OS (yet untested).
 *  Make sure one of these dynamic libraries are on path for LoadLibraryA / dlopen, change the code below in alad_open_ or provide your own function loader.
 *  The shared library will only be loaded once, you can call "aladLoadALFromLoaderFunction(NULL)" as often as you want
 *  to reload the pointers from the loaded shared library.
 *
 *  If you're unsure about loading with a function loader, intialize with
 *
 *          aladLoadALContextFree(AL_FALSE);
 *
 *  to load function pointers directly from the shared library. This only works for default initialization.
 *  This will however not load all function pointers, but only those necessary to create a context.
 *  Those are the core ALC functions and all Core AL functions not relating to buffers, listeners, sources,
 *  and also not alDopplerFactor, alDopplerVelocity, alSpeedOfSound and alDistanceModel.
 *  However, if you change the parameter to anything else (like AL_TRUE), all core AL and ALC functions will be loaded from the shared library.
 *
 *  If you want to use your own library, not available in default initialization, and want to load directly from it, you need to write a wrapper of type
 *  LPALGETPROCADDRESS of the form my_alGetProcAddress("[al-function]") = dlsym(my_lib, "[al-function]"). aladLoadALContextFree is not used here.
 *
 *  These aladLoad functions only initialize the Core API. To load the extensions you have to update the function pointers.
 *
 *  You can update those via the ALCcontext* context through
 *
 *          aladUpdateALPointers(context, AL_FALSE);
 *          aladUpdateALCPointersFromContext(context, AL_FALSE);
 *
 *  which will load all AL and ALC functions, including extensions, via alGetProcAddress for the specific context
 *  by switching the current context temporarily. It will just use the current context, if the parameter is NULL.
 *  If you replace AL_FALSE with anything else (AL_TRUE makes the most sense) this will only load the extensions,
 *  and the core function pointers will remain unchanged
 *
 *  Update ALC pointers to those loaded with a specific ALCdevice* device with
 * 
 *          aladUpdateALCPointersFromDevice(device, AL_FALSE);
 *
 *  If you want to remove this reference to the device, reload them to the nonspecific pointers by calling "aladUpdateALCPointersFromContext" again.
 *  or by calling "aladLoadALContextFree()" if you need them without reference to any context.
 *  Calling "aladLoadALFromLoaderFunction" again won't do anything different from "aladUpdateALCPointersFromContext",
 *  since both call alGetProcAddress and are therefore just dependent on driver state.
 *  The last parameter again makes sure to reload the core, change the value to disable that.
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
 *          strings in the internal alad_load_alc_functions_ function.
 * 
 */


#pragma once

/* only include alad if the OpenAL headers are not used, because we include them again, and they may have been included with prototypes */
#if !(defined(ALAD_H)) && !(defined(AL_AL_H)) && !(defined(AL_ALC_H)) && !(defined(AL_ALEXT_H)) && !(defined(AL_EFX_H)) && !(defined(EFX_PRESETS_H))
#define ALAD_H

/* revision date */
#define ALAD_HEADER_REVISION 0x20240323

#ifndef __cplusplus
#ifndef nullptr
#define nullptr NULL
#endif
#endif

#ifdef __cplusplus
#define REINTERPRET_CAST(X,Y) reinterpret_cast<X>(Y)
#else
#define REINTERPRET_CAST(X,Y) ((X) Y)
#endif


#if defined(__cplusplus)
extern "C" {
#endif


#define AL_NO_PROTOTYPES
#define ALC_NO_PROTOTYPES
#include <AL/alext.h>
#include <AL/efx-presets.h>
/* currently it can't be checked whether or not the headers work properly. the old prototypes will be shadowed if present */



/* Public Interface: */


/* simplified Interface */
extern void                             aladLoadAL();
extern void                             aladUpdateAL();

/* manual interface */
extern void                             aladLoadALContextFree (ALboolean loadAll);
extern void                             aladLoadALFromLoaderFunction (LPALGETPROCADDRESS inital_loader);
extern void                             aladUpdateALPointers (ALCcontext *context, ALboolean extensionsOnly);
extern void                             aladUpdateALCPointersFromContext (ALCcontext *context, ALboolean extensionsOnly);
extern void                             aladUpdateALCPointersFromDevice (ALCdevice *device, ALboolean extensionsOnly);

/* shared function */
extern void                             aladTerminate();

/* Core AL */
extern LPALDOPPLERFACTOR                alad_alDopplerFactor;
extern LPALDOPPLERVELOCITY              alad_alDopplerVelocity;
extern LPALSPEEDOFSOUND                 alad_alSpeedOfSound;
extern LPALDISTANCEMODEL                alad_alDistanceModel;
extern LPALENABLE                       alad_alEnable;
extern LPALDISABLE                      alad_alDisable;
extern LPALISENABLED                    alad_alIsEnabled;
extern LPALGETSTRING                    alad_alGetString;
extern LPALGETBOOLEANV                  alad_alGetBooleanv;
extern LPALGETINTEGERV                  alad_alGetIntegerv;
extern LPALGETFLOATV                    alad_alGetFloatv;
extern LPALGETDOUBLEV                   alad_alGetDoublev;
extern LPALGETBOOLEAN                   alad_alGetBoolean;
extern LPALGETINTEGER                   alad_alGetInteger;
extern LPALGETFLOAT                     alad_alGetFloat;
extern LPALGETDOUBLE                    alad_alGetDouble;
extern LPALGETERROR                     alad_alGetError;
extern LPALISEXTENSIONPRESENT           alad_alIsExtensionPresent;
extern LPALGETPROCADDRESS               alad_alGetProcAddress;
extern LPALGETENUMVALUE                 alad_alGetEnumValue;
extern LPALLISTENERF                    alad_alListenerf;
extern LPALLISTENER3F                   alad_alListener3f;
extern LPALLISTENERFV                   alad_alListenerfv;
extern LPALLISTENERI                    alad_alListeneri;
extern LPALLISTENER3I                   alad_alListener3i;
extern LPALLISTENERIV                   alad_alListeneriv;
extern LPALGETLISTENERF                 alad_alGetListenerf;
extern LPALGETLISTENER3F                alad_alGetListener3f;
extern LPALGETLISTENERFV                alad_alGetListenerfv;
extern LPALGETLISTENERI                 alad_alGetListeneri;
extern LPALGETLISTENER3I                alad_alGetListener3i;
extern LPALGETLISTENERIV                alad_alGetListeneriv;
extern LPALGENSOURCES                   alad_alGenSources;
extern LPALDELETESOURCES                alad_alDeleteSources;
extern LPALISSOURCE                     alad_alIsSource;
extern LPALSOURCEF                      alad_alSourcef;
extern LPALSOURCE3F                     alad_alSource3f;
extern LPALSOURCEFV                     alad_alSourcefv;
extern LPALSOURCEI                      alad_alSourcei;
extern LPALSOURCE3I                     alad_alSource3i;
extern LPALSOURCEIV                     alad_alSourceiv;
extern LPALGETSOURCEF                   alad_alGetSourcef;
extern LPALGETSOURCE3F                  alad_alGetSource3f;
extern LPALGETSOURCEFV                  alad_alGetSourcefv;
extern LPALGETSOURCEI                   alad_alGetSourcei;
extern LPALGETSOURCE3I                  alad_alGetSource3i;
extern LPALGETSOURCEIV                  alad_alGetSourceiv;
extern LPALSOURCEPLAYV                  alad_alSourcePlayv;
extern LPALSOURCESTOPV                  alad_alSourceStopv;
extern LPALSOURCEREWINDV                alad_alSourceRewindv;
extern LPALSOURCEPAUSEV                 alad_alSourcePausev;
extern LPALSOURCEPLAY                   alad_alSourcePlay;
extern LPALSOURCESTOP                   alad_alSourceStop;
extern LPALSOURCEREWIND                 alad_alSourceRewind;
extern LPALSOURCEPAUSE                  alad_alSourcePause;
extern LPALSOURCEQUEUEBUFFERS           alad_alSourceQueueBuffers;
extern LPALSOURCEUNQUEUEBUFFERS         alad_alSourceUnqueueBuffers;
extern LPALGENBUFFERS                   alad_alGenBuffers;
extern LPALDELETEBUFFERS                alad_alDeleteBuffers;
extern LPALISBUFFER                     alad_alIsBuffer;
extern LPALBUFFERDATA                   alad_alBufferData;
extern LPALBUFFERF                      alad_alBufferf;
extern LPALBUFFER3F                     alad_alBuffer3f;
extern LPALBUFFERFV                     alad_alBufferfv;
extern LPALBUFFERI                      alad_alBufferi;
extern LPALBUFFER3I                     alad_alBuffer3i;
extern LPALBUFFERIV                     alad_alBufferiv;
extern LPALGETBUFFERF                   alad_alGetBufferf;
extern LPALGETBUFFER3F                  alad_alGetBuffer3f;
extern LPALGETBUFFERFV                  alad_alGetBufferfv;
extern LPALGETBUFFERI                   alad_alGetBufferi;
extern LPALGETBUFFER3I                  alad_alGetBuffer3i;
extern LPALGETBUFFERIV                  alad_alGetBufferiv;

/* EFX */
extern LPALGENEFFECTS                   alad_alGenEffects;
extern LPALDELETEEFFECTS                alad_alDeleteEffects;
extern LPALISEFFECT                     alad_alIsEffect;
extern LPALEFFECTI                      alad_alEffecti;
extern LPALEFFECTIV                     alad_alEffectiv;
extern LPALEFFECTF                      alad_alEffectf;
extern LPALEFFECTFV                     alad_alEffectfv;
extern LPALGETEFFECTI                   alad_alGetEffecti;
extern LPALGETEFFECTIV                  alad_alGetEffectiv;
extern LPALGETEFFECTF                   alad_alGetEffectf;
extern LPALGETEFFECTFV                  alad_alGetEffectfv;
extern LPALGENFILTERS                   alad_alGenFilters;
extern LPALDELETEFILTERS                alad_alDeleteFilters;
extern LPALISFILTER                     alad_alIsFilter;
extern LPALFILTERI                      alad_alFilteri;
extern LPALFILTERIV                     alad_alFilteriv;
extern LPALFILTERF                      alad_alFilterf;
extern LPALFILTERFV                     alad_alFilterfv;
extern LPALGETFILTERI                   alad_alGetFilteri;
extern LPALGETFILTERIV                  alad_alGetFilteriv;
extern LPALGETFILTERF                   alad_alGetFilterf;
extern LPALGETFILTERFV                  alad_alGetFilterfv;
extern LPALGENAUXILIARYEFFECTSLOTS      alad_alGenAuxiliaryEffectSlots;
extern LPALDELETEAUXILIARYEFFECTSLOTS   alad_alDeleteAuxiliaryEffectSlots;
extern LPALISAUXILIARYEFFECTSLOT        alad_alIsAuxiliaryEffectSlot;
extern LPALAUXILIARYEFFECTSLOTI         alad_alAuxiliaryEffectSloti;
extern LPALAUXILIARYEFFECTSLOTIV        alad_alAuxiliaryEffectSlotiv;
extern LPALAUXILIARYEFFECTSLOTF         alad_alAuxiliaryEffectSlotf;
extern LPALAUXILIARYEFFECTSLOTFV        alad_alAuxiliaryEffectSlotfv;
extern LPALGETAUXILIARYEFFECTSLOTI      alad_alGetAuxiliaryEffectSloti;
extern LPALGETAUXILIARYEFFECTSLOTIV     alad_alGetAuxiliaryEffectSlotiv;
extern LPALGETAUXILIARYEFFECTSLOTF      alad_alGetAuxiliaryEffectSlotf;
extern LPALGETAUXILIARYEFFECTSLOTFV     alad_alGetAuxiliaryEffectSlotfv;

/* AL extensions */
/* AL_EXT_STATIC_BUFFER */
extern PFNALBUFFERDATASTATICPROC        alad_alBufferDataStatic;
/* AL_SOFT_buffer_sub_data */
extern PFNALBUFFERSUBDATASOFTPROC       alad_alBufferSubDataSOFT;
/* AL_EXT_FOLDBACK */
extern LPALREQUESTFOLDBACKSTART         alad_alRequestFoldbackStart;
extern LPALREQUESTFOLDBACKSTOP          alad_alRequestFoldbackStop;
/* AL_SOFT_buffer_samples */
extern LPALBUFFERSAMPLESSOFT            alad_alBufferSamplesSOFT;
extern LPALBUFFERSUBSAMPLESSOFT         alad_alBufferSubSamplesSOFT;
extern LPALGETBUFFERSAMPLESSOFT         alad_alGetBufferSamplesSOFT;
extern LPALISBUFFERFORMATSUPPORTEDSOFT  alad_alIsBufferFormatSupportedSOFT;
/* AL_SOFT_source_latency */
extern LPALSOURCEDSOFT                  alad_alSourcedSOFT;
extern LPALSOURCE3DSOFT                 alad_alSource3dSOFT;
extern LPALSOURCEDVSOFT                 alad_alSourcedvSOFT;
extern LPALGETSOURCEDSOFT               alad_alGetSourcedSOFT;
extern LPALGETSOURCE3DSOFT              alad_alGetSource3dSOFT;
extern LPALGETSOURCEDVSOFT              alad_alGetSourcedvSOFT;
extern LPALSOURCEI64SOFT                alad_alSourcei64SOFT;
extern LPALSOURCE3I64SOFT               alad_alSource3i64SOFT;
extern LPALSOURCEI64VSOFT               alad_alSourcei64vSOFT;
extern LPALGETSOURCEI64SOFT             alad_alGetSourcei64SOFT;
extern LPALGETSOURCE3I64SOFT            alad_alGetSource3i64SOFT;
extern LPALGETSOURCEI64VSOFT            alad_alGetSourcei64vSOFT;
/* AL_SOFT_deferred_updates */
extern LPALDEFERUPDATESSOFT             alad_alDeferUpdatesSOFT;
extern LPALPROCESSUPDATESSOFT           alad_alProcessUpdatesSOFT;
/* AL_SOFT_source_resampler */
extern LPALGETSTRINGISOFT               alad_alGetStringiSOFT;
/* AL_SOFT_events */
extern LPALEVENTCONTROLSOFT             alad_alEventControlSOFT;
extern LPALEVENTCALLBACKSOFT            alad_alEventCallbackSOFT;
extern LPALGETPOINTERSOFT               alad_alGetPointerSOFT;
extern LPALGETPOINTERVSOFT              alad_alGetPointervSOFT;
/* AL_SOFT_callback_buffer */
extern LPALBUFFERCALLBACKSOFT           alad_alBufferCallbackSOFT;
extern LPALGETBUFFERPTRSOFT             alad_alGetBufferPtrSOFT;
extern LPALGETBUFFER3PTRSOFT            alad_alGetBuffer3PtrSOFT;
extern LPALGETBUFFERPTRVSOFT            alad_alGetBufferPtrvSOFT;
/* AL_SOFT_source_start_delay */
extern LPALSOURCEPLAYATTIMESOFT         alad_alSourcePlayAtTimeSOFT;
extern LPALSOURCEPLAYATTIMEVSOFT        alad_alSourcePlayAtTimevSOFT;
/* AL_EXT_debug */
extern LPALDEBUGMESSAGECALLBACKEXT      alad_alDebugMessageCallbackEXT;
extern LPALDEBUGMESSAGEINSERTEXT        alad_alDebugMessageInsertEXT;
extern LPALDEBUGMESSAGECONTROLEXT       alad_alDebugMessageControlEXT;
extern LPALPUSHDEBUGGROUPEXT            alad_alPushDebugGroupEXT;
extern LPALPOPDEBUGGROUPEXT             alad_alPopDebugGroupEXT;
extern LPALGETDEBUGMESSAGELOGEXT        alad_alGetDebugMessageLogEXT;
extern LPALOBJECTLABELEXT               alad_alObjectLabelEXT;
extern LPALGETOBJECTLABELEXT            alad_alGetObjectLabelEXT;

/* Core ALC */
extern LPALCCREATECONTEXT               alad_alcCreateContext;
extern LPALCMAKECONTEXTCURRENT          alad_alcMakeContextCurrent;
extern LPALCPROCESSCONTEXT              alad_alcProcessContext;
extern LPALCSUSPENDCONTEXT              alad_alcSuspendContext;
extern LPALCDESTROYCONTEXT              alad_alcDestroyContext;
extern LPALCGETCURRENTCONTEXT           alad_alcGetCurrentContext;
extern LPALCGETCONTEXTSDEVICE           alad_alcGetContextsDevice;
extern LPALCOPENDEVICE                  alad_alcOpenDevice;
extern LPALCCLOSEDEVICE                 alad_alcCloseDevice;
extern LPALCGETERROR                    alad_alcGetError;
extern LPALCISEXTENSIONPRESENT          alad_alcIsExtensionPresent;
extern LPALCGETPROCADDRESS              alad_alcGetProcAddress;
extern LPALCGETENUMVALUE                alad_alcGetEnumValue;
extern LPALCGETSTRING                   alad_alcGetString;
extern LPALCGETINTEGERV                 alad_alcGetIntegerv;
extern LPALCCAPTUREOPENDEVICE           alad_alcCaptureOpenDevice;
extern LPALCCAPTURECLOSEDEVICE          alad_alcCaptureCloseDevice;
extern LPALCCAPTURESTART                alad_alcCaptureStart;
extern LPALCCAPTURESTOP                 alad_alcCaptureStop;
extern LPALCCAPTURESAMPLES              alad_alcCaptureSamples;

/* ALC extensions */
/* ALC_EXT_thread_local_context */
extern PFNALCSETTHREADCONTEXTPROC       alad_alcSetThreadContext;
extern PFNALCGETTHREADCONTEXTPROC       alad_alcGetThreadContext;
/* ALC_SOFT_loopback */
extern LPALCLOOPBACKOPENDEVICESOFT      alad_alcLoopbackOpenDeviceSOFT;
extern LPALCISRENDERFORMATSUPPORTEDSOFT alad_alcIsRenderFormatSupportedSOFT;
extern LPALCRENDERSAMPLESSOFT           alad_alcRenderSamplesSOFT;
/* ALC_SOFT_pause_device */
extern LPALCDEVICEPAUSESOFT             alad_alcDevicePauseSOFT;
extern LPALCDEVICERESUMESOFT            alad_alcDeviceResumeSOFT;
/* ALC_SOFT_HRTF */
extern LPALCGETSTRINGISOFT              alad_alcGetStringiSOFT;
extern LPALCRESETDEVICESOFT             alad_alcResetDeviceSOFT;
/* ALC_SOFT_device_clock */
extern LPALCGETINTEGER64VSOFT           alad_alcGetInteger64vSOFT;
/* ALC_SOFT_reopen_device */
extern LPALCREOPENDEVICESOFT            alad_alcReopenDeviceSOFT;
/* ALC_SOFT_system_events */
extern LPALCEVENTISSUPPORTEDSOFT        alad_alcEventIsSupportedSOFT;
extern LPALCEVENTCONTROLSOFT            alad_alcEventControlSOFT;
extern LPALCEVENTCALLBACKSOFT           alad_alcEventCallbackSOFT;






/* Core AL */
#define alDopplerFactor                 alad_alDopplerFactor
#define alDopplerVelocity               alad_alDopplerVelocity
#define alSpeedOfSound                  alad_alSpeedOfSound
#define alDistanceModel                 alad_alDistanceModel
#define alEnable                        alad_alEnable
#define alDisable                       alad_alDisable
#define alIsEnabled                     alad_alIsEnabled
#define alGetString                     alad_alGetString
#define alGetBooleanv                   alad_alGetBooleanv
#define alGetIntegerv                   alad_alGetIntegerv
#define alGetFloatv                     alad_alGetFloatv
#define alGetDoublev                    alad_alGetDoublev
#define alGetBoolean                    alad_alGetBoolean
#define alGetInteger                    alad_alGetInteger
#define alGetFloat                      alad_alGetFloat
#define alGetDouble                     alad_alGetDouble
#define alGetError                      alad_alGetError
#define alIsExtensionPresent            alad_alIsExtensionPresent
#define alGetProcAddress                alad_alGetProcAddress
#define alGetEnumValue                  alad_alGetEnumValue
#define alListenerf                     alad_alListenerf
#define alListener3f                    alad_alListener3f
#define alListenerfv                    alad_alListenerfv
#define alListeneri                     alad_alListeneri
#define alListener3i                    alad_alListener3i
#define alListeneriv                    alad_alListeneriv
#define alGetListenerf                  alad_alGetListenerf
#define alGetListener3f                 alad_alGetListener3f
#define alGetListenerfv                 alad_alGetListenerfv
#define alGetListeneri                  alad_alGetListeneri
#define alGetListener3i                 alad_alGetListener3i
#define alGetListeneriv                 alad_alGetListeneriv
#define alGenSources                    alad_alGenSources
#define alDeleteSources                 alad_alDeleteSources
#define alIsSource                      alad_alIsSource
#define alSourcef                       alad_alSourcef
#define alSource3f                      alad_alSource3f
#define alSourcefv                      alad_alSourcefv
#define alSourcei                       alad_alSourcei
#define alSource3i                      alad_alSource3i
#define alSourceiv                      alad_alSourceiv
#define alGetSourcef                    alad_alGetSourcef
#define alGetSource3f                   alad_alGetSource3f
#define alGetSourcefv                   alad_alGetSourcefv
#define alGetSourcei                    alad_alGetSourcei
#define alGetSource3i                   alad_alGetSource3i
#define alGetSourceiv                   alad_alGetSourceiv
#define alSourcePlayv                   alad_alSourcePlayv
#define alSourceStopv                   alad_alSourceStopv
#define alSourceRewindv                 alad_alSourceRewindv
#define alSourcePausev                  alad_alSourcePausev
#define alSourcePlay                    alad_alSourcePlay
#define alSourceStop                    alad_alSourceStop
#define alSourceRewind                  alad_alSourceRewind
#define alSourcePause                   alad_alSourcePause
#define alSourceQueueBuffers            alad_alSourceQueueBuffers
#define alSourceUnqueueBuffers          alad_alSourceUnqueueBuffers
#define alGenBuffers                    alad_alGenBuffers
#define alDeleteBuffers                 alad_alDeleteBuffers
#define alIsBuffer                      alad_alIsBuffer
#define alBufferData                    alad_alBufferData
#define alBufferf                       alad_alBufferf
#define alBuffer3f                      alad_alBuffer3f
#define alBufferfv                      alad_alBufferfv
#define alBufferi                       alad_alBufferi
#define alBuffer3i                      alad_alBuffer3i
#define alBufferiv                      alad_alBufferiv
#define alGetBufferf                    alad_alGetBufferf
#define alGetBuffer3f                   alad_alGetBuffer3f
#define alGetBufferfv                   alad_alGetBufferfv
#define alGetBufferi                    alad_alGetBufferi
#define alGetBuffer3i                   alad_alGetBuffer3i
#define alGetBufferiv                   alad_alGetBufferiv

/* EFX */
#define alGenEffects                    alad_alGenEffects
#define alDeleteEffects                 alad_alDeleteEffects
#define alIsEffect                      alad_alIsEffect
#define alEffecti                       alad_alEffecti
#define alEffectiv                      alad_alEffectiv
#define alEffectf                       alad_alEffectf
#define alEffectfv                      alad_alEffectfv
#define alGetEffecti                    alad_alGetEffecti
#define alGetEffectiv                   alad_alGetEffectiv
#define alGetEffectf                    alad_alGetEffectf
#define alGetEffectfv                   alad_alGetEffectfv
#define alGenFilters                    alad_alGenFilters
#define alDeleteFilters                 alad_alDeleteFilters
#define alIsFilter                      alad_alIsFilter
#define alFilteri                       alad_alFilteri
#define alFilteriv                      alad_alFilteriv
#define alFilterf                       alad_alFilterf
#define alFilterfv                      alad_alFilterfv
#define alGetFilteri                    alad_alGetFilteri
#define alGetFilteriv                   alad_alGetFilteriv
#define alGetFilterf                    alad_alGetFilterf
#define alGetFilterfv                   alad_alGetFilterfv
#define alGenAuxiliaryEffectSlots       alad_alGenAuxiliaryEffectSlots
#define alDeleteAuxiliaryEffectSlots    alad_alDeleteAuxiliaryEffectSlots
#define alIsAuxiliaryEffectSlot         alad_alIsAuxiliaryEffectSlot
#define alAuxiliaryEffectSloti          alad_alAuxiliaryEffectSloti
#define alAuxiliaryEffectSlotiv         alad_alAuxiliaryEffectSlotiv
#define alAuxiliaryEffectSlotf          alad_alAuxiliaryEffectSlotf
#define alAuxiliaryEffectSlotfv         alad_alAuxiliaryEffectSlotfv
#define alGetAuxiliaryEffectSloti       alad_alGetAuxiliaryEffectSloti
#define alGetAuxiliaryEffectSlotiv      alad_alGetAuxiliaryEffectSlotiv
#define alGetAuxiliaryEffectSlotf       alad_alGetAuxiliaryEffectSlotf
#define alGetAuxiliaryEffectSlotfv      alad_alGetAuxiliaryEffectSlotfv

/* AL extensions */
/* AL_EXT_STATIC_BUFFER */
#define alBufferDataStatic              alad_alBufferDataStatic
/* AL_SOFT_buffer_sub_data */
#define alBufferSubDataSOFT             alad_alBufferSubDataSOFT
/* AL_EXT_FOLDBACK */
#define alRequestFoldbackStart          alad_alRequestFoldbackStart
#define alRequestFoldbackStop           alad_alRequestFoldbackStop
/* AL_SOFT_buffer_samples */
#define alBufferSamplesSOFT             alad_alBufferSamplesSOFT
#define alBufferSubSamplesSOFT          alad_alBufferSubSamplesSOFT
#define alGetBufferSamplesSOFT          alad_alGetBufferSamplesSOFT
#define alIsBufferFormatSupportedSOFT   alad_alIsBufferFormatSupportedSOFT
/* AL_SOFT_source_latency */
#define alSourcedSOFT                   alad_alSourcedSOFT
#define alSource3dSOFT                  alad_alSource3dSOFT
#define alSourcedvSOFT                  alad_alSourcedvSOFT
#define alGetSourcedSOFT                alad_alGetSourcedSOFT
#define alGetSource3dSOFT               alad_alGetSource3dSOFT
#define alGetSourcedvSOFT               alad_alGetSourcedvSOFT
#define alSourcei64SOFT                 alad_alSourcei64SOFT
#define alSource3i64SOFT                alad_alSource3i64SOFT
#define alSourcei64vSOFT                alad_alSourcei64vSOFT
#define alGetSourcei64SOFT              alad_alGetSourcei64SOFT
#define alGetSource3i64SOFT             alad_alGetSource3i64SOFT
#define alGetSourcei64vSOFT             alad_alGetSourcei64vSOFT
/* AL_SOFT_deferred_updates */
#define alDeferUpdatesSOFT              alad_alDeferUpdatesSOFT
#define alProcessUpdatesSOFT            alad_alProcessUpdatesSOFT
/* AL_SOFT_source_resampler */
#define alGetStringiSOFT                alad_alGetStringiSOFT
/* AL_SOFT_events */
#define alEventControlSOFT              alad_alEventControlSOFT
#define alEventCallbackSOFT             alad_alEventCallbackSOFT
#define alGetPointerSOFT                alad_alGetPointerSOFT
#define alGetPointervSOFT               alad_alGetPointervSOFT
/* AL_SOFT_callback_buffer */
#define alBufferCallbackSOFT            alad_alBufferCallbackSOFT
#define alGetBufferPtrSOFT              alad_alGetBufferPtrSOFT
#define alGetBuffer3PtrSOFT             alad_alGetBuffer3PtrSOFT
#define alGetBufferPtrvSOFT             alad_alGetBufferPtrvSOFT
/* AL_SOFT_source_start_delay */
#define alSourcePlayAtTimeSOFT          alad_alSourcePlayAtTimeSOFT
#define alSourcePlayAtTimevSOFT         alad_alSourcePlayAtTimevSOFT
/* AL_EXT_debug */
#define alDebugMessageCallbackEXT       alad_alDebugMessageCallbackEXT;
#define alDebugMessageInsertEXT         alad_alDebugMessageInsertEXT;
#define alDebugMessageControlEXT        alad_alDebugMessageControlEXT;
#define alPushDebugGroupEXT             alad_alPushDebugGroupEXT;
#define alPopDebugGroupEXT              alad_alPopDebugGroupEXT;
#define alGetDebugMessageLogEXT         alad_alGetDebugMessageLogEXT;
#define alObjectLabelEXT                alad_alObjectLabelEXT;
#define alGetObjectLabelEXT             alad_alGetObjectLabelEXT;

/* Core ALC */
#define alcCreateContext                alad_alcCreateContext
#define alcMakeContextCurrent           alad_alcMakeContextCurrent
#define alcProcessContext               alad_alcProcessContext
#define alcSuspendContext               alad_alcSuspendContext
#define alcDestroyContext               alad_alcDestroyContext
#define alcGetCurrentContext            alad_alcGetCurrentContext
#define alcGetContextsDevice            alad_alcGetContextsDevice
#define alcOpenDevice                   alad_alcOpenDevice
#define alcCloseDevice                  alad_alcCloseDevice
#define alcGetError                     alad_alcGetError
#define alcIsExtensionPresent           alad_alcIsExtensionPresent
#define alcGetProcAddress               alad_alcGetProcAddress
#define alcGetEnumValue                 alad_alcGetEnumValue
#define alcGetString                    alad_alcGetString
#define alcGetIntegerv                  alad_alcGetIntegerv
#define alcCaptureOpenDevice            alad_alcCaptureOpenDevice
#define alcCaptureCloseDevice           alad_alcCaptureCloseDevice
#define alcCaptureStart                 alad_alcCaptureStart
#define alcCaptureStop                  alad_alcCaptureStop
#define alcCaptureSamples               alad_alcCaptureSamples

/* ALC extensions */
/* ALC_EXT_thread_local_context */
#define alcSetThreadContext             alad_alcSetThreadContext
#define alcGetThreadContext             alad_alcGetThreadContext
/* ALC_SOFT_loopback */
#define alcLoopbackOpenDeviceSOFT       alad_alcLoopbackOpenDeviceSOFT
#define alcIsRenderFormatSupportedSOFT  alad_alcIsRenderFormatSupportedSOFT
#define alcRenderSamplesSOFT            alad_alcRenderSamplesSOFT
/* ALC_SOFT_pause_device */
#define alcDevicePauseSOFT              alad_alcDevicePauseSOFT
#define alcDeviceResumeSOFT             alad_alcDeviceResumeSOFT
/* ALC_SOFT_HRTF */
#define alcGetStringiSOFT               alad_alcGetStringiSOFT
#define alcResetDeviceSOFT              alad_alcResetDeviceSOFT
/* ALC_SOFT_device_clock */
#define alcGetInteger64vSOFT            alad_alcGetInteger64vSOFT
/* ALC_SOFT_reopen_device */
#define alcReopenDeviceSOFT             alad_alcReopenDeviceSOFT
/* ALC_SOFT_system_events */
#define alcEventIsSupportedSOFT         alad_alcEventIsSupportedSOFT;
#define alcEventControlSOFT             alad_alcEventControlSOFT;
#define alcEventCallbackSOFT            alad_alcEventCallbackSOFT;










#ifdef ALAD_IMPLEMENTATION

/* Core AL */
LPALDOPPLERFACTOR                alad_alDopplerFactor                = nullptr;
LPALDOPPLERVELOCITY              alad_alDopplerVelocity              = nullptr;
LPALSPEEDOFSOUND                 alad_alSpeedOfSound                 = nullptr;
LPALDISTANCEMODEL                alad_alDistanceModel                = nullptr;
LPALENABLE                       alad_alEnable                       = nullptr;
LPALDISABLE                      alad_alDisable                      = nullptr;
LPALISENABLED                    alad_alIsEnabled                    = nullptr;
LPALGETSTRING                    alad_alGetString                    = nullptr;
LPALGETBOOLEANV                  alad_alGetBooleanv                  = nullptr;
LPALGETINTEGERV                  alad_alGetIntegerv                  = nullptr;
LPALGETFLOATV                    alad_alGetFloatv                    = nullptr;
LPALGETDOUBLEV                   alad_alGetDoublev                   = nullptr;
LPALGETBOOLEAN                   alad_alGetBoolean                   = nullptr;
LPALGETINTEGER                   alad_alGetInteger                   = nullptr;
LPALGETFLOAT                     alad_alGetFloat                     = nullptr;
LPALGETDOUBLE                    alad_alGetDouble                    = nullptr;
LPALGETERROR                     alad_alGetError                     = nullptr;
LPALISEXTENSIONPRESENT           alad_alIsExtensionPresent           = nullptr;
LPALGETPROCADDRESS               alad_alGetProcAddress               = nullptr;
LPALGETENUMVALUE                 alad_alGetEnumValue                 = nullptr;
LPALLISTENERF                    alad_alListenerf                    = nullptr;
LPALLISTENER3F                   alad_alListener3f                   = nullptr;
LPALLISTENERFV                   alad_alListenerfv                   = nullptr;
LPALLISTENERI                    alad_alListeneri                    = nullptr;
LPALLISTENER3I                   alad_alListener3i                   = nullptr;
LPALLISTENERIV                   alad_alListeneriv                   = nullptr;
LPALGETLISTENERF                 alad_alGetListenerf                 = nullptr;
LPALGETLISTENER3F                alad_alGetListener3f                = nullptr;
LPALGETLISTENERFV                alad_alGetListenerfv                = nullptr;
LPALGETLISTENERI                 alad_alGetListeneri                 = nullptr;
LPALGETLISTENER3I                alad_alGetListener3i                = nullptr;
LPALGETLISTENERIV                alad_alGetListeneriv                = nullptr;
LPALGENSOURCES                   alad_alGenSources                   = nullptr;
LPALDELETESOURCES                alad_alDeleteSources                = nullptr;
LPALISSOURCE                     alad_alIsSource                     = nullptr;
LPALSOURCEF                      alad_alSourcef                      = nullptr;
LPALSOURCE3F                     alad_alSource3f                     = nullptr;
LPALSOURCEFV                     alad_alSourcefv                     = nullptr;
LPALSOURCEI                      alad_alSourcei                      = nullptr;
LPALSOURCE3I                     alad_alSource3i                     = nullptr;
LPALSOURCEIV                     alad_alSourceiv                     = nullptr;
LPALGETSOURCEF                   alad_alGetSourcef                   = nullptr;
LPALGETSOURCE3F                  alad_alGetSource3f                  = nullptr;
LPALGETSOURCEFV                  alad_alGetSourcefv                  = nullptr;
LPALGETSOURCEI                   alad_alGetSourcei                   = nullptr;
LPALGETSOURCE3I                  alad_alGetSource3i                  = nullptr;
LPALGETSOURCEIV                  alad_alGetSourceiv                  = nullptr;
LPALSOURCEPLAYV                  alad_alSourcePlayv                  = nullptr;
LPALSOURCESTOPV                  alad_alSourceStopv                  = nullptr;
LPALSOURCEREWINDV                alad_alSourceRewindv                = nullptr;
LPALSOURCEPAUSEV                 alad_alSourcePausev                 = nullptr;
LPALSOURCEPLAY                   alad_alSourcePlay                   = nullptr;
LPALSOURCESTOP                   alad_alSourceStop                   = nullptr;
LPALSOURCEREWIND                 alad_alSourceRewind                 = nullptr;
LPALSOURCEPAUSE                  alad_alSourcePause                  = nullptr;
LPALSOURCEQUEUEBUFFERS           alad_alSourceQueueBuffers           = nullptr;
LPALSOURCEUNQUEUEBUFFERS         alad_alSourceUnqueueBuffers         = nullptr;
LPALGENBUFFERS                   alad_alGenBuffers                   = nullptr;
LPALDELETEBUFFERS                alad_alDeleteBuffers                = nullptr;
LPALISBUFFER                     alad_alIsBuffer                     = nullptr;
LPALBUFFERDATA                   alad_alBufferData                   = nullptr;
LPALBUFFERF                      alad_alBufferf                      = nullptr;
LPALBUFFER3F                     alad_alBuffer3f                     = nullptr;
LPALBUFFERFV                     alad_alBufferfv                     = nullptr;
LPALBUFFERI                      alad_alBufferi                      = nullptr;
LPALBUFFER3I                     alad_alBuffer3i                     = nullptr;
LPALBUFFERIV                     alad_alBufferiv                     = nullptr;
LPALGETBUFFERF                   alad_alGetBufferf                   = nullptr;
LPALGETBUFFER3F                  alad_alGetBuffer3f                  = nullptr;
LPALGETBUFFERFV                  alad_alGetBufferfv                  = nullptr;
LPALGETBUFFERI                   alad_alGetBufferi                   = nullptr;
LPALGETBUFFER3I                  alad_alGetBuffer3i                  = nullptr;
LPALGETBUFFERIV                  alad_alGetBufferiv                  = nullptr;

/* EFX */
LPALGENEFFECTS                   alad_alGenEffects                   = nullptr;
LPALDELETEEFFECTS                alad_alDeleteEffects                = nullptr;
LPALISEFFECT                     alad_alIsEffect                     = nullptr;
LPALEFFECTI                      alad_alEffecti                      = nullptr;
LPALEFFECTIV                     alad_alEffectiv                     = nullptr;
LPALEFFECTF                      alad_alEffectf                      = nullptr;
LPALEFFECTFV                     alad_alEffectfv                     = nullptr;
LPALGETEFFECTI                   alad_alGetEffecti                   = nullptr;
LPALGETEFFECTIV                  alad_alGetEffectiv                  = nullptr;
LPALGETEFFECTF                   alad_alGetEffectf                   = nullptr;
LPALGETEFFECTFV                  alad_alGetEffectfv                  = nullptr;
LPALGENFILTERS                   alad_alGenFilters                   = nullptr;
LPALDELETEFILTERS                alad_alDeleteFilters                = nullptr;
LPALISFILTER                     alad_alIsFilter                     = nullptr;
LPALFILTERI                      alad_alFilteri                      = nullptr;
LPALFILTERIV                     alad_alFilteriv                     = nullptr;
LPALFILTERF                      alad_alFilterf                      = nullptr;
LPALFILTERFV                     alad_alFilterfv                     = nullptr;
LPALGETFILTERI                   alad_alGetFilteri                   = nullptr;
LPALGETFILTERIV                  alad_alGetFilteriv                  = nullptr;
LPALGETFILTERF                   alad_alGetFilterf                   = nullptr;
LPALGETFILTERFV                  alad_alGetFilterfv                  = nullptr;
LPALGENAUXILIARYEFFECTSLOTS      alad_alGenAuxiliaryEffectSlots      = nullptr;
LPALDELETEAUXILIARYEFFECTSLOTS   alad_alDeleteAuxiliaryEffectSlots   = nullptr;
LPALISAUXILIARYEFFECTSLOT        alad_alIsAuxiliaryEffectSlot        = nullptr;
LPALAUXILIARYEFFECTSLOTI         alad_alAuxiliaryEffectSloti         = nullptr;
LPALAUXILIARYEFFECTSLOTIV        alad_alAuxiliaryEffectSlotiv        = nullptr;
LPALAUXILIARYEFFECTSLOTF         alad_alAuxiliaryEffectSlotf         = nullptr;
LPALAUXILIARYEFFECTSLOTFV        alad_alAuxiliaryEffectSlotfv        = nullptr;
LPALGETAUXILIARYEFFECTSLOTI      alad_alGetAuxiliaryEffectSloti      = nullptr;
LPALGETAUXILIARYEFFECTSLOTIV     alad_alGetAuxiliaryEffectSlotiv     = nullptr;
LPALGETAUXILIARYEFFECTSLOTF      alad_alGetAuxiliaryEffectSlotf      = nullptr;
LPALGETAUXILIARYEFFECTSLOTFV     alad_alGetAuxiliaryEffectSlotfv     = nullptr;

/* AL extensions */
/* AL_EXT_STATIC_BUFFER */
PFNALBUFFERDATASTATICPROC        alad_alBufferDataStatic             = nullptr;
/* AL_SOFT_buffer_sub_data */
PFNALBUFFERSUBDATASOFTPROC       alad_alBufferSubDataSOFT            = nullptr;
/* AL_EXT_FOLDBACK */
LPALREQUESTFOLDBACKSTART         alad_alRequestFoldbackStart         = nullptr;
LPALREQUESTFOLDBACKSTOP          alad_alRequestFoldbackStop          = nullptr;
/* AL_SOFT_buffer_samples */
LPALBUFFERSAMPLESSOFT            alad_alBufferSamplesSOFT            = nullptr;
LPALBUFFERSUBSAMPLESSOFT         alad_alBufferSubSamplesSOFT         = nullptr;
LPALGETBUFFERSAMPLESSOFT         alad_alGetBufferSamplesSOFT         = nullptr;
LPALISBUFFERFORMATSUPPORTEDSOFT  alad_alIsBufferFormatSupportedSOFT  = nullptr;
/* AL_SOFT_source_latency */
LPALSOURCEDSOFT                  alad_alSourcedSOFT                  = nullptr;
LPALSOURCE3DSOFT                 alad_alSource3dSOFT                 = nullptr;
LPALSOURCEDVSOFT                 alad_alSourcedvSOFT                 = nullptr;
LPALGETSOURCEDSOFT               alad_alGetSourcedSOFT               = nullptr;
LPALGETSOURCE3DSOFT              alad_alGetSource3dSOFT              = nullptr;
LPALGETSOURCEDVSOFT              alad_alGetSourcedvSOFT              = nullptr;
LPALSOURCEI64SOFT                alad_alSourcei64SOFT                = nullptr;
LPALSOURCE3I64SOFT               alad_alSource3i64SOFT               = nullptr;
LPALSOURCEI64VSOFT               alad_alSourcei64vSOFT               = nullptr;
LPALGETSOURCEI64SOFT             alad_alGetSourcei64SOFT             = nullptr;
LPALGETSOURCE3I64SOFT            alad_alGetSource3i64SOFT            = nullptr;
LPALGETSOURCEI64VSOFT            alad_alGetSourcei64vSOFT            = nullptr;
/* AL_SOFT_deferred_updates */
LPALDEFERUPDATESSOFT             alad_alDeferUpdatesSOFT             = nullptr;
LPALPROCESSUPDATESSOFT           alad_alProcessUpdatesSOFT           = nullptr;
/* AL_SOFT_source_resampler */
LPALGETSTRINGISOFT               alad_alGetStringiSOFT               = nullptr;
/* AL_SOFT_events */
LPALEVENTCONTROLSOFT             alad_alEventControlSOFT             = nullptr;
LPALEVENTCALLBACKSOFT            alad_alEventCallbackSOFT            = nullptr;
LPALGETPOINTERSOFT               alad_alGetPointerSOFT               = nullptr;
LPALGETPOINTERVSOFT              alad_alGetPointervSOFT              = nullptr;
/* AL_SOFT_callback_buffer */
LPALBUFFERCALLBACKSOFT           alad_alBufferCallbackSOFT           = nullptr;
LPALGETBUFFERPTRSOFT             alad_alGetBufferPtrSOFT             = nullptr;
LPALGETBUFFER3PTRSOFT            alad_alGetBuffer3PtrSOFT            = nullptr;
LPALGETBUFFERPTRVSOFT            alad_alGetBufferPtrvSOFT            = nullptr;
/* AL_SOFT_source_start_delay */
LPALSOURCEPLAYATTIMESOFT         alad_alSourcePlayAtTimeSOFT         = nullptr;
LPALSOURCEPLAYATTIMEVSOFT        alad_alSourcePlayAtTimevSOFT        = nullptr;
/* AL_EXT_debug */
LPALDEBUGMESSAGECALLBACKEXT      alad_alDebugMessageCallbackEXT      = nullptr;
LPALDEBUGMESSAGEINSERTEXT        alad_alDebugMessageInsertEXT        = nullptr;
LPALDEBUGMESSAGECONTROLEXT       alad_alDebugMessageControlEXT       = nullptr;
LPALPUSHDEBUGGROUPEXT            alad_alPushDebugGroupEXT            = nullptr;
LPALPOPDEBUGGROUPEXT             alad_alPopDebugGroupEXT             = nullptr;
LPALGETDEBUGMESSAGELOGEXT        alad_alGetDebugMessageLogEXT        = nullptr;
LPALOBJECTLABELEXT               alad_alObjectLabelEXT               = nullptr;
LPALGETOBJECTLABELEXT            alad_alGetObjectLabelEXT            = nullptr;

/* Core ALC */
LPALCCREATECONTEXT               alad_alcCreateContext               = nullptr;
LPALCMAKECONTEXTCURRENT          alad_alcMakeContextCurrent          = nullptr;
LPALCPROCESSCONTEXT              alad_alcProcessContext              = nullptr;
LPALCSUSPENDCONTEXT              alad_alcSuspendContext              = nullptr;
LPALCDESTROYCONTEXT              alad_alcDestroyContext              = nullptr;
LPALCGETCURRENTCONTEXT           alad_alcGetCurrentContext           = nullptr;
LPALCGETCONTEXTSDEVICE           alad_alcGetContextsDevice           = nullptr;
LPALCOPENDEVICE                  alad_alcOpenDevice                  = nullptr;
LPALCCLOSEDEVICE                 alad_alcCloseDevice                 = nullptr;
LPALCGETERROR                    alad_alcGetError                    = nullptr;
LPALCISEXTENSIONPRESENT          alad_alcIsExtensionPresent          = nullptr;
LPALCGETPROCADDRESS              alad_alcGetProcAddress              = nullptr;
LPALCGETENUMVALUE                alad_alcGetEnumValue                = nullptr;
LPALCGETSTRING                   alad_alcGetString                   = nullptr;
LPALCGETINTEGERV                 alad_alcGetIntegerv                 = nullptr;
LPALCCAPTUREOPENDEVICE           alad_alcCaptureOpenDevice           = nullptr;
LPALCCAPTURECLOSEDEVICE          alad_alcCaptureCloseDevice          = nullptr;
LPALCCAPTURESTART                alad_alcCaptureStart                = nullptr;
LPALCCAPTURESTOP                 alad_alcCaptureStop                 = nullptr;
LPALCCAPTURESAMPLES              alad_alcCaptureSamples              = nullptr;

/* ALC extensions */
/* ALC_EXT_thread_local_context */
PFNALCSETTHREADCONTEXTPROC       alad_alcSetThreadContext            = nullptr;
PFNALCGETTHREADCONTEXTPROC       alad_alcGetThreadContext            = nullptr;
/* ALC_SOFT_loopback */
LPALCLOOPBACKOPENDEVICESOFT      alad_alcLoopbackOpenDeviceSOFT      = nullptr;
LPALCISRENDERFORMATSUPPORTEDSOFT alad_alcIsRenderFormatSupportedSOFT = nullptr;
LPALCRENDERSAMPLESSOFT           alad_alcRenderSamplesSOFT           = nullptr;
/* ALC_SOFT_pause_device */
LPALCDEVICEPAUSESOFT             alad_alcDevicePauseSOFT             = nullptr;
LPALCDEVICERESUMESOFT            alad_alcDeviceResumeSOFT            = nullptr;
/* ALC_SOFT_HRTF */
LPALCGETSTRINGISOFT              alad:alcGetStringiSOFT              = nullptr;
LPALCRESETDEVICESOFT             alad:alcResetDeviceSOFT             = nullptr;
/* ALC_SOFT_device_clock */
LPALCGETINTEGER64VSOFT           alad_alcGetInteger64vSOFT           = nullptr;
/* ALC_SOFT_reopen_device */
LPALCREOPENDEVICESOFT            alad_alcReopenDeviceSOFT            = nullptr;
/* ALC_SOFT_system_events */
LPALCEVENTISSUPPORTEDSOFT        alad_alcEventIsSupportedSOFT        = nullptr;
LPALCEVENTCONTROLSOFT            alad_alcEventControlSOFT            = nullptr;
LPALCEVENTCALLBACKSOFT           alad_alcEventCallbackSOFT           = nullptr;



        
        
/*  Function loading facilities: */
        
        
        
typedef void (*alad_proc_) (void);
        

/* ISO C compatibility types for GCC warning: ISO C forbids conversion of object pointer to function pointer type [-Wpedantic] */
typedef alad_proc_ (AL_APIENTRY *ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_) (const ALchar *fname);
typedef alad_proc_ (ALC_APIENTRY *ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS_) (ALCdevice *device, const ALCchar *funcname);
typedef alad_proc_ (ALC_APIENTRY *ALAD_ISO_C_COMPAT_dlsym_) (void *module, const char *name);
        

/* modelled after GLFW, see win32_module.c and posix_module.c specifically */
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__)
#define NOMINMAX
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef HMODULE alad_module_t_;
static alad_module_t_ alad_open_ (const char *path) {
        return LoadLibraryA (path);
}
static alad_proc_ alad_load_ (alad_module_t_ module, const char *name) {
        return REINTERPRET_CAST(alad_proc_, GetProcAddress (module, name));
}
static void alad_close_ (alad_module_t_ module) {
        FreeLibrary (module);
}
#define alad_LIB_NAME_           "OpenAL32.dll"
#define alad_SECONDARY_LIB_NAME_ "soft_oal.dll"
#else /* Unix defaults otherwise */
#include <dlfcn.h>
typedef void *alad_module_t_;
static alad_module_t_ alad_open_ (const char *path) {
        return dlopen (path, RTLD_LAZY | RTLD_LOCAL);
}
static alad_proc_ alad_load_ (alad_module_t_ module, const char *name) {
        ALAD_ISO_C_COMPAT_dlsym_ compat_dlsym;
        compat_dlsym = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_dlsym_, dlsym);
        return REINTERPRET_CAST(alad_proc_, compat_dlsym (module, name));
}
static void alad_close_ (alad_module_t_ module) {
        dlclose (module);
}

/* there are also libopenal.so.1.[X].[Y] and libopenal.1.[X].[Y].dylib respectively, but it would be difficult to look all of those up */
#if defined(__APPLE__)
/* not tested myself; the only references I could find are https://github.com/ToweOPrO/sadsad and https://pastebin.com/MEmh3ZFr, which is at least tenuous */
#define alad_LIB_NAME_           "libopenal.1.dylib"
#define alad_SECONDARY_LIB_NAME_ "libopenal.dylib"
#else
#define alad_LIB_NAME_           "libopenal.so.1"
#define alad_SECONDARY_LIB_NAME_ "libopenal.so"
#endif

#endif /* _WIN32 */


/* The functions using alad_load_ and alad_close_ are defined down below, the definitions were moved to here
   in order to have the alad_module_t_ type defined to be used in the following functions. */

static void alad_load_al_functions_contextfree_dlsym_ (alad_module_t_ module, ALboolean loadAll) {
        if (module == nullptr) return;
        /* Core AL functions without buffer, source, listener and doppler/distance functions, because none of these are necessary to create a context */
        alad_alGetProcAddress     = REINTERPRET_CAST(LPALGETPROCADDRESS, alad_load_ (module, "alGetProcAddress"));
        alad_alEnable             = REINTERPRET_CAST(LPALENABLE, alad_load_ (module, "alEnable"));
        alad_alDisable            = REINTERPRET_CAST(LPALDISABLE, alad_load_ (module, "alDisable"));
        alad_alIsEnabled          = REINTERPRET_CAST(LPALISENABLED, alad_load_ (module, "alIsEnabled"));
        alad_alGetString          = REINTERPRET_CAST(LPALGETSTRING, alad_load_ (module, "alGetString"));
        alad_alGetBooleanv        = REINTERPRET_CAST(LPALGETBOOLEANV, alad_load_ (module, "alGetBooleanv"));
        alad_alGetIntegerv        = REINTERPRET_CAST(LPALGETINTEGERV, alad_load_ (module, "alGetIntegerv"));
        alad_alGetFloatv          = REINTERPRET_CAST(LPALGETFLOATV, alad_load_ (module, "alGetFloatv"));
        alad_alGetDoublev         = REINTERPRET_CAST(LPALGETDOUBLEV, alad_load_ (module, "alGetDoublev"));
        alad_alGetBoolean         = REINTERPRET_CAST(LPALGETBOOLEAN, alad_load_ (module, "alGetBoolean"));
        alad_alGetInteger         = REINTERPRET_CAST(LPALGETINTEGER, alad_load_ (module, "alGetInteger"));
        alad_alGetFloat           = REINTERPRET_CAST(LPALGETFLOAT, alad_load_ (module, "alGetFloat"));
        alad_alGetDouble          = REINTERPRET_CAST(LPALGETDOUBLE, alad_load_ (module, "alGetDouble"));
        alad_alGetError           = REINTERPRET_CAST(LPALGETERROR, alad_load_ (module, "alGetError"));
        alad_alIsExtensionPresent = REINTERPRET_CAST(LPALISEXTENSIONPRESENT, alad_load_ (module, "alIsExtensionPresent"));
        alad_alGetEnumValue       = REINTERPRET_CAST(LPALGETENUMVALUE, alad_load_ (module, "alGetEnumValue"));
        /* the rest if wanted */
        if (loadAll != AL_FALSE) {
                alad_alDopplerFactor        = REINTERPRET_CAST(LPALDOPPLERFACTOR, alad_load_ (module, "alDopplerFactor"));
                alad_alDopplerVelocity      = REINTERPRET_CAST(LPALDOPPLERVELOCITY, alad_load_ (module, "alDopplerVelocity"));
                alad_alSpeedOfSound         = REINTERPRET_CAST(LPALSPEEDOFSOUND, alad_load_ (module, "alSpeedOfSound"));
                alad_alDistanceModel        = REINTERPRET_CAST(LPALDISTANCEMODEL, alad_load_ (module, "alDistanceModel"));
                alad_alListenerf            = REINTERPRET_CAST(LPALLISTENERF, alad_load_ (module, "alListenerf"));
                alad_alListener3f           = REINTERPRET_CAST(LPALLISTENER3F, alad_load_ (module, "alListener3f"));
                alad_alListenerfv           = REINTERPRET_CAST(LPALLISTENERFV, alad_load_ (module, "alListenerfv"));
                alad_alListeneri            = REINTERPRET_CAST(LPALLISTENERI, alad_load_ (module, "alListeneri"));
                alad_alListener3i           = REINTERPRET_CAST(LPALLISTENER3I, alad_load_ (module, "alListener3i"));
                alad_alListeneriv           = REINTERPRET_CAST(LPALLISTENERIV, alad_load_ (module, "alListeneriv"));
                alad_alGetListenerf         = REINTERPRET_CAST(LPALGETLISTENERF, alad_load_ (module, "alGetListenerf"));
                alad_alGetListener3f        = REINTERPRET_CAST(LPALGETLISTENER3F, alad_load_ (module, "alGetListener3f"));
                alad_alGetListenerfv        = REINTERPRET_CAST(LPALGETLISTENERFV, alad_load_ (module, "alGetListenerfv"));
                alad_alGetListeneri         = REINTERPRET_CAST(LPALGETLISTENERI, alad_load_ (module, "alGetListeneri"));
                alad_alGetListener3i        = REINTERPRET_CAST(LPALGETLISTENER3I, alad_load_ (module, "alGetListener3i"));
                alad_alGetListeneriv        = REINTERPRET_CAST(LPALGETLISTENERIV, alad_load_ (module, "alGetListeneriv"));
                alad_alGenSources           = REINTERPRET_CAST(LPALGENSOURCES, alad_load_ (module, "alGenSources"));
                alad_alDeleteSources        = REINTERPRET_CAST(LPALDELETESOURCES, alad_load_ (module, "alDeleteSources"));
                alad_alIsSource             = REINTERPRET_CAST(LPALISSOURCE, alad_load_ (module, "alIsSource"));
                alad_alSourcef              = REINTERPRET_CAST(LPALSOURCEF, alad_load_ (module, "alSourcef"));
                alad_alSource3f             = REINTERPRET_CAST(LPALSOURCE3F, alad_load_ (module, "alSource3f"));
                alad_alSourcefv             = REINTERPRET_CAST(LPALSOURCEFV, alad_load_ (module, "alSourcefv"));
                alad_alSourcei              = REINTERPRET_CAST(LPALSOURCEI, alad_load_ (module, "alSourcei"));
                alad_alSource3i             = REINTERPRET_CAST(LPALSOURCE3I, alad_load_ (module, "alSource3i"));
                alad_alSourceiv             = REINTERPRET_CAST(LPALSOURCEIV, alad_load_ (module, "alSourceiv"));
                alad_alGetSourcef           = REINTERPRET_CAST(LPALGETSOURCEF, alad_load_ (module, "alGetSourcef"));
                alad_alGetSource3f          = REINTERPRET_CAST(LPALGETSOURCE3F, alad_load_ (module, "alGetSource3f"));
                alad_alGetSourcefv          = REINTERPRET_CAST(LPALGETSOURCEFV, alad_load_ (module, "alGetSourcefv"));
                alad_alGetSourcei           = REINTERPRET_CAST(LPALGETSOURCEI, alad_load_ (module, "alGetSourcei"));
                alad_alGetSource3i          = REINTERPRET_CAST(LPALGETSOURCE3I, alad_load_ (module, "alGetSource3i"));
                alad_alGetSourceiv          = REINTERPRET_CAST(LPALGETSOURCEIV, alad_load_ (module, "alGetSourceiv"));
                alad_alSourcePlayv          = REINTERPRET_CAST(LPALSOURCEPLAYV, alad_load_ (module, "alSourcePlayv"));
                alad_alSourceStopv          = REINTERPRET_CAST(LPALSOURCESTOPV, alad_load_ (module, "alSourceStopv"));
                alad_alSourceRewindv        = REINTERPRET_CAST(LPALSOURCEREWINDV, alad_load_ (module, "alSourceRewindv"));
                alad_alSourcePausev         = REINTERPRET_CAST(LPALSOURCEPAUSEV, alad_load_ (module, "alSourcePausev"));
                alad_alSourcePlay           = REINTERPRET_CAST(LPALSOURCEPLAY, alad_load_ (module, "alSourcePlay"));
                alad_alSourceStop           = REINTERPRET_CAST(LPALSOURCESTOP, alad_load_ (module, "alSourceStop"));
                alad_alSourceRewind         = REINTERPRET_CAST(LPALSOURCEREWIND, alad_load_ (module, "alSourceRewind"));
                alad_alSourcePause          = REINTERPRET_CAST(LPALSOURCEPAUSE, alad_load_ (module, "alSourcePause"));
                alad_alSourceQueueBuffers   = REINTERPRET_CAST(LPALSOURCEQUEUEBUFFERS, alad_load_ (module, "alSourceQueueBuffers"));
                alad_alSourceUnqueueBuffers = REINTERPRET_CAST(LPALSOURCEUNQUEUEBUFFERS, alad_load_ (module, "alSourceUnqueueBuffers"));
                alad_alGenBuffers           = REINTERPRET_CAST(LPALGENBUFFERS, alad_load_ (module, "alGenBuffers"));
                alad_alDeleteBuffers        = REINTERPRET_CAST(LPALDELETEBUFFERS, alad_load_ (module, "alDeleteBuffers"));
                alad_alIsBuffer             = REINTERPRET_CAST(LPALISBUFFER, alad_load_ (module, "alIsBuffer"));
                alad_alBufferData           = REINTERPRET_CAST(LPALBUFFERDATA, alad_load_ (module, "alBufferData"));
                alad_alBufferf              = REINTERPRET_CAST(LPALBUFFERF, alad_load_ (module, "alBufferf"));
                alad_alBuffer3f             = REINTERPRET_CAST(LPALBUFFER3F, alad_load_ (module, "alBuffer3f"));
                alad_alBufferfv             = REINTERPRET_CAST(LPALBUFFERFV, alad_load_ (module, "alBufferfv"));
                alad_alBufferi              = REINTERPRET_CAST(LPALBUFFERI, alad_load_ (module, "alBufferi"));
                alad_alBuffer3i             = REINTERPRET_CAST(LPALBUFFER3I, alad_load_ (module, "alBuffer3i"));
                alad_alBufferiv             = REINTERPRET_CAST(LPALBUFFERIV, alad_load_ (module, "alBufferiv"));
                alad_alGetBufferf           = REINTERPRET_CAST(LPALGETBUFFERF, alad_load_ (module, "alGetBufferf"));
                alad_alGetBuffer3f          = REINTERPRET_CAST(LPALGETBUFFER3F, alad_load_ (module, "alGetBuffer3f"));
                alad_alGetBufferfv          = REINTERPRET_CAST(LPALGETBUFFERFV, alad_load_ (module, "alGetBufferfv"));
                alad_alGetBufferi           = REINTERPRET_CAST(LPALGETBUFFERI, alad_load_ (module, "alGetBufferi"));
                alad_alGetBuffer3i          = REINTERPRET_CAST(LPALGETBUFFER3I, alad_load_ (module, "alGetBuffer3i"));
                alad_alGetBufferiv          = REINTERPRET_CAST(LPALGETBUFFERIV, alad_load_ (module, "alGetBufferiv"));
        }
}

static void alad_load_al_functions_ () {
        ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_ compat_alGetProcAddress;
        if (alGetProcAddress == nullptr) return;
        compat_alGetProcAddress = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_, alad_alGetProcAddress);
        /* Core AL */
        alad_alDopplerFactor                                               = REINTERPRET_CAST(LPALDOPPLERFACTOR, compat_alGetProcAddress ("alDopplerFactor"));
        alad_alDopplerVelocity                                             = REINTERPRET_CAST(LPALDOPPLERVELOCITY, compat_alGetProcAddress ("alDopplerVelocity"));
        alad_alSpeedOfSound                                                = REINTERPRET_CAST(LPALSPEEDOFSOUND, compat_alGetProcAddress ("alSpeedOfSound"));
        alad_alDistanceModel                                               = REINTERPRET_CAST(LPALDISTANCEMODEL, compat_alGetProcAddress ("alDistanceModel"));
        alad_alEnable                                                      = REINTERPRET_CAST(LPALENABLE, compat_alGetProcAddress ("alEnable"));
        alad_alDisable                                                     = REINTERPRET_CAST(LPALDISABLE, compat_alGetProcAddress ("alDisable"));
        alad_alIsEnabled                                                   = REINTERPRET_CAST(LPALISENABLED, compat_alGetProcAddress ("alIsEnabled"));
        alad_alGetString                                                   = REINTERPRET_CAST(LPALGETSTRING, compat_alGetProcAddress ("alGetString"));
        alad_alGetBooleanv                                                 = REINTERPRET_CAST(LPALGETBOOLEANV, compat_alGetProcAddress ("alGetBooleanv"));
        alad_alGetIntegerv                                                 = REINTERPRET_CAST(LPALGETINTEGERV, compat_alGetProcAddress ("alGetIntegerv"));
        alad_alGetFloatv                                                   = REINTERPRET_CAST(LPALGETFLOATV, compat_alGetProcAddress ("alGetFloatv"));
        alad_alGetDoublev                                                  = REINTERPRET_CAST(LPALGETDOUBLEV, compat_alGetProcAddress ("alGetDoublev"));
        alad_alGetBoolean                                                  = REINTERPRET_CAST(LPALGETBOOLEAN, compat_alGetProcAddress ("alGetBoolean"));
        alad_alGetInteger                                                  = REINTERPRET_CAST(LPALGETINTEGER, compat_alGetProcAddress ("alGetInteger"));
        alad_alGetFloat                                                    = REINTERPRET_CAST(LPALGETFLOAT, compat_alGetProcAddress ("alGetFloat"));
        alad_alGetDouble                                                   = REINTERPRET_CAST(LPALGETDOUBLE, compat_alGetProcAddress ("alGetDouble"));
        alad_alGetError                                                    = REINTERPRET_CAST(LPALGETERROR, compat_alGetProcAddress ("alGetError"));
        alad_alIsExtensionPresent                                          = REINTERPRET_CAST(LPALISEXTENSIONPRESENT, compat_alGetProcAddress ("alIsExtensionPresent"));
        alad_alGetEnumValue                                                = REINTERPRET_CAST(LPALGETENUMVALUE, compat_alGetProcAddress ("alGetEnumValue"));
        alad_alListenerf                                                   = REINTERPRET_CAST(LPALLISTENERF, compat_alGetProcAddress ("alListenerf"));
        alad_alListener3f                                                  = REINTERPRET_CAST(LPALLISTENER3F, compat_alGetProcAddress ("alListener3f"));
        alad_alListenerfv                                                  = REINTERPRET_CAST(LPALLISTENERFV, compat_alGetProcAddress ("alListenerfv"));
        alad_alListeneri                                                   = REINTERPRET_CAST(LPALLISTENERI, compat_alGetProcAddress ("alListeneri"));
        alad_alListener3i                                                  = REINTERPRET_CAST(LPALLISTENER3I, compat_alGetProcAddress ("alListener3i"));
        alad_alListeneriv                                                  = REINTERPRET_CAST(LPALLISTENERIV, compat_alGetProcAddress ("alListeneriv"));
        alad_alGetListenerf                                                = REINTERPRET_CAST(LPALGETLISTENERF, compat_alGetProcAddress ("alGetListenerf"));
        alad_alGetListener3f                                               = REINTERPRET_CAST(LPALGETLISTENER3F, compat_alGetProcAddress ("alGetListener3f"));
        alad_alGetListenerfv                                               = REINTERPRET_CAST(LPALGETLISTENERFV, compat_alGetProcAddress ("alGetListenerfv"));
        alad_alGetListeneri                                                = REINTERPRET_CAST(LPALGETLISTENERI, compat_alGetProcAddress ("alGetListeneri"));
        alad_alGetListener3i                                               = REINTERPRET_CAST(LPALGETLISTENER3I, compat_alGetProcAddress ("alGetListener3i"));
        alad_alGetListeneriv                                               = REINTERPRET_CAST(LPALGETLISTENERIV, compat_alGetProcAddress ("alGetListeneriv"));
        alad_alGenSources                                                  = REINTERPRET_CAST(LPALGENSOURCES, compat_alGetProcAddress ("alGenSources"));
        alad_alDeleteSources                                               = REINTERPRET_CAST(LPALDELETESOURCES, compat_alGetProcAddress ("alDeleteSources"));
        alad_alIsSource                                                    = REINTERPRET_CAST(LPALISSOURCE, compat_alGetProcAddress ("alIsSource"));
        alad_alSourcef                                                     = REINTERPRET_CAST(LPALSOURCEF, compat_alGetProcAddress ("alSourcef"));
        alad_alSource3f                                                    = REINTERPRET_CAST(LPALSOURCE3F, compat_alGetProcAddress ("alSource3f"));
        alad_alSourcefv                                                    = REINTERPRET_CAST(LPALSOURCEFV, compat_alGetProcAddress ("alSourcefv"));
        alad_alSourcei                                                     = REINTERPRET_CAST(LPALSOURCEI, compat_alGetProcAddress ("alSourcei"));
        alad_alSource3i                                                    = REINTERPRET_CAST(LPALSOURCE3I, compat_alGetProcAddress ("alSource3i"));
        alad_alSourceiv                                                    = REINTERPRET_CAST(LPALSOURCEIV, compat_alGetProcAddress ("alSourceiv"));
        alad_alGetSourcef                                                  = REINTERPRET_CAST(LPALGETSOURCEF, compat_alGetProcAddress ("alGetSourcef"));
        alad_alGetSource3f                                                 = REINTERPRET_CAST(LPALGETSOURCE3F, compat_alGetProcAddress ("alGetSource3f"));
        alad_alGetSourcefv                                                 = REINTERPRET_CAST(LPALGETSOURCEFV, compat_alGetProcAddress ("alGetSourcefv"));
        alad_alGetSourcei                                                  = REINTERPRET_CAST(LPALGETSOURCEI, compat_alGetProcAddress ("alGetSourcei"));
        alad_alGetSource3i                                                 = REINTERPRET_CAST(LPALGETSOURCE3I, compat_alGetProcAddress ("alGetSource3i"));
        alad_alGetSourceiv                                                 = REINTERPRET_CAST(LPALGETSOURCEIV, compat_alGetProcAddress ("alGetSourceiv"));
        alad_alSourcePlayv                                                 = REINTERPRET_CAST(LPALSOURCEPLAYV, compat_alGetProcAddress ("alSourcePlayv"));
        alad_alSourceStopv                                                 = REINTERPRET_CAST(LPALSOURCESTOPV, compat_alGetProcAddress ("alSourceStopv"));
        alad_alSourceRewindv                                               = REINTERPRET_CAST(LPALSOURCEREWINDV, compat_alGetProcAddress ("alSourceRewindv"));
        alad_alSourcePausev                                                = REINTERPRET_CAST(LPALSOURCEPAUSEV, compat_alGetProcAddress ("alSourcePausev"));
        alad_alSourcePlay                                                  = REINTERPRET_CAST(LPALSOURCEPLAY, compat_alGetProcAddress ("alSourcePlay"));
        alad_alSourceStop                                                  = REINTERPRET_CAST(LPALSOURCESTOP, compat_alGetProcAddress ("alSourceStop"));
        alad_alSourceRewind                                                = REINTERPRET_CAST(LPALSOURCEREWIND, compat_alGetProcAddress ("alSourceRewind"));
        alad_alSourcePause                                                 = REINTERPRET_CAST(LPALSOURCEPAUSE, compat_alGetProcAddress ("alSourcePause"));
        alad_alSourceQueueBuffers                                          = REINTERPRET_CAST(LPALSOURCEQUEUEBUFFERS, compat_alGetProcAddress ("alSourceQueueBuffers"));
        alad_alSourceUnqueueBuffers                                        = REINTERPRET_CAST(LPALSOURCEUNQUEUEBUFFERS, compat_alGetProcAddress ("alSourceUnqueueBuffers"));
        alad_alGenBuffers                                                  = REINTERPRET_CAST(LPALGENBUFFERS, compat_alGetProcAddress ("alGenBuffers"));
        alad_alDeleteBuffers                                               = REINTERPRET_CAST(LPALDELETEBUFFERS, compat_alGetProcAddress ("alDeleteBuffers"));
        alad_alIsBuffer                                                    = REINTERPRET_CAST(LPALISBUFFER, compat_alGetProcAddress ("alIsBuffer"));
        alad_alBufferData                                                  = REINTERPRET_CAST(LPALBUFFERDATA, compat_alGetProcAddress ("alBufferData"));
        alad_alBufferf                                                     = REINTERPRET_CAST(LPALBUFFERF, compat_alGetProcAddress ("alBufferf"));
        alad_alBuffer3f                                                    = REINTERPRET_CAST(LPALBUFFER3F, compat_alGetProcAddress ("alBuffer3f"));
        alad_alBufferfv                                                    = REINTERPRET_CAST(LPALBUFFERFV, compat_alGetProcAddress ("alBufferfv"));
        alad_alBufferi                                                     = REINTERPRET_CAST(LPALBUFFERI, compat_alGetProcAddress ("alBufferi"));
        alad_alBuffer3i                                                    = REINTERPRET_CAST(LPALBUFFER3I, compat_alGetProcAddress ("alBuffer3i"));
        alad_alBufferiv                                                    = REINTERPRET_CAST(LPALBUFFERIV, compat_alGetProcAddress ("alBufferiv"));
        alad_alGetBufferf                                                  = REINTERPRET_CAST(LPALGETBUFFERF, compat_alGetProcAddress ("alGetBufferf"));
        alad_alGetBuffer3f                                                 = REINTERPRET_CAST(LPALGETBUFFER3F, compat_alGetProcAddress ("alGetBuffer3f"));
        alad_alGetBufferfv                                                 = REINTERPRET_CAST(LPALGETBUFFERFV, compat_alGetProcAddress ("alGetBufferfv"));
        alad_alGetBufferi                                                  = REINTERPRET_CAST(LPALGETBUFFERI, compat_alGetProcAddress ("alGetBufferi"));
        alad_alGetBuffer3i                                                 = REINTERPRET_CAST(LPALGETBUFFER3I, compat_alGetProcAddress ("alGetBuffer3i"));
        alad_alGetBufferiv                                                 = REINTERPRET_CAST(LPALGETBUFFERIV, compat_alGetProcAddress ("alGetBufferiv"));
}

static void alad_load_al_extension_functions_ () {
        ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_ compat_alGetProcAddress;
        if (alGetProcAddress == nullptr) return;
        compat_alGetProcAddress = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_, alad_alGetProcAddress);
        /* EFX */
        alad_alGenEffects                                                  = REINTERPRET_CAST(LPALGENEFFECTS, compat_alGetProcAddress ("alGenEffects"));
        alad_alDeleteEffects                                               = REINTERPRET_CAST(LPALDELETEEFFECTS, compat_alGetProcAddress ("alDeleteEffects"));
        alad_alIsEffect                                                    = REINTERPRET_CAST(LPALISEFFECT, compat_alGetProcAddress ("alIsEffect"));
        alad_alEffecti                                                     = REINTERPRET_CAST(LPALEFFECTI, compat_alGetProcAddress ("alEffecti"));
        alad_alEffectiv                                                    = REINTERPRET_CAST(LPALEFFECTIV, compat_alGetProcAddress ("alEffectiv"));
        alad_alEffectf                                                     = REINTERPRET_CAST(LPALEFFECTF, compat_alGetProcAddress ("alEffectf"));
        alad_alEffectfv                                                    = REINTERPRET_CAST(LPALEFFECTFV, compat_alGetProcAddress ("alEffectfv"));
        alad_alGetEffecti                                                  = REINTERPRET_CAST(LPALGETEFFECTI, compat_alGetProcAddress ("alGetEffecti"));
        alad_alGetEffectiv                                                 = REINTERPRET_CAST(LPALGETEFFECTIV, compat_alGetProcAddress ("alGetEffectiv"));
        alad_alGetEffectf                                                  = REINTERPRET_CAST(LPALGETEFFECTF, compat_alGetProcAddress ("alGetEffectf"));
        alad_alGetEffectfv                                                 = REINTERPRET_CAST(LPALGETEFFECTFV, compat_alGetProcAddress ("alGetEffectfv"));
        alad_alGenFilters                                                  = REINTERPRET_CAST(LPALGENFILTERS, compat_alGetProcAddress ("alGenFilters"));
        alad_alDeleteFilters                                               = REINTERPRET_CAST(LPALDELETEFILTERS, compat_alGetProcAddress ("alDeleteFilters"));
        alad_alIsFilter                                                    = REINTERPRET_CAST(LPALISFILTER, compat_alGetProcAddress ("alIsFilter"));
        alad_alFilteri                                                     = REINTERPRET_CAST(LPALFILTERI, compat_alGetProcAddress ("alFilteri"));
        alad_alFilteriv                                                    = REINTERPRET_CAST(LPALFILTERIV, compat_alGetProcAddress ("alFilteriv"));
        alad_alFilterf                                                     = REINTERPRET_CAST(LPALFILTERF, compat_alGetProcAddress ("alFilterf"));
        alad_alFilterfv                                                    = REINTERPRET_CAST(LPALFILTERFV, compat_alGetProcAddress ("alFilterfv"));
        alad_alGetFilteri                                                  = REINTERPRET_CAST(LPALGETFILTERI, compat_alGetProcAddress ("alGetFilteri"));
        alad_alGetFilteriv                                                 = REINTERPRET_CAST(LPALGETFILTERIV, compat_alGetProcAddress ("alGetFilteriv"));
        alad_alGetFilterf                                                  = REINTERPRET_CAST(LPALGETFILTERF, compat_alGetProcAddress ("alGetFilterf"));
        alad_alGetFilterfv                                                 = REINTERPRET_CAST(LPALGETFILTERFV, compat_alGetProcAddress ("alGetFilterfv"));
        alad_alGenAuxiliaryEffectSlots                                     = REINTERPRET_CAST(LPALGENAUXILIARYEFFECTSLOTS, compat_alGetProcAddress ("alGenAuxiliaryEffectSlots"));
        alad_alDeleteAuxiliaryEffectSlots  = REINTERPRET_CAST(LPALDELETEAUXILIARYEFFECTSLOTS, compat_alGetProcAddress ("alDeleteAuxiliaryEffectSlots"));
        alad_alIsAuxiliaryEffectSlot       = REINTERPRET_CAST(LPALISAUXILIARYEFFECTSLOT, compat_alGetProcAddress ("alIsAuxiliaryEffectSlot"));
        alad_alAuxiliaryEffectSloti        = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTI, compat_alGetProcAddress ("alAuxiliaryEffectSloti"));
        alad_alAuxiliaryEffectSlotiv       = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTIV, compat_alGetProcAddress ("alAuxiliaryEffectSlotiv"));
        alad_alAuxiliaryEffectSlotf        = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTF, compat_alGetProcAddress ("alAuxiliaryEffectSlotf"));
        alad_alAuxiliaryEffectSlotfv       = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTFV, compat_alGetProcAddress ("alAuxiliaryEffectSlotfv"));
        alad_alGetAuxiliaryEffectSloti     = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTI, compat_alGetProcAddress ("alGetAuxiliaryEffectSloti"));
        alad_alGetAuxiliaryEffectSlotiv    = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTIV, compat_alGetProcAddress ("alGetAuxiliaryEffectSlotiv"));
        alad_alGetAuxiliaryEffectSlotf     = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTF, compat_alGetProcAddress ("alGetAuxiliaryEffectSlotf"));
        alad_alGetAuxiliaryEffectSlotfv    = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTFV, compat_alGetProcAddress ("alGetAuxiliaryEffectSlotfv"));

        /* AL extensions */
        /* AL_EXT_STATIC_BUFFER */
        alad_alBufferDataStatic            = REINTERPRET_CAST(PFNALBUFFERDATASTATICPROC, compat_alGetProcAddress ("alBufferDataStatic"));
        /* AL_SOFT_buffer_sub_data */
        alad_alBufferSubDataSOFT           = REINTERPRET_CAST(PFNALBUFFERSUBDATASOFTPROC, compat_alGetProcAddress ("alBufferSubDataSOFT"));
        /* AL_EXT_FOLDBACK */
        alad_alRequestFoldbackStart        = REINTERPRET_CAST(LPALREQUESTFOLDBACKSTART, compat_alGetProcAddress ("alRequestFoldbackStart"));
        alad_alRequestFoldbackStop         = REINTERPRET_CAST(LPALREQUESTFOLDBACKSTOP, compat_alGetProcAddress ("alRequestFoldbackStop"));
        /* AL_SOFT_buffer_samples */
        alad_alBufferSamplesSOFT           = REINTERPRET_CAST(LPALBUFFERSAMPLESSOFT, compat_alGetProcAddress ("alBufferSamplesSOFT"));
        alad_alBufferSubSamplesSOFT        = REINTERPRET_CAST(LPALBUFFERSUBSAMPLESSOFT, compat_alGetProcAddress ("alBufferSubSamplesSOFT"));
        alad_alGetBufferSamplesSOFT        = REINTERPRET_CAST(LPALGETBUFFERSAMPLESSOFT, compat_alGetProcAddress ("alGetBufferSamplesSOFT"));
        alad_alIsBufferFormatSupportedSOFT = REINTERPRET_CAST(LPALISBUFFERFORMATSUPPORTEDSOFT, compat_alGetProcAddress ("alIsBufferFormatSupportedSOFT"));
        /* AL_SOFT_source_latency */
        alad_alSourcedSOFT                 = REINTERPRET_CAST(LPALSOURCEDSOFT, compat_alGetProcAddress ("alSourcedSOFT"));
        alad_alSource3dSOFT                = REINTERPRET_CAST(LPALSOURCE3DSOFT, compat_alGetProcAddress ("alSource3dSOFT"));
        alad_alSourcedvSOFT                = REINTERPRET_CAST(LPALSOURCEDVSOFT, compat_alGetProcAddress ("alSourcedvSOFT"));
        alad_alGetSourcedSOFT              = REINTERPRET_CAST(LPALGETSOURCEDSOFT, compat_alGetProcAddress ("alGetSourcedSOFT"));
        alad_alGetSource3dSOFT             = REINTERPRET_CAST(LPALGETSOURCE3DSOFT, compat_alGetProcAddress ("alGetSource3dSOFT"));
        alad_alGetSourcedvSOFT             = REINTERPRET_CAST(LPALGETSOURCEDVSOFT, compat_alGetProcAddress ("alGetSourcedvSOFT"));
        alad_alSourcei64SOFT               = REINTERPRET_CAST(LPALSOURCEI64SOFT, compat_alGetProcAddress ("alSourcei64SOFT"));
        alad_alSource3i64SOFT              = REINTERPRET_CAST(LPALSOURCE3I64SOFT, compat_alGetProcAddress ("alSource3i64SOFT"));
        alad_alSourcei64vSOFT              = REINTERPRET_CAST(LPALSOURCEI64VSOFT, compat_alGetProcAddress ("alSourcei64vSOFT"));
        alad_alGetSourcei64SOFT            = REINTERPRET_CAST(LPALGETSOURCEI64SOFT, compat_alGetProcAddress ("alGetSourcei64SOFT"));
        alad_alGetSource3i64SOFT           = REINTERPRET_CAST(LPALGETSOURCE3I64SOFT, compat_alGetProcAddress ("alGetSource3i64SOFT"));
        alad_alGetSourcei64vSOFT           = REINTERPRET_CAST(LPALGETSOURCEI64VSOFT, compat_alGetProcAddress ("alGetSourcei64vSOFT"));
        /* AL_SOFT_deferred_updates */
        alad_alDeferUpdatesSOFT            = REINTERPRET_CAST(LPALDEFERUPDATESSOFT, compat_alGetProcAddress ("alDeferUpdatesSOFT"));
        alad_alProcessUpdatesSOFT          = REINTERPRET_CAST(LPALPROCESSUPDATESSOFT, compat_alGetProcAddress ("alProcessUpdatesSOFT"));
        /* AL_SOFT_source_resampler */
        alad_alGetStringiSOFT              = REINTERPRET_CAST(LPALGETSTRINGISOFT, compat_alGetProcAddress ("alGetStringiSOFT"));
        /* AL_SOFT_events */
        alad_alEventControlSOFT            = REINTERPRET_CAST(LPALEVENTCONTROLSOFT, compat_alGetProcAddress ("alEventControlSOFT"));
        alad_alEventCallbackSOFT           = REINTERPRET_CAST(LPALEVENTCALLBACKSOFT, compat_alGetProcAddress ("alEventCallbackSOFT"));
        alad_alGetPointerSOFT              = REINTERPRET_CAST(LPALGETPOINTERSOFT, compat_alGetProcAddress ("alGetPointerSOFT"));
        alad_alGetPointervSOFT             = REINTERPRET_CAST(LPALGETPOINTERVSOFT, compat_alGetProcAddress ("alGetPointervSOFT"));
        /* AL_SOFT_callback_buffer */
        alad_alBufferCallbackSOFT          = REINTERPRET_CAST(LPALBUFFERCALLBACKSOFT, compat_alGetProcAddress ("alBufferCallbackSOFT"));
        alad_alGetBufferPtrSOFT            = REINTERPRET_CAST(LPALGETBUFFERPTRSOFT, compat_alGetProcAddress ("alGetBufferPtrSOFT"));
        alad_alGetBuffer3PtrSOFT           = REINTERPRET_CAST(LPALGETBUFFER3PTRSOFT, compat_alGetProcAddress ("alGetBuffer3PtrSOFT"));
        alad_alGetBufferPtrvSOFT           = REINTERPRET_CAST(LPALGETBUFFERPTRVSOFT, compat_alGetProcAddress ("alGetBufferPtrvSOFT"));
        /* AL_SOFT_source_start_delay */
        alad_alSourcePlayAtTimeSOFT        = REINTERPRET_CAST(LPALSOURCEPLAYATTIMESOFT, compat_alGetProcAddress ("alSourcePlayAtTimeSOFT"));
        alad_alSourcePlayAtTimevSOFT       = REINTERPRET_CAST(LPALSOURCEPLAYATTIMEVSOFT, compat_alGetProcAddress ("alSourcePlayAtTimevSOFT"));
        /* AL_EXT_debug */
        alad_alDebugMessageCallbackEXT      = REINTERPRET_CAST(LPALDEBUGMESSAGECALLBACKEXT, compat_alGetProcAddress ("alDebugMessageCallbackEXT"));
        alad_alDebugMessageInsertEXT        = REINTERPRET_CAST(LPALDEBUGMESSAGEINSERTEXT, compat_alGetProcAddress ("alDebugMessageInsertEXT"));
        alad_alDebugMessageControlEXT       = REINTERPRET_CAST(LPALDEBUGMESSAGECONTROLEXT, compat_alGetProcAddress ("alDebugMessageControlEXT"));
        alad_alPushDebugGroupEXT            = REINTERPRET_CAST(LPALPUSHDEBUGGROUPEXT, compat_alGetProcAddress ("alPushDebugGroupEXT"));
        alad_alPopDebugGroupEXT             = REINTERPRET_CAST(LPALPOPDEBUGGROUPEXT, compat_alGetProcAddress ("alPopDebugGroupEXT"));
        alad_alGetDebugMessageLogEXT        = REINTERPRET_CAST(LPALGETDEBUGMESSAGELOGEXT, compat_alGetProcAddress ("alGetDebugMessageLogEXT"));
        alad_alObjectLabelEXT               = REINTERPRET_CAST(LPALOBJECTLABELEXT, compat_alGetProcAddress ("alObjectLabelEXT"));
        alad_alGetObjectLabelEXT            = REINTERPRET_CAST(LPALGETOBJECTLABELEXT, compat_alGetProcAddress ("alGetObjectLabelEXT"));
}

static void alad_load_alc_functions_contextfree_dlsym_ (alad_module_t_ module) {
        if (module == nullptr) return;
        /* Core ALC */
        alad_alcGetProcAddress     = REINTERPRET_CAST(LPALCGETPROCADDRESS, alad_load_ (module, "alcGetProcAddress"));
        alad_alcCreateContext      = REINTERPRET_CAST(LPALCCREATECONTEXT, alad_load_ (module, "alcCreateContext"));
        alad_alcMakeContextCurrent = REINTERPRET_CAST(LPALCMAKECONTEXTCURRENT, alad_load_ (module, "alcMakeContextCurrent"));
        alad_alcProcessContext     = REINTERPRET_CAST(LPALCPROCESSCONTEXT, alad_load_ (module, "alcProcessContext"));
        alad_alcSuspendContext     = REINTERPRET_CAST(LPALCSUSPENDCONTEXT, alad_load_ (module, "alcSuspendContext"));
        alad_alcDestroyContext     = REINTERPRET_CAST(LPALCDESTROYCONTEXT, alad_load_ (module, "alcDestroyContext"));
        alad_alcGetCurrentContext  = REINTERPRET_CAST(LPALCGETCURRENTCONTEXT, alad_load_ (module, "alcGetCurrentContext"));
        alad_alcGetContextsDevice  = REINTERPRET_CAST(LPALCGETCONTEXTSDEVICE, alad_load_ (module, "alcGetContextsDevice"));
        alad_alcOpenDevice         = REINTERPRET_CAST(LPALCOPENDEVICE, alad_load_ (module, "alcOpenDevice"));
        alad_alcCloseDevice        = REINTERPRET_CAST(LPALCCLOSEDEVICE, alad_load_ (module, "alcCloseDevice"));
        alad_alcGetError           = REINTERPRET_CAST(LPALCGETERROR, alad_load_ (module, "alcGetError"));
        alad_alcIsExtensionPresent = REINTERPRET_CAST(LPALCISEXTENSIONPRESENT, alad_load_ (module, "alcIsExtensionPresent"));
        alad_alcGetEnumValue       = REINTERPRET_CAST(LPALCGETENUMVALUE, alad_load_ (module, "alcGetEnumValue"));
        alad_alcGetString          = REINTERPRET_CAST(LPALCGETSTRING, alad_load_ (module, "alcGetString"));
        alad_alcGetIntegerv        = REINTERPRET_CAST(LPALCGETINTEGERV, alad_load_ (module, "alcGetIntegerv"));
        alad_alcCaptureOpenDevice  = REINTERPRET_CAST(LPALCCAPTUREOPENDEVICE, alad_load_ (module, "alcCaptureOpenDevice"));
        alad_alcCaptureCloseDevice = REINTERPRET_CAST(LPALCCAPTURECLOSEDEVICE, alad_load_ (module, "alcCaptureCloseDevice"));
        alad_alcCaptureStart       = REINTERPRET_CAST(LPALCCAPTURESTART, alad_load_ (module, "alcCaptureStart"));
        alad_alcCaptureStop        = REINTERPRET_CAST(LPALCCAPTURESTOP, alad_load_ (module, "alcCaptureStop"));
        alad_alcCaptureSamples     = REINTERPRET_CAST(LPALCCAPTURESAMPLES, alad_load_ (module, "alcCaptureSamples"));
}


static void alad_load_alc_functions_from_al_ () {
        ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_ compat_alGetProcAddress;
        if (alGetProcAddress == nullptr) return;
        compat_alGetProcAddress = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_, alad_alGetProcAddress);
        /* Only load function loader once, and only if not already loaded from shared library */
        if (alad_alcGetProcAddress == nullptr) {
                alad_alcGetProcAddress = REINTERPRET_CAST(LPALCGETPROCADDRESS, compat_alGetProcAddress ("alcGetProcAddress"));
        }
        /* Core ALC */
        alad_alcCreateContext      = REINTERPRET_CAST(LPALCCREATECONTEXT, compat_alGetProcAddress ("alcCreateContext"));
        alad_alcMakeContextCurrent = REINTERPRET_CAST(LPALCMAKECONTEXTCURRENT, compat_alGetProcAddress ("alcMakeContextCurrent"));
        alad_alcProcessContext     = REINTERPRET_CAST(LPALCPROCESSCONTEXT, compat_alGetProcAddress ("alcProcessContext"));
        alad_alcSuspendContext     = REINTERPRET_CAST(LPALCSUSPENDCONTEXT, compat_alGetProcAddress ("alcSuspendContext"));
        alad_alcDestroyContext     = REINTERPRET_CAST(LPALCDESTROYCONTEXT, compat_alGetProcAddress ("alcDestroyContext"));
        alad_alcGetCurrentContext  = REINTERPRET_CAST(LPALCGETCURRENTCONTEXT, compat_alGetProcAddress ("alcGetCurrentContext"));
        alad_alcGetContextsDevice  = REINTERPRET_CAST(LPALCGETCONTEXTSDEVICE, compat_alGetProcAddress ("alcGetContextsDevice"));
        alad_alcOpenDevice         = REINTERPRET_CAST(LPALCOPENDEVICE, compat_alGetProcAddress ("alcOpenDevice"));
        alad_alcCloseDevice        = REINTERPRET_CAST(LPALCCLOSEDEVICE, compat_alGetProcAddress ("alcCloseDevice"));
        alad_alcGetError           = REINTERPRET_CAST(LPALCGETERROR, compat_alGetProcAddress ("alcGetError"));
        alad_alcIsExtensionPresent = REINTERPRET_CAST(LPALCISEXTENSIONPRESENT, compat_alGetProcAddress ("alcIsExtensionPresent"));
        alad_alcGetEnumValue       = REINTERPRET_CAST(LPALCGETENUMVALUE, compat_alGetProcAddress ("alcGetEnumValue"));
        alad_alcGetString          = REINTERPRET_CAST(LPALCGETSTRING, compat_alGetProcAddress ("alcGetString"));
        alad_alcGetIntegerv        = REINTERPRET_CAST(LPALCGETINTEGERV, compat_alGetProcAddress ("alcGetIntegerv"));
        alad_alcCaptureOpenDevice  = REINTERPRET_CAST(LPALCCAPTUREOPENDEVICE, compat_alGetProcAddress ("alcCaptureOpenDevice"));
        alad_alcCaptureCloseDevice = REINTERPRET_CAST(LPALCCAPTURECLOSEDEVICE, compat_alGetProcAddress ("alcCaptureCloseDevice"));
        alad_alcCaptureStart       = REINTERPRET_CAST(LPALCCAPTURESTART, compat_alGetProcAddress ("alcCaptureStart"));
        alad_alcCaptureStop        = REINTERPRET_CAST(LPALCCAPTURESTOP, compat_alGetProcAddress ("alcCaptureStop"));
        alad_alcCaptureSamples     = REINTERPRET_CAST(LPALCCAPTURESAMPLES, compat_alGetProcAddress ("alcCaptureSamples"));
}


static void alad_load_alc_extension_functions_from_al_ () {
        ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_ compat_alGetProcAddress;
        if (alGetProcAddress == nullptr) return;
        compat_alGetProcAddress = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_, alad_alGetProcAddress);
        /* ALC extensions */
        /* ALC_EXT_thread_local_context */
        alad_alcSetThreadContext                                           = REINTERPRET_CAST(PFNALCSETTHREADCONTEXTPROC, compat_alGetProcAddress ("alcSetThreadContext"));
        alad_alcGetThreadContext                                           = REINTERPRET_CAST(PFNALCGETTHREADCONTEXTPROC, compat_alGetProcAddress ("alcGetThreadContext"));
        /* ALC_SOFT_loopback */
        alad_alcLoopbackOpenDeviceSOFT                                     = REINTERPRET_CAST(LPALCLOOPBACKOPENDEVICESOFT, compat_alGetProcAddress ("alcLoopbackOpenDeviceSOFT"));
        alad_alcIsRenderFormatSupportedSOFT = REINTERPRET_CAST(LPALCISRENDERFORMATSUPPORTEDSOFT, compat_alGetProcAddress ("alcIsRenderFormatSupportedSOFT"));
        alad_alcRenderSamplesSOFT           = REINTERPRET_CAST(LPALCRENDERSAMPLESSOFT, compat_alGetProcAddress ("alcRenderSamplesSOFT"));
        /* ALC_SOFT_pause_device */
        alad_alcDevicePauseSOFT             = REINTERPRET_CAST(LPALCDEVICEPAUSESOFT, compat_alGetProcAddress ("alcDevicePauseSOFT"));
        alad_alcDeviceResumeSOFT            = REINTERPRET_CAST(LPALCDEVICERESUMESOFT, compat_alGetProcAddress ("alcDeviceResumeSOFT"));
        /* ALC_SOFT_HRTF */
        alad_alcGetStringiSOFT              = REINTERPRET_CAST(LPALCGETSTRINGISOFT, compat_alGetProcAddress ("alcGetStringiSOFT"));
        alad_alcResetDeviceSOFT             = REINTERPRET_CAST(LPALCRESETDEVICESOFT, compat_alGetProcAddress ("alcResetDeviceSOFT"));
        /* ALC_SOFT_device_clock */
        alad_alcGetInteger64vSOFT           = REINTERPRET_CAST(LPALCGETINTEGER64VSOFT, compat_alGetProcAddress ("alcGetInteger64vSOFT"));
        /* ALC_SOFT_reopen_device */
        alad_alcReopenDeviceSOFT            = REINTERPRET_CAST(LPALCREOPENDEVICESOFT, compat_alGetProcAddress ("alcReopenDeviceSOFT"));
        /* ALC_SOFT_system_events */
        alad_alcEventIsSupportedSOFT        = REINTERPRET_CAST(LPALCEVENTISSUPPORTEDSOFT, compat_alGetProcAddress ("alcEventIsSupportedSOFT"));
        alad_alcEventControlSOFT            = REINTERPRET_CAST(LPALCEVENTCONTROLSOFT, compat_alGetProcAddress ("alcEventControlSOFT"));
        alad_alcEventCallbackSOFT           = REINTERPRET_CAST(LPALCEVENTCALLBACKSOFT, compat_alGetProcAddress ("alcEventCallbackSOFT"));
}

static void alad_load_alc_functions_ (ALCdevice *device) {
        ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS_ compat_alcGetProcAddress;
        /* old loader code left for future use */
        /*
    if((alad_alGetProcAddress == nullptr) && (alad_alcGetProcAddress == nullptr)) return;
    if(alad_alcGetProcAddress == nullptr) {
        alad_alcGetProcAddress = REINTERPRET_CAST(LPALCGETPROCADDRESS              , alad_alGetProcAddress("alcGetProcAddress"));
    }
    */
        if (alad_alcGetProcAddress == nullptr) return;
        compat_alcGetProcAddress = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS_, alad_alcGetProcAddress);
        /* Core ALC */
        alad_alcCreateContext                                                = REINTERPRET_CAST(LPALCCREATECONTEXT, compat_alcGetProcAddress (device, "alcCreateContext"));
        alad_alcMakeContextCurrent                                           = REINTERPRET_CAST(LPALCMAKECONTEXTCURRENT, compat_alcGetProcAddress (device, "alcMakeContextCurrent"));
        alad_alcProcessContext                                               = REINTERPRET_CAST(LPALCPROCESSCONTEXT, compat_alcGetProcAddress (device, "alcProcessContext"));
        alad_alcSuspendContext                                               = REINTERPRET_CAST(LPALCSUSPENDCONTEXT, compat_alcGetProcAddress (device, "alcSuspendContext"));
        alad_alcDestroyContext                                               = REINTERPRET_CAST(LPALCDESTROYCONTEXT, compat_alcGetProcAddress (device, "alcDestroyContext"));
        alad_alcGetCurrentContext                                            = REINTERPRET_CAST(LPALCGETCURRENTCONTEXT, compat_alcGetProcAddress (device, "alcGetCurrentContext"));
        alad_alcGetContextsDevice                                            = REINTERPRET_CAST(LPALCGETCONTEXTSDEVICE, compat_alcGetProcAddress (device, "alcGetContextsDevice"));
        alad_alcOpenDevice                                                   = REINTERPRET_CAST(LPALCOPENDEVICE, compat_alcGetProcAddress (device, "alcOpenDevice"));
        alad_alcCloseDevice                                                  = REINTERPRET_CAST(LPALCCLOSEDEVICE, compat_alcGetProcAddress (device, "alcCloseDevice"));
        alad_alcGetError                                                     = REINTERPRET_CAST(LPALCGETERROR, compat_alcGetProcAddress (device, "alcGetError"));
        alad_alcIsExtensionPresent                                           = REINTERPRET_CAST(LPALCISEXTENSIONPRESENT, compat_alcGetProcAddress (device, "alcIsExtensionPresent"));
        alad_alcGetEnumValue                                                 = REINTERPRET_CAST(LPALCGETENUMVALUE, compat_alcGetProcAddress (device, "alcGetEnumValue"));
        alad_alcGetString                                                    = REINTERPRET_CAST(LPALCGETSTRING, compat_alcGetProcAddress (device, "alcGetString"));
        alad_alcGetIntegerv                                                  = REINTERPRET_CAST(LPALCGETINTEGERV, compat_alcGetProcAddress (device, "alcGetIntegerv"));
        alad_alcCaptureOpenDevice                                            = REINTERPRET_CAST(LPALCCAPTUREOPENDEVICE, compat_alcGetProcAddress (device, "alcCaptureOpenDevice"));
        alad_alcCaptureCloseDevice                                           = REINTERPRET_CAST(LPALCCAPTURECLOSEDEVICE, compat_alcGetProcAddress (device, "alcCaptureCloseDevice"));
        alad_alcCaptureStart                                                 = REINTERPRET_CAST(LPALCCAPTURESTART, compat_alcGetProcAddress (device, "alcCaptureStart"));
        alad_alcCaptureStop                                                  = REINTERPRET_CAST(LPALCCAPTURESTOP, compat_alcGetProcAddress (device, "alcCaptureStop"));
        alad_alcCaptureSamples                                               = REINTERPRET_CAST(LPALCCAPTURESAMPLES, compat_alcGetProcAddress (device, "alcCaptureSamples"));
}

static void alad_load_alc_extension_functions_ (ALCdevice *device) {
        ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS_ compat_alcGetProcAddress;
        if (alad_alcGetProcAddress == nullptr) return;
        compat_alcGetProcAddress = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS_, alad_alcGetProcAddress);
        /* ALC extensions */
        /* ALC_EXT_thread_local_context */
        alad_alcSetThreadContext                                             = REINTERPRET_CAST(PFNALCSETTHREADCONTEXTPROC, compat_alcGetProcAddress (device, "alcSetThreadContext"));
        alad_alcGetThreadContext                                             = REINTERPRET_CAST(PFNALCGETTHREADCONTEXTPROC, compat_alcGetProcAddress (device, "alcGetThreadContext"));
        /* ALC_SOFT_loopback */
        alad_alcLoopbackOpenDeviceSOFT      = REINTERPRET_CAST(LPALCLOOPBACKOPENDEVICESOFT, compat_alcGetProcAddress (device, "alcLoopbackOpenDeviceSOFT"));
        alad_alcIsRenderFormatSupportedSOFT = REINTERPRET_CAST(LPALCISRENDERFORMATSUPPORTEDSOFT, compat_alcGetProcAddress (device, "alcIsRenderFormatSupportedSOFT"));
        alad_alcRenderSamplesSOFT           = REINTERPRET_CAST(LPALCRENDERSAMPLESSOFT, compat_alcGetProcAddress (device, "alcRenderSamplesSOFT"));
        /* ALC_SOFT_pause_device */
        alad_alcDevicePauseSOFT             = REINTERPRET_CAST(LPALCDEVICEPAUSESOFT, compat_alcGetProcAddress (device, "alcDevicePauseSOFT"));
        alad_alcDeviceResumeSOFT            = REINTERPRET_CAST(LPALCDEVICERESUMESOFT, compat_alcGetProcAddress (device, "alcDeviceResumeSOFT"));
        /* ALC_SOFT_HRTF */
        alad_alcGetStringiSOFT              = REINTERPRET_CAST(LPALCGETSTRINGISOFT, compat_alcGetProcAddress (device, "alcGetStringiSOFT"));
        alad_alcResetDeviceSOFT             = REINTERPRET_CAST(LPALCRESETDEVICESOFT, compat_alcGetProcAddress (device, "alcResetDeviceSOFT"));
        /* ALC_SOFT_device_clock */
        alad_alcGetInteger64vSOFT           = REINTERPRET_CAST(LPALCGETINTEGER64VSOFT, compat_alcGetProcAddress (device, "alcGetInteger64vSOFT"));
        /* ALC_SOFT_reopen_device */
        alad_alcReopenDeviceSOFT            = REINTERPRET_CAST(LPALCREOPENDEVICESOFT, compat_alcGetProcAddress (device, "alcReopenDeviceSOFT"));
        /* ALC_SOFT_system_events */
        alad_alcEventIsSupportedSOFT        = REINTERPRET_CAST(LPALCEVENTISSUPPORTEDSOFT, compat_alcGetProcAddress (device, "alcEventIsSupportedSOFT"));
        alad_alcEventControlSOFT            = REINTERPRET_CAST(LPALCEVENTCONTROLSOFT, compat_alcGetProcAddress (device, "alcEventControlSOFT"));
        alad_alcEventCallbackSOFT           = REINTERPRET_CAST(LPALCEVENTCALLBACKSOFT, compat_alcGetProcAddress (device, "alcEventCallbackSOFT"));
}





static alad_module_t_ alad_module_ = nullptr;

static void  alad_load_lib_ () {
        /* don't load shared object twice */
        if (alad_module_ != nullptr) return;
        /* use fallback so name */
        alad_module_ = alad_open_ (alad_LIB_NAME_);
        if (alad_module_ == nullptr) {
                alad_module_ = alad_open_ (alad_SECONDARY_LIB_NAME_);
        }
}

static LPALGETPROCADDRESS alad_initalloader_ () {
        alad_load_lib_();
        if (alad_module_ == nullptr) {
                return nullptr;
        }
        return REINTERPRET_CAST(LPALGETPROCADDRESS, alad_load_ (alad_module_, "alGetProcAddress"));
}

static void alad_unload_lib_ () {
        if (alad_module_ != nullptr) alad_close_ (alad_module_);
}



/* manual interface */
void aladLoadALContextFree (ALboolean loadAll) {
        alad_load_lib_();
        alad_load_al_functions_contextfree_dlsym_ (alad_module_, loadAll);
        alad_load_alc_functions_contextfree_dlsym_ (alad_module_);
}

void aladLoadALFromLoaderFunction (LPALGETPROCADDRESS inital_loader) {
        if (inital_loader != nullptr) {
                alGetProcAddress = inital_loader;
        } else if (alGetProcAddress == nullptr) {
                alGetProcAddress = alad_initalloader_();
        }
        alad_load_al_functions_();
        alad_load_alc_functions_from_al_();
}


void aladUpdateALPointers (ALCcontext *context, ALboolean extensionsOnly) {
        ALboolean   switchContext;
        ALCcontext *oldContext;
        
        switchContext = (context != nullptr);
        oldContext = nullptr;
        if (switchContext) {
                oldContext = alad_alcGetCurrentContext();
                alad_alcMakeContextCurrent (context);
        }
        if (extensionsOnly == AL_FALSE) {
                alad_load_al_functions_();
        }
        alad_load_al_extension_functions_();
        if (switchContext) {
                alad_alcMakeContextCurrent (oldContext);
        }
}

void aladUpdateALCPointersFromContext (ALCcontext *context, ALboolean extensionsOnly) {
        ALboolean   switchContext;
        ALCcontext *oldContext;
        
        switchContext = (context != nullptr);
        oldContext = nullptr;
        if (switchContext) {
                oldContext = alad_alcGetCurrentContext();
                alad_alcMakeContextCurrent (context);
        }
        if (extensionsOnly == AL_FALSE) {
                alad_load_alc_functions_from_al_();
        }
        alad_load_alc_extension_functions_from_al_();
        if (switchContext) {
                alad_alcMakeContextCurrent (oldContext);
        }
}

void aladUpdateALCPointersFromDevice (ALCdevice *device, ALboolean extensionsOnly) {
        if (extensionsOnly == AL_FALSE) {
                alad_load_alc_functions_ (device);
        }
        alad_load_alc_extension_functions_ (device);
}

void aladTerminate () {
        alad_unload_lib_();
}



/* simplified Interface */
void aladLoadAL () {
        aladLoadALContextFree (AL_TRUE);
}
void aladUpdateAL () {
        ALCdevice *device;
        
        /* load extensions with alGetProcAddress */
        alad_load_al_extension_functions_();
        /* use current contexts device to load ALC extensions with alcGetProcAddress */
        device = alad_alcGetContextsDevice (alad_alcGetCurrentContext());
        alad_load_alc_extension_functions_ (device);
}



#endif        /* ALAD_IMPLEMENTATION */

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif        /* ALAD_H */
