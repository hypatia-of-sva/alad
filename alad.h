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
 *  alad can't currently check on whether or not the headers work properly, but compilation will fail due to double definition.
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

//only include alad if the OpenAL headers are not used, because we include them again, and they may have been included with prototypes
#if !(defined(ALAD_H)) && !(defined(AL_AL_H)) && !(defined(AL_ALC_H)) && !(defined(AL_ALEXT_H)) && !(defined(AL_EFX_H)) && !(defined(EFX_PRESETS_H))
#define ALAD_H

//revision date
#define ALAD_HEADER_REVISION 0x20221230

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
//currently it can't be checked whether or not the headers work properly, but compilation will fail due to double definition.



//Public Interface:


//simplified Interface
extern void                             aladLoadAL();
extern void                             aladUpdateAL();

//manual interface
extern void                             aladLoadALContextFree (ALboolean loadAll);
extern void                             aladLoadALFromLoaderFunction (LPALGETPROCADDRESS inital_loader);
extern void                             aladUpdateALPointers (ALCcontext *context, ALboolean extensionsOnly);
extern void                             aladUpdateALCPointersFromContext (ALCcontext *context, ALboolean extensionsOnly);
extern void                             aladUpdateALCPointersFromDevice (ALCdevice *device, ALboolean extensionsOnly);

//shared function
extern void                             aladTerminate();

//Core AL
extern LPALDOPPLERFACTOR                alDopplerFactor;
extern LPALDOPPLERVELOCITY              alDopplerVelocity;
extern LPALSPEEDOFSOUND                 alSpeedOfSound;
extern LPALDISTANCEMODEL                alDistanceModel;
extern LPALENABLE                       alEnable;
extern LPALDISABLE                      alDisable;
extern LPALISENABLED                    alIsEnabled;
extern LPALGETSTRING                    alGetString;
extern LPALGETBOOLEANV                  alGetBooleanv;
extern LPALGETINTEGERV                  alGetIntegerv;
extern LPALGETFLOATV                    alGetFloatv;
extern LPALGETDOUBLEV                   alGetDoublev;
extern LPALGETBOOLEAN                   alGetBoolean;
extern LPALGETINTEGER                   alGetInteger;
extern LPALGETFLOAT                     alGetFloat;
extern LPALGETDOUBLE                    alGetDouble;
extern LPALGETERROR                     alGetError;
extern LPALISEXTENSIONPRESENT           alIsExtensionPresent;
extern LPALGETPROCADDRESS               alGetProcAddress;
extern LPALGETENUMVALUE                 alGetEnumValue;
extern LPALLISTENERF                    alListenerf;
extern LPALLISTENER3F                   alListener3f;
extern LPALLISTENERFV                   alListenerfv;
extern LPALLISTENERI                    alListeneri;
extern LPALLISTENER3I                   alListener3i;
extern LPALLISTENERIV                   alListeneriv;
extern LPALGETLISTENERF                 alGetListenerf;
extern LPALGETLISTENER3F                alGetListener3f;
extern LPALGETLISTENERFV                alGetListenerfv;
extern LPALGETLISTENERI                 alGetListeneri;
extern LPALGETLISTENER3I                alGetListener3i;
extern LPALGETLISTENERIV                alGetListeneriv;
extern LPALGENSOURCES                   alGenSources;
extern LPALDELETESOURCES                alDeleteSources;
extern LPALISSOURCE                     alIsSource;
extern LPALSOURCEF                      alSourcef;
extern LPALSOURCE3F                     alSource3f;
extern LPALSOURCEFV                     alSourcefv;
extern LPALSOURCEI                      alSourcei;
extern LPALSOURCE3I                     alSource3i;
extern LPALSOURCEIV                     alSourceiv;
extern LPALGETSOURCEF                   alGetSourcef;
extern LPALGETSOURCE3F                  alGetSource3f;
extern LPALGETSOURCEFV                  alGetSourcefv;
extern LPALGETSOURCEI                   alGetSourcei;
extern LPALGETSOURCE3I                  alGetSource3i;
extern LPALGETSOURCEIV                  alGetSourceiv;
extern LPALSOURCEPLAYV                  alSourcePlayv;
extern LPALSOURCESTOPV                  alSourceStopv;
extern LPALSOURCEREWINDV                alSourceRewindv;
extern LPALSOURCEPAUSEV                 alSourcePausev;
extern LPALSOURCEPLAY                   alSourcePlay;
extern LPALSOURCESTOP                   alSourceStop;
extern LPALSOURCEREWIND                 alSourceRewind;
extern LPALSOURCEPAUSE                  alSourcePause;
extern LPALSOURCEQUEUEBUFFERS           alSourceQueueBuffers;
extern LPALSOURCEUNQUEUEBUFFERS         alSourceUnqueueBuffers;
extern LPALGENBUFFERS                   alGenBuffers;
extern LPALDELETEBUFFERS                alDeleteBuffers;
extern LPALISBUFFER                     alIsBuffer;
extern LPALBUFFERDATA                   alBufferData;
extern LPALBUFFERF                      alBufferf;
extern LPALBUFFER3F                     alBuffer3f;
extern LPALBUFFERFV                     alBufferfv;
extern LPALBUFFERI                      alBufferi;
extern LPALBUFFER3I                     alBuffer3i;
extern LPALBUFFERIV                     alBufferiv;
extern LPALGETBUFFERF                   alGetBufferf;
extern LPALGETBUFFER3F                  alGetBuffer3f;
extern LPALGETBUFFERFV                  alGetBufferfv;
extern LPALGETBUFFERI                   alGetBufferi;
extern LPALGETBUFFER3I                  alGetBuffer3i;
extern LPALGETBUFFERIV                  alGetBufferiv;

//EFX
extern LPALGENEFFECTS                   alGenEffects;
extern LPALDELETEEFFECTS                alDeleteEffects;
extern LPALISEFFECT                     alIsEffect;
extern LPALEFFECTI                      alEffecti;
extern LPALEFFECTIV                     alEffectiv;
extern LPALEFFECTF                      alEffectf;
extern LPALEFFECTFV                     alEffectfv;
extern LPALGETEFFECTI                   alGetEffecti;
extern LPALGETEFFECTIV                  alGetEffectiv;
extern LPALGETEFFECTF                   alGetEffectf;
extern LPALGETEFFECTFV                  alGetEffectfv;
extern LPALGENFILTERS                   alGenFilters;
extern LPALDELETEFILTERS                alDeleteFilters;
extern LPALISFILTER                     alIsFilter;
extern LPALFILTERI                      alFilteri;
extern LPALFILTERIV                     alFilteriv;
extern LPALFILTERF                      alFilterf;
extern LPALFILTERFV                     alFilterfv;
extern LPALGETFILTERI                   alGetFilteri;
extern LPALGETFILTERIV                  alGetFilteriv;
extern LPALGETFILTERF                   alGetFilterf;
extern LPALGETFILTERFV                  alGetFilterfv;
extern LPALGENAUXILIARYEFFECTSLOTS      alGenAuxiliaryEffectSlots;
extern LPALDELETEAUXILIARYEFFECTSLOTS   alDeleteAuxiliaryEffectSlots;
extern LPALISAUXILIARYEFFECTSLOT        alIsAuxiliaryEffectSlot;
extern LPALAUXILIARYEFFECTSLOTI         alAuxiliaryEffectSloti;
extern LPALAUXILIARYEFFECTSLOTIV        alAuxiliaryEffectSlotiv;
extern LPALAUXILIARYEFFECTSLOTF         alAuxiliaryEffectSlotf;
extern LPALAUXILIARYEFFECTSLOTFV        alAuxiliaryEffectSlotfv;
extern LPALGETAUXILIARYEFFECTSLOTI      alGetAuxiliaryEffectSloti;
extern LPALGETAUXILIARYEFFECTSLOTIV     alGetAuxiliaryEffectSlotiv;
extern LPALGETAUXILIARYEFFECTSLOTF      alGetAuxiliaryEffectSlotf;
extern LPALGETAUXILIARYEFFECTSLOTFV     alGetAuxiliaryEffectSlotfv;

//AL extensions
//AL_EXT_STATIC_BUFFER
extern PFNALBUFFERDATASTATICPROC        alBufferDataStatic;
//AL_SOFT_buffer_sub_data
extern PFNALBUFFERSUBDATASOFTPROC       alBufferSubDataSOFT;
//AL_EXT_FOLDBACK
extern LPALREQUESTFOLDBACKSTART         alRequestFoldbackStart;
extern LPALREQUESTFOLDBACKSTOP          alRequestFoldbackStop;
//AL_SOFT_buffer_samples
extern LPALBUFFERSAMPLESSOFT            alBufferSamplesSOFT;
extern LPALBUFFERSUBSAMPLESSOFT         alBufferSubSamplesSOFT;
extern LPALGETBUFFERSAMPLESSOFT         alGetBufferSamplesSOFT;
extern LPALISBUFFERFORMATSUPPORTEDSOFT  alIsBufferFormatSupportedSOFT;
//AL_SOFT_source_latency
extern LPALSOURCEDSOFT                  alSourcedSOFT;
extern LPALSOURCE3DSOFT                 alSource3dSOFT;
extern LPALSOURCEDVSOFT                 alSourcedvSOFT;
extern LPALGETSOURCEDSOFT               alGetSourcedSOFT;
extern LPALGETSOURCE3DSOFT              alGetSource3dSOFT;
extern LPALGETSOURCEDVSOFT              alGetSourcedvSOFT;
extern LPALSOURCEI64SOFT                alSourcei64SOFT;
extern LPALSOURCE3I64SOFT               alSource3i64SOFT;
extern LPALSOURCEI64VSOFT               alSourcei64vSOFT;
extern LPALGETSOURCEI64SOFT             alGetSourcei64SOFT;
extern LPALGETSOURCE3I64SOFT            alGetSource3i64SOFT;
extern LPALGETSOURCEI64VSOFT            alGetSourcei64vSOFT;
//AL_SOFT_deferred_updates
extern LPALDEFERUPDATESSOFT             alDeferUpdatesSOFT;
extern LPALPROCESSUPDATESSOFT           alProcessUpdatesSOFT;
//AL_SOFT_source_resampler
extern LPALGETSTRINGISOFT               alGetStringiSOFT;
//AL_SOFT_events
extern LPALEVENTCONTROLSOFT             alEventControlSOFT;
extern LPALEVENTCALLBACKSOFT            alEventCallbackSOFT;
extern LPALGETPOINTERSOFT               alGetPointerSOFT;
extern LPALGETPOINTERVSOFT              alGetPointervSOFT;
//AL_SOFT_callback_buffer
extern LPALBUFFERCALLBACKSOFT           alBufferCallbackSOFT;
extern LPALGETBUFFERPTRSOFT             alGetBufferPtrSOFT;
extern LPALGETBUFFER3PTRSOFT            alGetBuffer3PtrSOFT;
extern LPALGETBUFFERPTRVSOFT            alGetBufferPtrvSOFT;

//Core ALC
extern LPALCCREATECONTEXT               alcCreateContext;
extern LPALCMAKECONTEXTCURRENT          alcMakeContextCurrent;
extern LPALCPROCESSCONTEXT              alcProcessContext;
extern LPALCSUSPENDCONTEXT              alcSuspendContext;
extern LPALCDESTROYCONTEXT              alcDestroyContext;
extern LPALCGETCURRENTCONTEXT           alcGetCurrentContext;
extern LPALCGETCONTEXTSDEVICE           alcGetContextsDevice;
extern LPALCOPENDEVICE                  alcOpenDevice;
extern LPALCCLOSEDEVICE                 alcCloseDevice;
extern LPALCGETERROR                    alcGetError;
extern LPALCISEXTENSIONPRESENT          alcIsExtensionPresent;
extern LPALCGETPROCADDRESS              alcGetProcAddress;
extern LPALCGETENUMVALUE                alcGetEnumValue;
extern LPALCGETSTRING                   alcGetString;
extern LPALCGETINTEGERV                 alcGetIntegerv;
extern LPALCCAPTUREOPENDEVICE           alcCaptureOpenDevice;
extern LPALCCAPTURECLOSEDEVICE          alcCaptureCloseDevice;
extern LPALCCAPTURESTART                alcCaptureStart;
extern LPALCCAPTURESTOP                 alcCaptureStop;
extern LPALCCAPTURESAMPLES              alcCaptureSamples;

//ALC extensions
//ALC_EXT_thread_local_context
extern PFNALCSETTHREADCONTEXTPROC       alcSetThreadContext;
extern PFNALCGETTHREADCONTEXTPROC       alcGetThreadContext;
//ALC_SOFT_loopback
extern LPALCLOOPBACKOPENDEVICESOFT      alcLoopbackOpenDeviceSOFT;
extern LPALCISRENDERFORMATSUPPORTEDSOFT alcIsRenderFormatSupportedSOFT;
extern LPALCRENDERSAMPLESSOFT           alcRenderSamplesSOFT;
//ALC_SOFT_pause_device
extern LPALCDEVICEPAUSESOFT             alcDevicePauseSOFT;
extern LPALCDEVICERESUMESOFT            alcDeviceResumeSOFT;
//ALC_SOFT_HRTF
extern LPALCGETSTRINGISOFT              alcGetStringiSOFT;
extern LPALCRESETDEVICESOFT             alcResetDeviceSOFT;
//ALC_SOFT_device_clock
extern LPALCGETINTEGER64VSOFT           alcGetInteger64vSOFT;
//ALC_SOFT_reopen_device
extern LPALCREOPENDEVICESOFT            alcReopenDeviceSOFT;
















#ifdef ALAD_IMPLEMENTATION

//Core AL
LPALDOPPLERFACTOR                alDopplerFactor                = nullptr;
LPALDOPPLERVELOCITY              alDopplerVelocity              = nullptr;
LPALSPEEDOFSOUND                 alSpeedOfSound                 = nullptr;
LPALDISTANCEMODEL                alDistanceModel                = nullptr;
LPALENABLE                       alEnable                       = nullptr;
LPALDISABLE                      alDisable                      = nullptr;
LPALISENABLED                    alIsEnabled                    = nullptr;
LPALGETSTRING                    alGetString                    = nullptr;
LPALGETBOOLEANV                  alGetBooleanv                  = nullptr;
LPALGETINTEGERV                  alGetIntegerv                  = nullptr;
LPALGETFLOATV                    alGetFloatv                    = nullptr;
LPALGETDOUBLEV                   alGetDoublev                   = nullptr;
LPALGETBOOLEAN                   alGetBoolean                   = nullptr;
LPALGETINTEGER                   alGetInteger                   = nullptr;
LPALGETFLOAT                     alGetFloat                     = nullptr;
LPALGETDOUBLE                    alGetDouble                    = nullptr;
LPALGETERROR                     alGetError                     = nullptr;
LPALISEXTENSIONPRESENT           alIsExtensionPresent           = nullptr;
LPALGETPROCADDRESS               alGetProcAddress               = nullptr;
LPALGETENUMVALUE                 alGetEnumValue                 = nullptr;
LPALLISTENERF                    alListenerf                    = nullptr;
LPALLISTENER3F                   alListener3f                   = nullptr;
LPALLISTENERFV                   alListenerfv                   = nullptr;
LPALLISTENERI                    alListeneri                    = nullptr;
LPALLISTENER3I                   alListener3i                   = nullptr;
LPALLISTENERIV                   alListeneriv                   = nullptr;
LPALGETLISTENERF                 alGetListenerf                 = nullptr;
LPALGETLISTENER3F                alGetListener3f                = nullptr;
LPALGETLISTENERFV                alGetListenerfv                = nullptr;
LPALGETLISTENERI                 alGetListeneri                 = nullptr;
LPALGETLISTENER3I                alGetListener3i                = nullptr;
LPALGETLISTENERIV                alGetListeneriv                = nullptr;
LPALGENSOURCES                   alGenSources                   = nullptr;
LPALDELETESOURCES                alDeleteSources                = nullptr;
LPALISSOURCE                     alIsSource                     = nullptr;
LPALSOURCEF                      alSourcef                      = nullptr;
LPALSOURCE3F                     alSource3f                     = nullptr;
LPALSOURCEFV                     alSourcefv                     = nullptr;
LPALSOURCEI                      alSourcei                      = nullptr;
LPALSOURCE3I                     alSource3i                     = nullptr;
LPALSOURCEIV                     alSourceiv                     = nullptr;
LPALGETSOURCEF                   alGetSourcef                   = nullptr;
LPALGETSOURCE3F                  alGetSource3f                  = nullptr;
LPALGETSOURCEFV                  alGetSourcefv                  = nullptr;
LPALGETSOURCEI                   alGetSourcei                   = nullptr;
LPALGETSOURCE3I                  alGetSource3i                  = nullptr;
LPALGETSOURCEIV                  alGetSourceiv                  = nullptr;
LPALSOURCEPLAYV                  alSourcePlayv                  = nullptr;
LPALSOURCESTOPV                  alSourceStopv                  = nullptr;
LPALSOURCEREWINDV                alSourceRewindv                = nullptr;
LPALSOURCEPAUSEV                 alSourcePausev                 = nullptr;
LPALSOURCEPLAY                   alSourcePlay                   = nullptr;
LPALSOURCESTOP                   alSourceStop                   = nullptr;
LPALSOURCEREWIND                 alSourceRewind                 = nullptr;
LPALSOURCEPAUSE                  alSourcePause                  = nullptr;
LPALSOURCEQUEUEBUFFERS           alSourceQueueBuffers           = nullptr;
LPALSOURCEUNQUEUEBUFFERS         alSourceUnqueueBuffers         = nullptr;
LPALGENBUFFERS                   alGenBuffers                   = nullptr;
LPALDELETEBUFFERS                alDeleteBuffers                = nullptr;
LPALISBUFFER                     alIsBuffer                     = nullptr;
LPALBUFFERDATA                   alBufferData                   = nullptr;
LPALBUFFERF                      alBufferf                      = nullptr;
LPALBUFFER3F                     alBuffer3f                     = nullptr;
LPALBUFFERFV                     alBufferfv                     = nullptr;
LPALBUFFERI                      alBufferi                      = nullptr;
LPALBUFFER3I                     alBuffer3i                     = nullptr;
LPALBUFFERIV                     alBufferiv                     = nullptr;
LPALGETBUFFERF                   alGetBufferf                   = nullptr;
LPALGETBUFFER3F                  alGetBuffer3f                  = nullptr;
LPALGETBUFFERFV                  alGetBufferfv                  = nullptr;
LPALGETBUFFERI                   alGetBufferi                   = nullptr;
LPALGETBUFFER3I                  alGetBuffer3i                  = nullptr;
LPALGETBUFFERIV                  alGetBufferiv                  = nullptr;

//EFX
LPALGENEFFECTS                   alGenEffects                   = nullptr;
LPALDELETEEFFECTS                alDeleteEffects                = nullptr;
LPALISEFFECT                     alIsEffect                     = nullptr;
LPALEFFECTI                      alEffecti                      = nullptr;
LPALEFFECTIV                     alEffectiv                     = nullptr;
LPALEFFECTF                      alEffectf                      = nullptr;
LPALEFFECTFV                     alEffectfv                     = nullptr;
LPALGETEFFECTI                   alGetEffecti                   = nullptr;
LPALGETEFFECTIV                  alGetEffectiv                  = nullptr;
LPALGETEFFECTF                   alGetEffectf                   = nullptr;
LPALGETEFFECTFV                  alGetEffectfv                  = nullptr;
LPALGENFILTERS                   alGenFilters                   = nullptr;
LPALDELETEFILTERS                alDeleteFilters                = nullptr;
LPALISFILTER                     alIsFilter                     = nullptr;
LPALFILTERI                      alFilteri                      = nullptr;
LPALFILTERIV                     alFilteriv                     = nullptr;
LPALFILTERF                      alFilterf                      = nullptr;
LPALFILTERFV                     alFilterfv                     = nullptr;
LPALGETFILTERI                   alGetFilteri                   = nullptr;
LPALGETFILTERIV                  alGetFilteriv                  = nullptr;
LPALGETFILTERF                   alGetFilterf                   = nullptr;
LPALGETFILTERFV                  alGetFilterfv                  = nullptr;
LPALGENAUXILIARYEFFECTSLOTS      alGenAuxiliaryEffectSlots      = nullptr;
LPALDELETEAUXILIARYEFFECTSLOTS   alDeleteAuxiliaryEffectSlots   = nullptr;
LPALISAUXILIARYEFFECTSLOT        alIsAuxiliaryEffectSlot        = nullptr;
LPALAUXILIARYEFFECTSLOTI         alAuxiliaryEffectSloti         = nullptr;
LPALAUXILIARYEFFECTSLOTIV        alAuxiliaryEffectSlotiv        = nullptr;
LPALAUXILIARYEFFECTSLOTF         alAuxiliaryEffectSlotf         = nullptr;
LPALAUXILIARYEFFECTSLOTFV        alAuxiliaryEffectSlotfv        = nullptr;
LPALGETAUXILIARYEFFECTSLOTI      alGetAuxiliaryEffectSloti      = nullptr;
LPALGETAUXILIARYEFFECTSLOTIV     alGetAuxiliaryEffectSlotiv     = nullptr;
LPALGETAUXILIARYEFFECTSLOTF      alGetAuxiliaryEffectSlotf      = nullptr;
LPALGETAUXILIARYEFFECTSLOTFV     alGetAuxiliaryEffectSlotfv     = nullptr;

//AL extensions
//AL_EXT_STATIC_BUFFER
PFNALBUFFERDATASTATICPROC        alBufferDataStatic             = nullptr;
//AL_SOFT_buffer_sub_data
PFNALBUFFERSUBDATASOFTPROC       alBufferSubDataSOFT            = nullptr;
//AL_EXT_FOLDBACK
LPALREQUESTFOLDBACKSTART         alRequestFoldbackStart         = nullptr;
LPALREQUESTFOLDBACKSTOP          alRequestFoldbackStop          = nullptr;
//AL_SOFT_buffer_samples
LPALBUFFERSAMPLESSOFT            alBufferSamplesSOFT            = nullptr;
LPALBUFFERSUBSAMPLESSOFT         alBufferSubSamplesSOFT         = nullptr;
LPALGETBUFFERSAMPLESSOFT         alGetBufferSamplesSOFT         = nullptr;
LPALISBUFFERFORMATSUPPORTEDSOFT  alIsBufferFormatSupportedSOFT  = nullptr;
//AL_SOFT_source_latency
LPALSOURCEDSOFT                  alSourcedSOFT                  = nullptr;
LPALSOURCE3DSOFT                 alSource3dSOFT                 = nullptr;
LPALSOURCEDVSOFT                 alSourcedvSOFT                 = nullptr;
LPALGETSOURCEDSOFT               alGetSourcedSOFT               = nullptr;
LPALGETSOURCE3DSOFT              alGetSource3dSOFT              = nullptr;
LPALGETSOURCEDVSOFT              alGetSourcedvSOFT              = nullptr;
LPALSOURCEI64SOFT                alSourcei64SOFT                = nullptr;
LPALSOURCE3I64SOFT               alSource3i64SOFT               = nullptr;
LPALSOURCEI64VSOFT               alSourcei64vSOFT               = nullptr;
LPALGETSOURCEI64SOFT             alGetSourcei64SOFT             = nullptr;
LPALGETSOURCE3I64SOFT            alGetSource3i64SOFT            = nullptr;
LPALGETSOURCEI64VSOFT            alGetSourcei64vSOFT            = nullptr;
//AL_SOFT_deferred_updates
LPALDEFERUPDATESSOFT             alDeferUpdatesSOFT             = nullptr;
LPALPROCESSUPDATESSOFT           alProcessUpdatesSOFT           = nullptr;
//AL_SOFT_source_resampler
LPALGETSTRINGISOFT               alGetStringiSOFT               = nullptr;
//AL_SOFT_events
LPALEVENTCONTROLSOFT             alEventControlSOFT             = nullptr;
LPALEVENTCALLBACKSOFT            alEventCallbackSOFT            = nullptr;
LPALGETPOINTERSOFT               alGetPointerSOFT               = nullptr;
LPALGETPOINTERVSOFT              alGetPointervSOFT              = nullptr;
//AL_SOFT_callback_buffer
LPALBUFFERCALLBACKSOFT           alBufferCallbackSOFT           = nullptr;
LPALGETBUFFERPTRSOFT             alGetBufferPtrSOFT             = nullptr;
LPALGETBUFFER3PTRSOFT            alGetBuffer3PtrSOFT            = nullptr;
LPALGETBUFFERPTRVSOFT            alGetBufferPtrvSOFT            = nullptr;

//Core ALC
LPALCCREATECONTEXT               alcCreateContext               = nullptr;
LPALCMAKECONTEXTCURRENT          alcMakeContextCurrent          = nullptr;
LPALCPROCESSCONTEXT              alcProcessContext              = nullptr;
LPALCSUSPENDCONTEXT              alcSuspendContext              = nullptr;
LPALCDESTROYCONTEXT              alcDestroyContext              = nullptr;
LPALCGETCURRENTCONTEXT           alcGetCurrentContext           = nullptr;
LPALCGETCONTEXTSDEVICE           alcGetContextsDevice           = nullptr;
LPALCOPENDEVICE                  alcOpenDevice                  = nullptr;
LPALCCLOSEDEVICE                 alcCloseDevice                 = nullptr;
LPALCGETERROR                    alcGetError                    = nullptr;
LPALCISEXTENSIONPRESENT          alcIsExtensionPresent          = nullptr;
LPALCGETPROCADDRESS              alcGetProcAddress              = nullptr;
LPALCGETENUMVALUE                alcGetEnumValue                = nullptr;
LPALCGETSTRING                   alcGetString                   = nullptr;
LPALCGETINTEGERV                 alcGetIntegerv                 = nullptr;
LPALCCAPTUREOPENDEVICE           alcCaptureOpenDevice           = nullptr;
LPALCCAPTURECLOSEDEVICE          alcCaptureCloseDevice          = nullptr;
LPALCCAPTURESTART                alcCaptureStart                = nullptr;
LPALCCAPTURESTOP                 alcCaptureStop                 = nullptr;
LPALCCAPTURESAMPLES              alcCaptureSamples              = nullptr;

//ALC extensions
//ALC_EXT_thread_local_context
PFNALCSETTHREADCONTEXTPROC       alcSetThreadContext            = nullptr;
PFNALCGETTHREADCONTEXTPROC       alcGetThreadContext            = nullptr;
//ALC_SOFT_loopback
LPALCLOOPBACKOPENDEVICESOFT      alcLoopbackOpenDeviceSOFT      = nullptr;
LPALCISRENDERFORMATSUPPORTEDSOFT alcIsRenderFormatSupportedSOFT = nullptr;
LPALCRENDERSAMPLESSOFT           alcRenderSamplesSOFT           = nullptr;
//ALC_SOFT_pause_device
LPALCDEVICEPAUSESOFT             alcDevicePauseSOFT             = nullptr;
LPALCDEVICERESUMESOFT            alcDeviceResumeSOFT            = nullptr;
//ALC_SOFT_HRTF
LPALCGETSTRINGISOFT              alcGetStringiSOFT              = nullptr;
LPALCRESETDEVICESOFT             alcResetDeviceSOFT             = nullptr;
//ALC_SOFT_device_clock
LPALCGETINTEGER64VSOFT           alcGetInteger64vSOFT           = nullptr;
//ALC_SOFT_reopen_device
LPALCREOPENDEVICESOFT            alcReopenDeviceSOFT            = nullptr;



//implemented down below
typedef void (*alad_proc_) (void);
static void      *alad_open_ (const char *path);
static alad_proc_ alad_load_ (void *module, const char *name);
static void       alad_close_ (void *module);


//ISO C compatibility types for GCC warning: ISO C forbids conversion of object pointer to function pointer type [-Wpedantic]
typedef alad_proc_ (AL_APIENTRY *ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_) (const ALchar *fname);
typedef alad_proc_ (ALC_APIENTRY *ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS_) (ALCdevice *device, const ALCchar *funcname);
typedef alad_proc_ (ALC_APIENTRY *ALAD_ISO_C_COMPAT_dlsym_) (void *module, const char *name);

static void alad_load_al_functions_contextfree_dlsym_ (void *module, ALboolean loadAll) {
        if (module == nullptr) return;
        //Core AL functions without buffer, source, listener and doppler/distance functions, because none of these are necessary to create a context
        alGetProcAddress     = REINTERPRET_CAST(LPALGETPROCADDRESS, alad_load_ (module, "alGetProcAddress"));
        alEnable             = REINTERPRET_CAST(LPALENABLE, alad_load_ (module, "alEnable"));
        alDisable            = REINTERPRET_CAST(LPALDISABLE, alad_load_ (module, "alDisable"));
        alIsEnabled          = REINTERPRET_CAST(LPALISENABLED, alad_load_ (module, "alIsEnabled"));
        alGetString          = REINTERPRET_CAST(LPALGETSTRING, alad_load_ (module, "alGetString"));
        alGetBooleanv        = REINTERPRET_CAST(LPALGETBOOLEANV, alad_load_ (module, "alGetBooleanv"));
        alGetIntegerv        = REINTERPRET_CAST(LPALGETINTEGERV, alad_load_ (module, "alGetIntegerv"));
        alGetFloatv          = REINTERPRET_CAST(LPALGETFLOATV, alad_load_ (module, "alGetFloatv"));
        alGetDoublev         = REINTERPRET_CAST(LPALGETDOUBLEV, alad_load_ (module, "alGetDoublev"));
        alGetBoolean         = REINTERPRET_CAST(LPALGETBOOLEAN, alad_load_ (module, "alGetBoolean"));
        alGetInteger         = REINTERPRET_CAST(LPALGETINTEGER, alad_load_ (module, "alGetInteger"));
        alGetFloat           = REINTERPRET_CAST(LPALGETFLOAT, alad_load_ (module, "alGetFloat"));
        alGetDouble          = REINTERPRET_CAST(LPALGETDOUBLE, alad_load_ (module, "alGetDouble"));
        alGetError           = REINTERPRET_CAST(LPALGETERROR, alad_load_ (module, "alGetError"));
        alIsExtensionPresent = REINTERPRET_CAST(LPALISEXTENSIONPRESENT, alad_load_ (module, "alIsExtensionPresent"));
        alGetEnumValue       = REINTERPRET_CAST(LPALGETENUMVALUE, alad_load_ (module, "alGetEnumValue"));
        //the rest if wanted
        if (loadAll != AL_FALSE) {
                alDopplerFactor        = REINTERPRET_CAST(LPALDOPPLERFACTOR, alad_load_ (module, "alDopplerFactor"));
                alDopplerVelocity      = REINTERPRET_CAST(LPALDOPPLERVELOCITY, alad_load_ (module, "alDopplerVelocity"));
                alSpeedOfSound         = REINTERPRET_CAST(LPALSPEEDOFSOUND, alad_load_ (module, "alSpeedOfSound"));
                alDistanceModel        = REINTERPRET_CAST(LPALDISTANCEMODEL, alad_load_ (module, "alDistanceModel"));
                alListenerf            = REINTERPRET_CAST(LPALLISTENERF, alad_load_ (module, "alListenerf"));
                alListener3f           = REINTERPRET_CAST(LPALLISTENER3F, alad_load_ (module, "alListener3f"));
                alListenerfv           = REINTERPRET_CAST(LPALLISTENERFV, alad_load_ (module, "alListenerfv"));
                alListeneri            = REINTERPRET_CAST(LPALLISTENERI, alad_load_ (module, "alListeneri"));
                alListener3i           = REINTERPRET_CAST(LPALLISTENER3I, alad_load_ (module, "alListener3i"));
                alListeneriv           = REINTERPRET_CAST(LPALLISTENERIV, alad_load_ (module, "alListeneriv"));
                alGetListenerf         = REINTERPRET_CAST(LPALGETLISTENERF, alad_load_ (module, "alGetListenerf"));
                alGetListener3f        = REINTERPRET_CAST(LPALGETLISTENER3F, alad_load_ (module, "alGetListener3f"));
                alGetListenerfv        = REINTERPRET_CAST(LPALGETLISTENERFV, alad_load_ (module, "alGetListenerfv"));
                alGetListeneri         = REINTERPRET_CAST(LPALGETLISTENERI, alad_load_ (module, "alGetListeneri"));
                alGetListener3i        = REINTERPRET_CAST(LPALGETLISTENER3I, alad_load_ (module, "alGetListener3i"));
                alGetListeneriv        = REINTERPRET_CAST(LPALGETLISTENERIV, alad_load_ (module, "alGetListeneriv"));
                alGenSources           = REINTERPRET_CAST(LPALGENSOURCES, alad_load_ (module, "alGenSources"));
                alDeleteSources        = REINTERPRET_CAST(LPALDELETESOURCES, alad_load_ (module, "alDeleteSources"));
                alIsSource             = REINTERPRET_CAST(LPALISSOURCE, alad_load_ (module, "alIsSource"));
                alSourcef              = REINTERPRET_CAST(LPALSOURCEF, alad_load_ (module, "alSourcef"));
                alSource3f             = REINTERPRET_CAST(LPALSOURCE3F, alad_load_ (module, "alSource3f"));
                alSourcefv             = REINTERPRET_CAST(LPALSOURCEFV, alad_load_ (module, "alSourcefv"));
                alSourcei              = REINTERPRET_CAST(LPALSOURCEI, alad_load_ (module, "alSourcei"));
                alSource3i             = REINTERPRET_CAST(LPALSOURCE3I, alad_load_ (module, "alSource3i"));
                alSourceiv             = REINTERPRET_CAST(LPALSOURCEIV, alad_load_ (module, "alSourceiv"));
                alGetSourcef           = REINTERPRET_CAST(LPALGETSOURCEF, alad_load_ (module, "alGetSourcef"));
                alGetSource3f          = REINTERPRET_CAST(LPALGETSOURCE3F, alad_load_ (module, "alGetSource3f"));
                alGetSourcefv          = REINTERPRET_CAST(LPALGETSOURCEFV, alad_load_ (module, "alGetSourcefv"));
                alGetSourcei           = REINTERPRET_CAST(LPALGETSOURCEI, alad_load_ (module, "alGetSourcei"));
                alGetSource3i          = REINTERPRET_CAST(LPALGETSOURCE3I, alad_load_ (module, "alGetSource3i"));
                alGetSourceiv          = REINTERPRET_CAST(LPALGETSOURCEIV, alad_load_ (module, "alGetSourceiv"));
                alSourcePlayv          = REINTERPRET_CAST(LPALSOURCEPLAYV, alad_load_ (module, "alSourcePlayv"));
                alSourceStopv          = REINTERPRET_CAST(LPALSOURCESTOPV, alad_load_ (module, "alSourceStopv"));
                alSourceRewindv        = REINTERPRET_CAST(LPALSOURCEREWINDV, alad_load_ (module, "alSourceRewindv"));
                alSourcePausev         = REINTERPRET_CAST(LPALSOURCEPAUSEV, alad_load_ (module, "alSourcePausev"));
                alSourcePlay           = REINTERPRET_CAST(LPALSOURCEPLAY, alad_load_ (module, "alSourcePlay"));
                alSourceStop           = REINTERPRET_CAST(LPALSOURCESTOP, alad_load_ (module, "alSourceStop"));
                alSourceRewind         = REINTERPRET_CAST(LPALSOURCEREWIND, alad_load_ (module, "alSourceRewind"));
                alSourcePause          = REINTERPRET_CAST(LPALSOURCEPAUSE, alad_load_ (module, "alSourcePause"));
                alSourceQueueBuffers   = REINTERPRET_CAST(LPALSOURCEQUEUEBUFFERS, alad_load_ (module, "alSourceQueueBuffers"));
                alSourceUnqueueBuffers = REINTERPRET_CAST(LPALSOURCEUNQUEUEBUFFERS, alad_load_ (module, "alSourceUnqueueBuffers"));
                alGenBuffers           = REINTERPRET_CAST(LPALGENBUFFERS, alad_load_ (module, "alGenBuffers"));
                alDeleteBuffers        = REINTERPRET_CAST(LPALDELETEBUFFERS, alad_load_ (module, "alDeleteBuffers"));
                alIsBuffer             = REINTERPRET_CAST(LPALISBUFFER, alad_load_ (module, "alIsBuffer"));
                alBufferData           = REINTERPRET_CAST(LPALBUFFERDATA, alad_load_ (module, "alBufferData"));
                alBufferf              = REINTERPRET_CAST(LPALBUFFERF, alad_load_ (module, "alBufferf"));
                alBuffer3f             = REINTERPRET_CAST(LPALBUFFER3F, alad_load_ (module, "alBuffer3f"));
                alBufferfv             = REINTERPRET_CAST(LPALBUFFERFV, alad_load_ (module, "alBufferfv"));
                alBufferi              = REINTERPRET_CAST(LPALBUFFERI, alad_load_ (module, "alBufferi"));
                alBuffer3i             = REINTERPRET_CAST(LPALBUFFER3I, alad_load_ (module, "alBuffer3i"));
                alBufferiv             = REINTERPRET_CAST(LPALBUFFERIV, alad_load_ (module, "alBufferiv"));
                alGetBufferf           = REINTERPRET_CAST(LPALGETBUFFERF, alad_load_ (module, "alGetBufferf"));
                alGetBuffer3f          = REINTERPRET_CAST(LPALGETBUFFER3F, alad_load_ (module, "alGetBuffer3f"));
                alGetBufferfv          = REINTERPRET_CAST(LPALGETBUFFERFV, alad_load_ (module, "alGetBufferfv"));
                alGetBufferi           = REINTERPRET_CAST(LPALGETBUFFERI, alad_load_ (module, "alGetBufferi"));
                alGetBuffer3i          = REINTERPRET_CAST(LPALGETBUFFER3I, alad_load_ (module, "alGetBuffer3i"));
                alGetBufferiv          = REINTERPRET_CAST(LPALGETBUFFERIV, alad_load_ (module, "alGetBufferiv"));
        }
}

static void alad_load_al_functions_ () {
        if (alGetProcAddress == nullptr) return;
        ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_ compat_alGetProcAddress = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_, alGetProcAddress);
        //Core AL
        alDopplerFactor                                               = REINTERPRET_CAST(LPALDOPPLERFACTOR, compat_alGetProcAddress ("alDopplerFactor"));
        alDopplerVelocity                                             = REINTERPRET_CAST(LPALDOPPLERVELOCITY, compat_alGetProcAddress ("alDopplerVelocity"));
        alSpeedOfSound                                                = REINTERPRET_CAST(LPALSPEEDOFSOUND, compat_alGetProcAddress ("alSpeedOfSound"));
        alDistanceModel                                               = REINTERPRET_CAST(LPALDISTANCEMODEL, compat_alGetProcAddress ("alDistanceModel"));
        alEnable                                                      = REINTERPRET_CAST(LPALENABLE, compat_alGetProcAddress ("alEnable"));
        alDisable                                                     = REINTERPRET_CAST(LPALDISABLE, compat_alGetProcAddress ("alDisable"));
        alIsEnabled                                                   = REINTERPRET_CAST(LPALISENABLED, compat_alGetProcAddress ("alIsEnabled"));
        alGetString                                                   = REINTERPRET_CAST(LPALGETSTRING, compat_alGetProcAddress ("alGetString"));
        alGetBooleanv                                                 = REINTERPRET_CAST(LPALGETBOOLEANV, compat_alGetProcAddress ("alGetBooleanv"));
        alGetIntegerv                                                 = REINTERPRET_CAST(LPALGETINTEGERV, compat_alGetProcAddress ("alGetIntegerv"));
        alGetFloatv                                                   = REINTERPRET_CAST(LPALGETFLOATV, compat_alGetProcAddress ("alGetFloatv"));
        alGetDoublev                                                  = REINTERPRET_CAST(LPALGETDOUBLEV, compat_alGetProcAddress ("alGetDoublev"));
        alGetBoolean                                                  = REINTERPRET_CAST(LPALGETBOOLEAN, compat_alGetProcAddress ("alGetBoolean"));
        alGetInteger                                                  = REINTERPRET_CAST(LPALGETINTEGER, compat_alGetProcAddress ("alGetInteger"));
        alGetFloat                                                    = REINTERPRET_CAST(LPALGETFLOAT, compat_alGetProcAddress ("alGetFloat"));
        alGetDouble                                                   = REINTERPRET_CAST(LPALGETDOUBLE, compat_alGetProcAddress ("alGetDouble"));
        alGetError                                                    = REINTERPRET_CAST(LPALGETERROR, compat_alGetProcAddress ("alGetError"));
        alIsExtensionPresent                                          = REINTERPRET_CAST(LPALISEXTENSIONPRESENT, compat_alGetProcAddress ("alIsExtensionPresent"));
        alGetEnumValue                                                = REINTERPRET_CAST(LPALGETENUMVALUE, compat_alGetProcAddress ("alGetEnumValue"));
        alListenerf                                                   = REINTERPRET_CAST(LPALLISTENERF, compat_alGetProcAddress ("alListenerf"));
        alListener3f                                                  = REINTERPRET_CAST(LPALLISTENER3F, compat_alGetProcAddress ("alListener3f"));
        alListenerfv                                                  = REINTERPRET_CAST(LPALLISTENERFV, compat_alGetProcAddress ("alListenerfv"));
        alListeneri                                                   = REINTERPRET_CAST(LPALLISTENERI, compat_alGetProcAddress ("alListeneri"));
        alListener3i                                                  = REINTERPRET_CAST(LPALLISTENER3I, compat_alGetProcAddress ("alListener3i"));
        alListeneriv                                                  = REINTERPRET_CAST(LPALLISTENERIV, compat_alGetProcAddress ("alListeneriv"));
        alGetListenerf                                                = REINTERPRET_CAST(LPALGETLISTENERF, compat_alGetProcAddress ("alGetListenerf"));
        alGetListener3f                                               = REINTERPRET_CAST(LPALGETLISTENER3F, compat_alGetProcAddress ("alGetListener3f"));
        alGetListenerfv                                               = REINTERPRET_CAST(LPALGETLISTENERFV, compat_alGetProcAddress ("alGetListenerfv"));
        alGetListeneri                                                = REINTERPRET_CAST(LPALGETLISTENERI, compat_alGetProcAddress ("alGetListeneri"));
        alGetListener3i                                               = REINTERPRET_CAST(LPALGETLISTENER3I, compat_alGetProcAddress ("alGetListener3i"));
        alGetListeneriv                                               = REINTERPRET_CAST(LPALGETLISTENERIV, compat_alGetProcAddress ("alGetListeneriv"));
        alGenSources                                                  = REINTERPRET_CAST(LPALGENSOURCES, compat_alGetProcAddress ("alGenSources"));
        alDeleteSources                                               = REINTERPRET_CAST(LPALDELETESOURCES, compat_alGetProcAddress ("alDeleteSources"));
        alIsSource                                                    = REINTERPRET_CAST(LPALISSOURCE, compat_alGetProcAddress ("alIsSource"));
        alSourcef                                                     = REINTERPRET_CAST(LPALSOURCEF, compat_alGetProcAddress ("alSourcef"));
        alSource3f                                                    = REINTERPRET_CAST(LPALSOURCE3F, compat_alGetProcAddress ("alSource3f"));
        alSourcefv                                                    = REINTERPRET_CAST(LPALSOURCEFV, compat_alGetProcAddress ("alSourcefv"));
        alSourcei                                                     = REINTERPRET_CAST(LPALSOURCEI, compat_alGetProcAddress ("alSourcei"));
        alSource3i                                                    = REINTERPRET_CAST(LPALSOURCE3I, compat_alGetProcAddress ("alSource3i"));
        alSourceiv                                                    = REINTERPRET_CAST(LPALSOURCEIV, compat_alGetProcAddress ("alSourceiv"));
        alGetSourcef                                                  = REINTERPRET_CAST(LPALGETSOURCEF, compat_alGetProcAddress ("alGetSourcef"));
        alGetSource3f                                                 = REINTERPRET_CAST(LPALGETSOURCE3F, compat_alGetProcAddress ("alGetSource3f"));
        alGetSourcefv                                                 = REINTERPRET_CAST(LPALGETSOURCEFV, compat_alGetProcAddress ("alGetSourcefv"));
        alGetSourcei                                                  = REINTERPRET_CAST(LPALGETSOURCEI, compat_alGetProcAddress ("alGetSourcei"));
        alGetSource3i                                                 = REINTERPRET_CAST(LPALGETSOURCE3I, compat_alGetProcAddress ("alGetSource3i"));
        alGetSourceiv                                                 = REINTERPRET_CAST(LPALGETSOURCEIV, compat_alGetProcAddress ("alGetSourceiv"));
        alSourcePlayv                                                 = REINTERPRET_CAST(LPALSOURCEPLAYV, compat_alGetProcAddress ("alSourcePlayv"));
        alSourceStopv                                                 = REINTERPRET_CAST(LPALSOURCESTOPV, compat_alGetProcAddress ("alSourceStopv"));
        alSourceRewindv                                               = REINTERPRET_CAST(LPALSOURCEREWINDV, compat_alGetProcAddress ("alSourceRewindv"));
        alSourcePausev                                                = REINTERPRET_CAST(LPALSOURCEPAUSEV, compat_alGetProcAddress ("alSourcePausev"));
        alSourcePlay                                                  = REINTERPRET_CAST(LPALSOURCEPLAY, compat_alGetProcAddress ("alSourcePlay"));
        alSourceStop                                                  = REINTERPRET_CAST(LPALSOURCESTOP, compat_alGetProcAddress ("alSourceStop"));
        alSourceRewind                                                = REINTERPRET_CAST(LPALSOURCEREWIND, compat_alGetProcAddress ("alSourceRewind"));
        alSourcePause                                                 = REINTERPRET_CAST(LPALSOURCEPAUSE, compat_alGetProcAddress ("alSourcePause"));
        alSourceQueueBuffers                                          = REINTERPRET_CAST(LPALSOURCEQUEUEBUFFERS, compat_alGetProcAddress ("alSourceQueueBuffers"));
        alSourceUnqueueBuffers                                        = REINTERPRET_CAST(LPALSOURCEUNQUEUEBUFFERS, compat_alGetProcAddress ("alSourceUnqueueBuffers"));
        alGenBuffers                                                  = REINTERPRET_CAST(LPALGENBUFFERS, compat_alGetProcAddress ("alGenBuffers"));
        alDeleteBuffers                                               = REINTERPRET_CAST(LPALDELETEBUFFERS, compat_alGetProcAddress ("alDeleteBuffers"));
        alIsBuffer                                                    = REINTERPRET_CAST(LPALISBUFFER, compat_alGetProcAddress ("alIsBuffer"));
        alBufferData                                                  = REINTERPRET_CAST(LPALBUFFERDATA, compat_alGetProcAddress ("alBufferData"));
        alBufferf                                                     = REINTERPRET_CAST(LPALBUFFERF, compat_alGetProcAddress ("alBufferf"));
        alBuffer3f                                                    = REINTERPRET_CAST(LPALBUFFER3F, compat_alGetProcAddress ("alBuffer3f"));
        alBufferfv                                                    = REINTERPRET_CAST(LPALBUFFERFV, compat_alGetProcAddress ("alBufferfv"));
        alBufferi                                                     = REINTERPRET_CAST(LPALBUFFERI, compat_alGetProcAddress ("alBufferi"));
        alBuffer3i                                                    = REINTERPRET_CAST(LPALBUFFER3I, compat_alGetProcAddress ("alBuffer3i"));
        alBufferiv                                                    = REINTERPRET_CAST(LPALBUFFERIV, compat_alGetProcAddress ("alBufferiv"));
        alGetBufferf                                                  = REINTERPRET_CAST(LPALGETBUFFERF, compat_alGetProcAddress ("alGetBufferf"));
        alGetBuffer3f                                                 = REINTERPRET_CAST(LPALGETBUFFER3F, compat_alGetProcAddress ("alGetBuffer3f"));
        alGetBufferfv                                                 = REINTERPRET_CAST(LPALGETBUFFERFV, compat_alGetProcAddress ("alGetBufferfv"));
        alGetBufferi                                                  = REINTERPRET_CAST(LPALGETBUFFERI, compat_alGetProcAddress ("alGetBufferi"));
        alGetBuffer3i                                                 = REINTERPRET_CAST(LPALGETBUFFER3I, compat_alGetProcAddress ("alGetBuffer3i"));
        alGetBufferiv                                                 = REINTERPRET_CAST(LPALGETBUFFERIV, compat_alGetProcAddress ("alGetBufferiv"));
}

static void alad_load_al_extension_functions_ () {
        if (alGetProcAddress == nullptr) return;
        ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_ compat_alGetProcAddress = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_, alGetProcAddress);
        //EFX
        alGenEffects                                                  = REINTERPRET_CAST(LPALGENEFFECTS, compat_alGetProcAddress ("alGenEffects"));
        alDeleteEffects                                               = REINTERPRET_CAST(LPALDELETEEFFECTS, compat_alGetProcAddress ("alDeleteEffects"));
        alIsEffect                                                    = REINTERPRET_CAST(LPALISEFFECT, compat_alGetProcAddress ("alIsEffect"));
        alEffecti                                                     = REINTERPRET_CAST(LPALEFFECTI, compat_alGetProcAddress ("alEffecti"));
        alEffectiv                                                    = REINTERPRET_CAST(LPALEFFECTIV, compat_alGetProcAddress ("alEffectiv"));
        alEffectf                                                     = REINTERPRET_CAST(LPALEFFECTF, compat_alGetProcAddress ("alEffectf"));
        alEffectfv                                                    = REINTERPRET_CAST(LPALEFFECTFV, compat_alGetProcAddress ("alEffectfv"));
        alGetEffecti                                                  = REINTERPRET_CAST(LPALGETEFFECTI, compat_alGetProcAddress ("alGetEffecti"));
        alGetEffectiv                                                 = REINTERPRET_CAST(LPALGETEFFECTIV, compat_alGetProcAddress ("alGetEffectiv"));
        alGetEffectf                                                  = REINTERPRET_CAST(LPALGETEFFECTF, compat_alGetProcAddress ("alGetEffectf"));
        alGetEffectfv                                                 = REINTERPRET_CAST(LPALGETEFFECTFV, compat_alGetProcAddress ("alGetEffectfv"));
        alGenFilters                                                  = REINTERPRET_CAST(LPALGENFILTERS, compat_alGetProcAddress ("alGenFilters"));
        alDeleteFilters                                               = REINTERPRET_CAST(LPALDELETEFILTERS, compat_alGetProcAddress ("alDeleteFilters"));
        alIsFilter                                                    = REINTERPRET_CAST(LPALISFILTER, compat_alGetProcAddress ("alIsFilter"));
        alFilteri                                                     = REINTERPRET_CAST(LPALFILTERI, compat_alGetProcAddress ("alFilteri"));
        alFilteriv                                                    = REINTERPRET_CAST(LPALFILTERIV, compat_alGetProcAddress ("alFilteriv"));
        alFilterf                                                     = REINTERPRET_CAST(LPALFILTERF, compat_alGetProcAddress ("alFilterf"));
        alFilterfv                                                    = REINTERPRET_CAST(LPALFILTERFV, compat_alGetProcAddress ("alFilterfv"));
        alGetFilteri                                                  = REINTERPRET_CAST(LPALGETFILTERI, compat_alGetProcAddress ("alGetFilteri"));
        alGetFilteriv                                                 = REINTERPRET_CAST(LPALGETFILTERIV, compat_alGetProcAddress ("alGetFilteriv"));
        alGetFilterf                                                  = REINTERPRET_CAST(LPALGETFILTERF, compat_alGetProcAddress ("alGetFilterf"));
        alGetFilterfv                                                 = REINTERPRET_CAST(LPALGETFILTERFV, compat_alGetProcAddress ("alGetFilterfv"));
        alGenAuxiliaryEffectSlots                                     = REINTERPRET_CAST(LPALGENAUXILIARYEFFECTSLOTS, compat_alGetProcAddress ("alGenAuxiliaryEffectSlots"));
        alDeleteAuxiliaryEffectSlots  = REINTERPRET_CAST(LPALDELETEAUXILIARYEFFECTSLOTS, compat_alGetProcAddress ("alDeleteAuxiliaryEffectSlots"));
        alIsAuxiliaryEffectSlot       = REINTERPRET_CAST(LPALISAUXILIARYEFFECTSLOT, compat_alGetProcAddress ("alIsAuxiliaryEffectSlot"));
        alAuxiliaryEffectSloti        = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTI, compat_alGetProcAddress ("alAuxiliaryEffectSloti"));
        alAuxiliaryEffectSlotiv       = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTIV, compat_alGetProcAddress ("alAuxiliaryEffectSlotiv"));
        alAuxiliaryEffectSlotf        = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTF, compat_alGetProcAddress ("alAuxiliaryEffectSlotf"));
        alAuxiliaryEffectSlotfv       = REINTERPRET_CAST(LPALAUXILIARYEFFECTSLOTFV, compat_alGetProcAddress ("alAuxiliaryEffectSlotfv"));
        alGetAuxiliaryEffectSloti     = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTI, compat_alGetProcAddress ("alGetAuxiliaryEffectSloti"));
        alGetAuxiliaryEffectSlotiv    = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTIV, compat_alGetProcAddress ("alGetAuxiliaryEffectSlotiv"));
        alGetAuxiliaryEffectSlotf     = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTF, compat_alGetProcAddress ("alGetAuxiliaryEffectSlotf"));
        alGetAuxiliaryEffectSlotfv    = REINTERPRET_CAST(LPALGETAUXILIARYEFFECTSLOTFV, compat_alGetProcAddress ("alGetAuxiliaryEffectSlotfv"));

        //AL extensions
        //AL_EXT_STATIC_BUFFER
        alBufferDataStatic            = REINTERPRET_CAST(PFNALBUFFERDATASTATICPROC, compat_alGetProcAddress ("alBufferDataStatic"));
        //AL_SOFT_buffer_sub_data
        alBufferSubDataSOFT           = REINTERPRET_CAST(PFNALBUFFERSUBDATASOFTPROC, compat_alGetProcAddress ("alBufferSubDataSOFT"));
        //AL_EXT_FOLDBACK
        alRequestFoldbackStart        = REINTERPRET_CAST(LPALREQUESTFOLDBACKSTART, compat_alGetProcAddress ("alRequestFoldbackStart"));
        alRequestFoldbackStop         = REINTERPRET_CAST(LPALREQUESTFOLDBACKSTOP, compat_alGetProcAddress ("alRequestFoldbackStop"));
        //AL_SOFT_buffer_samples
        alBufferSamplesSOFT           = REINTERPRET_CAST(LPALBUFFERSAMPLESSOFT, compat_alGetProcAddress ("alBufferSamplesSOFT"));
        alBufferSubSamplesSOFT        = REINTERPRET_CAST(LPALBUFFERSUBSAMPLESSOFT, compat_alGetProcAddress ("alBufferSubSamplesSOFT"));
        alGetBufferSamplesSOFT        = REINTERPRET_CAST(LPALGETBUFFERSAMPLESSOFT, compat_alGetProcAddress ("alGetBufferSamplesSOFT"));
        alIsBufferFormatSupportedSOFT = REINTERPRET_CAST(LPALISBUFFERFORMATSUPPORTEDSOFT, compat_alGetProcAddress ("alIsBufferFormatSupportedSOFT"));
        //AL_SOFT_source_latency
        alSourcedSOFT                 = REINTERPRET_CAST(LPALSOURCEDSOFT, compat_alGetProcAddress ("alSourcedSOFT"));
        alSource3dSOFT                = REINTERPRET_CAST(LPALSOURCE3DSOFT, compat_alGetProcAddress ("alSource3dSOFT"));
        alSourcedvSOFT                = REINTERPRET_CAST(LPALSOURCEDVSOFT, compat_alGetProcAddress ("alSourcedvSOFT"));
        alGetSourcedSOFT              = REINTERPRET_CAST(LPALGETSOURCEDSOFT, compat_alGetProcAddress ("alGetSourcedSOFT"));
        alGetSource3dSOFT             = REINTERPRET_CAST(LPALGETSOURCE3DSOFT, compat_alGetProcAddress ("alGetSource3dSOFT"));
        alGetSourcedvSOFT             = REINTERPRET_CAST(LPALGETSOURCEDVSOFT, compat_alGetProcAddress ("alGetSourcedvSOFT"));
        alSourcei64SOFT               = REINTERPRET_CAST(LPALSOURCEI64SOFT, compat_alGetProcAddress ("alSourcei64SOFT"));
        alSource3i64SOFT              = REINTERPRET_CAST(LPALSOURCE3I64SOFT, compat_alGetProcAddress ("alSource3i64SOFT"));
        alSourcei64vSOFT              = REINTERPRET_CAST(LPALSOURCEI64VSOFT, compat_alGetProcAddress ("alSourcei64vSOFT"));
        alGetSourcei64SOFT            = REINTERPRET_CAST(LPALGETSOURCEI64SOFT, compat_alGetProcAddress ("alGetSourcei64SOFT"));
        alGetSource3i64SOFT           = REINTERPRET_CAST(LPALGETSOURCE3I64SOFT, compat_alGetProcAddress ("alGetSource3i64SOFT"));
        alGetSourcei64vSOFT           = REINTERPRET_CAST(LPALGETSOURCEI64VSOFT, compat_alGetProcAddress ("alGetSourcei64vSOFT"));
        //AL_SOFT_deferred_updates
        alDeferUpdatesSOFT            = REINTERPRET_CAST(LPALDEFERUPDATESSOFT, compat_alGetProcAddress ("alDeferUpdatesSOFT"));
        alProcessUpdatesSOFT          = REINTERPRET_CAST(LPALPROCESSUPDATESSOFT, compat_alGetProcAddress ("alProcessUpdatesSOFT"));
        //AL_SOFT_source_resampler
        alGetStringiSOFT              = REINTERPRET_CAST(LPALGETSTRINGISOFT, compat_alGetProcAddress ("alGetStringiSOFT"));
        //AL_SOFT_events
        alEventControlSOFT            = REINTERPRET_CAST(LPALEVENTCONTROLSOFT, compat_alGetProcAddress ("alEventControlSOFT"));
        alEventCallbackSOFT           = REINTERPRET_CAST(LPALEVENTCALLBACKSOFT, compat_alGetProcAddress ("alEventCallbackSOFT"));
        alGetPointerSOFT              = REINTERPRET_CAST(LPALGETPOINTERSOFT, compat_alGetProcAddress ("alGetPointerSOFT"));
        alGetPointervSOFT             = REINTERPRET_CAST(LPALGETPOINTERVSOFT, compat_alGetProcAddress ("alGetPointervSOFT"));
        //AL_SOFT_callback_buffer
        alBufferCallbackSOFT          = REINTERPRET_CAST(LPALBUFFERCALLBACKSOFT, compat_alGetProcAddress ("alBufferCallbackSOFT"));
        alGetBufferPtrSOFT            = REINTERPRET_CAST(LPALGETBUFFERPTRSOFT, compat_alGetProcAddress ("alGetBufferPtrSOFT"));
        alGetBuffer3PtrSOFT           = REINTERPRET_CAST(LPALGETBUFFER3PTRSOFT, compat_alGetProcAddress ("alGetBuffer3PtrSOFT"));
        alGetBufferPtrvSOFT           = REINTERPRET_CAST(LPALGETBUFFERPTRVSOFT, compat_alGetProcAddress ("alGetBufferPtrvSOFT"));
}

static void alad_load_alc_functions_contextfree_dlsym_ (void *module) {
        if (module == nullptr) return;
        //Core ALC
        alcGetProcAddress     = REINTERPRET_CAST(LPALCGETPROCADDRESS, alad_load_ (module, "alcGetProcAddress"));
        alcCreateContext      = REINTERPRET_CAST(LPALCCREATECONTEXT, alad_load_ (module, "alcCreateContext"));
        alcMakeContextCurrent = REINTERPRET_CAST(LPALCMAKECONTEXTCURRENT, alad_load_ (module, "alcMakeContextCurrent"));
        alcProcessContext     = REINTERPRET_CAST(LPALCPROCESSCONTEXT, alad_load_ (module, "alcProcessContext"));
        alcSuspendContext     = REINTERPRET_CAST(LPALCSUSPENDCONTEXT, alad_load_ (module, "alcSuspendContext"));
        alcDestroyContext     = REINTERPRET_CAST(LPALCDESTROYCONTEXT, alad_load_ (module, "alcDestroyContext"));
        alcGetCurrentContext  = REINTERPRET_CAST(LPALCGETCURRENTCONTEXT, alad_load_ (module, "alcGetCurrentContext"));
        alcGetContextsDevice  = REINTERPRET_CAST(LPALCGETCONTEXTSDEVICE, alad_load_ (module, "alcGetContextsDevice"));
        alcOpenDevice         = REINTERPRET_CAST(LPALCOPENDEVICE, alad_load_ (module, "alcOpenDevice"));
        alcCloseDevice        = REINTERPRET_CAST(LPALCCLOSEDEVICE, alad_load_ (module, "alcCloseDevice"));
        alcGetError           = REINTERPRET_CAST(LPALCGETERROR, alad_load_ (module, "alcGetError"));
        alcIsExtensionPresent = REINTERPRET_CAST(LPALCISEXTENSIONPRESENT, alad_load_ (module, "alcIsExtensionPresent"));
        alcGetEnumValue       = REINTERPRET_CAST(LPALCGETENUMVALUE, alad_load_ (module, "alcGetEnumValue"));
        alcGetString          = REINTERPRET_CAST(LPALCGETSTRING, alad_load_ (module, "alcGetString"));
        alcGetIntegerv        = REINTERPRET_CAST(LPALCGETINTEGERV, alad_load_ (module, "alcGetIntegerv"));
        alcCaptureOpenDevice  = REINTERPRET_CAST(LPALCCAPTUREOPENDEVICE, alad_load_ (module, "alcCaptureOpenDevice"));
        alcCaptureCloseDevice = REINTERPRET_CAST(LPALCCAPTURECLOSEDEVICE, alad_load_ (module, "alcCaptureCloseDevice"));
        alcCaptureStart       = REINTERPRET_CAST(LPALCCAPTURESTART, alad_load_ (module, "alcCaptureStart"));
        alcCaptureStop        = REINTERPRET_CAST(LPALCCAPTURESTOP, alad_load_ (module, "alcCaptureStop"));
        alcCaptureSamples     = REINTERPRET_CAST(LPALCCAPTURESAMPLES, alad_load_ (module, "alcCaptureSamples"));
}


static void alad_load_alc_functions_from_al_ () {
        if (alGetProcAddress == nullptr) return;
        ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_ compat_alGetProcAddress = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_, alGetProcAddress);
        //Only load function loader once, and only if not already loaded from shared library
        if (alcGetProcAddress == nullptr) {
                alcGetProcAddress = REINTERPRET_CAST(LPALCGETPROCADDRESS, compat_alGetProcAddress ("alcGetProcAddress"));
        }
        //Core ALC
        alcCreateContext      = REINTERPRET_CAST(LPALCCREATECONTEXT, compat_alGetProcAddress ("alcCreateContext"));
        alcMakeContextCurrent = REINTERPRET_CAST(LPALCMAKECONTEXTCURRENT, compat_alGetProcAddress ("alcMakeContextCurrent"));
        alcProcessContext     = REINTERPRET_CAST(LPALCPROCESSCONTEXT, compat_alGetProcAddress ("alcProcessContext"));
        alcSuspendContext     = REINTERPRET_CAST(LPALCSUSPENDCONTEXT, compat_alGetProcAddress ("alcSuspendContext"));
        alcDestroyContext     = REINTERPRET_CAST(LPALCDESTROYCONTEXT, compat_alGetProcAddress ("alcDestroyContext"));
        alcGetCurrentContext  = REINTERPRET_CAST(LPALCGETCURRENTCONTEXT, compat_alGetProcAddress ("alcGetCurrentContext"));
        alcGetContextsDevice  = REINTERPRET_CAST(LPALCGETCONTEXTSDEVICE, compat_alGetProcAddress ("alcGetContextsDevice"));
        alcOpenDevice         = REINTERPRET_CAST(LPALCOPENDEVICE, compat_alGetProcAddress ("alcOpenDevice"));
        alcCloseDevice        = REINTERPRET_CAST(LPALCCLOSEDEVICE, compat_alGetProcAddress ("alcCloseDevice"));
        alcGetError           = REINTERPRET_CAST(LPALCGETERROR, compat_alGetProcAddress ("alcGetError"));
        alcIsExtensionPresent = REINTERPRET_CAST(LPALCISEXTENSIONPRESENT, compat_alGetProcAddress ("alcIsExtensionPresent"));
        alcGetEnumValue       = REINTERPRET_CAST(LPALCGETENUMVALUE, compat_alGetProcAddress ("alcGetEnumValue"));
        alcGetString          = REINTERPRET_CAST(LPALCGETSTRING, compat_alGetProcAddress ("alcGetString"));
        alcGetIntegerv        = REINTERPRET_CAST(LPALCGETINTEGERV, compat_alGetProcAddress ("alcGetIntegerv"));
        alcCaptureOpenDevice  = REINTERPRET_CAST(LPALCCAPTUREOPENDEVICE, compat_alGetProcAddress ("alcCaptureOpenDevice"));
        alcCaptureCloseDevice = REINTERPRET_CAST(LPALCCAPTURECLOSEDEVICE, compat_alGetProcAddress ("alcCaptureCloseDevice"));
        alcCaptureStart       = REINTERPRET_CAST(LPALCCAPTURESTART, compat_alGetProcAddress ("alcCaptureStart"));
        alcCaptureStop        = REINTERPRET_CAST(LPALCCAPTURESTOP, compat_alGetProcAddress ("alcCaptureStop"));
        alcCaptureSamples     = REINTERPRET_CAST(LPALCCAPTURESAMPLES, compat_alGetProcAddress ("alcCaptureSamples"));
}


static void alad_load_alc_extension_functions_from_al_ () {
        if (alGetProcAddress == nullptr) return;
        ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_ compat_alGetProcAddress = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS_, alGetProcAddress);
        //ALC extensions
        //ALC_EXT_thread_local_context
        alcSetThreadContext                                           = REINTERPRET_CAST(PFNALCSETTHREADCONTEXTPROC, compat_alGetProcAddress ("alcSetThreadContext"));
        alcGetThreadContext                                           = REINTERPRET_CAST(PFNALCGETTHREADCONTEXTPROC, compat_alGetProcAddress ("alcGetThreadContext"));
        //ALC_SOFT_loopback
        alcLoopbackOpenDeviceSOFT                                     = REINTERPRET_CAST(LPALCLOOPBACKOPENDEVICESOFT, compat_alGetProcAddress ("alcLoopbackOpenDeviceSOFT"));
        alcIsRenderFormatSupportedSOFT = REINTERPRET_CAST(LPALCISRENDERFORMATSUPPORTEDSOFT, compat_alGetProcAddress ("alcIsRenderFormatSupportedSOFT"));
        alcRenderSamplesSOFT           = REINTERPRET_CAST(LPALCRENDERSAMPLESSOFT, compat_alGetProcAddress ("alcRenderSamplesSOFT"));
        //ALC_SOFT_pause_device
        alcDevicePauseSOFT             = REINTERPRET_CAST(LPALCDEVICEPAUSESOFT, compat_alGetProcAddress ("alcDevicePauseSOFT"));
        alcDeviceResumeSOFT            = REINTERPRET_CAST(LPALCDEVICERESUMESOFT, compat_alGetProcAddress ("alcDeviceResumeSOFT"));
        //ALC_SOFT_HRTF
        alcGetStringiSOFT              = REINTERPRET_CAST(LPALCGETSTRINGISOFT, compat_alGetProcAddress ("alcGetStringiSOFT"));
        alcResetDeviceSOFT             = REINTERPRET_CAST(LPALCRESETDEVICESOFT, compat_alGetProcAddress ("alcResetDeviceSOFT"));
        //ALC_SOFT_device_clock
        alcGetInteger64vSOFT           = REINTERPRET_CAST(LPALCGETINTEGER64VSOFT, compat_alGetProcAddress ("alcGetInteger64vSOFT"));
        //ALC_SOFT_reopen_device
        alcReopenDeviceSOFT            = REINTERPRET_CAST(LPALCREOPENDEVICESOFT, compat_alGetProcAddress ("alcReopenDeviceSOFT"));
}

static void alad_load_alc_functions_ (ALCdevice *device) {
        //old loader code left for future use
        /*
    if((alGetProcAddress == nullptr) && (alcGetProcAddress == nullptr)) return;
    if(alcGetProcAddress == nullptr) {
        alcGetProcAddress = REINTERPRET_CAST(LPALCGETPROCADDRESS              , alGetProcAddress("alcGetProcAddress"));
    }
    */
        if (alcGetProcAddress == nullptr) return;
        ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS_ compat_alcGetProcAddress = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS_, alcGetProcAddress);
        //Core ALC
        alcCreateContext                                                = REINTERPRET_CAST(LPALCCREATECONTEXT, compat_alcGetProcAddress (device, "alcCreateContext"));
        alcMakeContextCurrent                                           = REINTERPRET_CAST(LPALCMAKECONTEXTCURRENT, compat_alcGetProcAddress (device, "alcMakeContextCurrent"));
        alcProcessContext                                               = REINTERPRET_CAST(LPALCPROCESSCONTEXT, compat_alcGetProcAddress (device, "alcProcessContext"));
        alcSuspendContext                                               = REINTERPRET_CAST(LPALCSUSPENDCONTEXT, compat_alcGetProcAddress (device, "alcSuspendContext"));
        alcDestroyContext                                               = REINTERPRET_CAST(LPALCDESTROYCONTEXT, compat_alcGetProcAddress (device, "alcDestroyContext"));
        alcGetCurrentContext                                            = REINTERPRET_CAST(LPALCGETCURRENTCONTEXT, compat_alcGetProcAddress (device, "alcGetCurrentContext"));
        alcGetContextsDevice                                            = REINTERPRET_CAST(LPALCGETCONTEXTSDEVICE, compat_alcGetProcAddress (device, "alcGetContextsDevice"));
        alcOpenDevice                                                   = REINTERPRET_CAST(LPALCOPENDEVICE, compat_alcGetProcAddress (device, "alcOpenDevice"));
        alcCloseDevice                                                  = REINTERPRET_CAST(LPALCCLOSEDEVICE, compat_alcGetProcAddress (device, "alcCloseDevice"));
        alcGetError                                                     = REINTERPRET_CAST(LPALCGETERROR, compat_alcGetProcAddress (device, "alcGetError"));
        alcIsExtensionPresent                                           = REINTERPRET_CAST(LPALCISEXTENSIONPRESENT, compat_alcGetProcAddress (device, "alcIsExtensionPresent"));
        alcGetEnumValue                                                 = REINTERPRET_CAST(LPALCGETENUMVALUE, compat_alcGetProcAddress (device, "alcGetEnumValue"));
        alcGetString                                                    = REINTERPRET_CAST(LPALCGETSTRING, compat_alcGetProcAddress (device, "alcGetString"));
        alcGetIntegerv                                                  = REINTERPRET_CAST(LPALCGETINTEGERV, compat_alcGetProcAddress (device, "alcGetIntegerv"));
        alcCaptureOpenDevice                                            = REINTERPRET_CAST(LPALCCAPTUREOPENDEVICE, compat_alcGetProcAddress (device, "alcCaptureOpenDevice"));
        alcCaptureCloseDevice                                           = REINTERPRET_CAST(LPALCCAPTURECLOSEDEVICE, compat_alcGetProcAddress (device, "alcCaptureCloseDevice"));
        alcCaptureStart                                                 = REINTERPRET_CAST(LPALCCAPTURESTART, compat_alcGetProcAddress (device, "alcCaptureStart"));
        alcCaptureStop                                                  = REINTERPRET_CAST(LPALCCAPTURESTOP, compat_alcGetProcAddress (device, "alcCaptureStop"));
        alcCaptureSamples                                               = REINTERPRET_CAST(LPALCCAPTURESAMPLES, compat_alcGetProcAddress (device, "alcCaptureSamples"));
}

static void alad_load_alc_extension_functions_ (ALCdevice *device) {
        if (alcGetProcAddress == nullptr) return;
        ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS_ compat_alcGetProcAddress = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS_, alcGetProcAddress);
        //ALC extensions
        //ALC_EXT_thread_local_context
        alcSetThreadContext                                             = REINTERPRET_CAST(PFNALCSETTHREADCONTEXTPROC, compat_alcGetProcAddress (device, "alcSetThreadContext"));
        alcGetThreadContext                                             = REINTERPRET_CAST(PFNALCGETTHREADCONTEXTPROC, compat_alcGetProcAddress (device, "alcGetThreadContext"));
        //ALC_SOFT_loopback
        alcLoopbackOpenDeviceSOFT      = REINTERPRET_CAST(LPALCLOOPBACKOPENDEVICESOFT, compat_alcGetProcAddress (device, "alcLoopbackOpenDeviceSOFT"));
        alcIsRenderFormatSupportedSOFT = REINTERPRET_CAST(LPALCISRENDERFORMATSUPPORTEDSOFT, compat_alcGetProcAddress (device, "alcIsRenderFormatSupportedSOFT"));
        alcRenderSamplesSOFT           = REINTERPRET_CAST(LPALCRENDERSAMPLESSOFT, compat_alcGetProcAddress (device, "alcRenderSamplesSOFT"));
        //ALC_SOFT_pause_device
        alcDevicePauseSOFT             = REINTERPRET_CAST(LPALCDEVICEPAUSESOFT, compat_alcGetProcAddress (device, "alcDevicePauseSOFT"));
        alcDeviceResumeSOFT            = REINTERPRET_CAST(LPALCDEVICERESUMESOFT, compat_alcGetProcAddress (device, "alcDeviceResumeSOFT"));
        //ALC_SOFT_HRTF
        alcGetStringiSOFT              = REINTERPRET_CAST(LPALCGETSTRINGISOFT, compat_alcGetProcAddress (device, "alcGetStringiSOFT"));
        alcResetDeviceSOFT             = REINTERPRET_CAST(LPALCRESETDEVICESOFT, compat_alcGetProcAddress (device, "alcResetDeviceSOFT"));
        //ALC_SOFT_device_clock
        alcGetInteger64vSOFT           = REINTERPRET_CAST(LPALCGETINTEGER64VSOFT, compat_alcGetProcAddress (device, "alcGetInteger64vSOFT"));
        //ALC_SOFT_reopen_device
        alcReopenDeviceSOFT            = REINTERPRET_CAST(LPALCREOPENDEVICESOFT, compat_alcGetProcAddress (device, "alcReopenDeviceSOFT"));
}



//modelled after GLFW, see win32_module.c and posix_module.c specifically
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__)
#define NOMINMAX
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static void *alad_open_ (const char *path) {
        return LoadLibraryA (path);
}
static alad_proc_ alad_load_ (void *module, const char *name) {
        return (alad_proc_) GetProcAddress ((HMODULE) module, name);
}
static void alad_close_ (void *module) {
        FreeLibrary ((HMODULE) module);
}
#define alad_LIB_NAME_           "OpenAL32.dll"
#define alad_SECONDARY_LIB_NAME_ "soft_oal.dll"
#else /* Unix defaults otherwise */
#include <dlfcn.h>
static void *alad_open_ (const char *path) {
        return dlopen (path, RTLD_LAZY | RTLD_LOCAL);
}
static alad_proc_ alad_load_ (void *module, const char *name) {
        ALAD_ISO_C_COMPAT_dlsym_ compat_dlsym = REINTERPRET_CAST(ALAD_ISO_C_COMPAT_dlsym_, dlsym);
        return REINTERPRET_CAST(alad_proc_, compat_dlsym (module, name));
}
static void alad_close_ (void *module) {
        dlclose (module);
}

//there are also libopenal.so.1.[X].[Y] and libopenal.1.[X].[Y].dylib respectively, but it would be difficult to look all of those up
#if defined(__APPLE__)
//not tested myself; the only references I could find are https://github.com/ToweOPrO/sadsad and https://pastebin.com/MEmh3ZFr, which is at least tenuous
#define alad_LIB_NAME_           "libopenal.1.dylib"
#define alad_SECONDARY_LIB_NAME_ "libopenal.dylib"
#else
#define alad_LIB_NAME_           "libopenal.so.1"
#define alad_SECONDARY_LIB_NAME_ "libopenal.so"
#endif

#endif /* _WIN32 */



static void *alad_module_ = nullptr;

static void  alad_load_lib_ () {
        //don't load shared object twice
        if (alad_module_ != nullptr) return;
        //use fallback so name
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



//manual interface
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
        ALboolean   switchContext = (context != nullptr);
        ALCcontext *oldContext = nullptr;
        if (switchContext) {
                oldContext = alcGetCurrentContext();
                alcMakeContextCurrent (context);
        }
        if (extensionsOnly == AL_FALSE) {
                alad_load_al_functions_();
        }
        alad_load_al_extension_functions_();
        if (switchContext) {
                alcMakeContextCurrent (oldContext);
        }
}

void aladUpdateALCPointersFromContext (ALCcontext *context, ALboolean extensionsOnly) {
        ALboolean   switchContext = (context != nullptr);
        ALCcontext *oldContext = nullptr;
        if (switchContext) {
                oldContext = alcGetCurrentContext();
                alcMakeContextCurrent (context);
        }
        if (extensionsOnly == AL_FALSE) {
                alad_load_alc_functions_from_al_();
        }
        alad_load_alc_extension_functions_from_al_();
        if (switchContext) {
                alcMakeContextCurrent (oldContext);
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



//simplified Interface
void aladLoadAL () {
        aladLoadALContextFree (AL_TRUE);
}
void aladUpdateAL () {
        //load extensions with alGetProcAddress
        alad_load_al_extension_functions_();
        //use current contexts device to load ALC extensions with alcGetProcAddress
        ALCdevice *device = alcGetContextsDevice (alcGetCurrentContext());
        alad_load_alc_extension_functions_ (device);
}



#endif        // ALAD_IMPLEMENTATION

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif        // ALAD_H
