/*
 *  alad - a glad-like header-only C99 OpenAL-loader by Hypatia of Sva. Look for the current version at: https://github.com/hypatia-of-sva/alad
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
 *  Make sure one of these dynamic libraries are on path for LoadLibraryA / dlopen, change the code below in _alad_open or provide your own function loader.
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
 *          strings in the internal _alad_load_alc_functions function.
 * 
 */


#pragma once

//only include alad if the OpenAL headers are not used, because we include them again, and they may have been included with prototypes
#if !(defined(ALAD_H)) && !(defined(AL_AL_H)) && !(defined(AL_ALC_H)) && !(defined(AL_ALEXT_H)) && \
        !(defined(AL_EFX_H)) && !(defined(EFX_PRESETS_H))
#define ALAD_H

//revision date
#define ALAD_HEADER_REVISION 0x20221230

#if defined(__cplusplus)
extern "C" {
#endif


#define AL_NO_PROTOTYPES
#define ALC_NO_PROTOTYPES
#include <AL/alext.h>
#include <AL/efx-presets.h>
//currently it can't be checked whether or not the headers work properly, but compilation will fail due to double definition.



#ifdef ALAD_IMPLEMENTATION

//Core AL
LPALDOPPLERFACTOR                alDopplerFactor                = NULL;
LPALDOPPLERVELOCITY              alDopplerVelocity              = NULL;
LPALSPEEDOFSOUND                 alSpeedOfSound                 = NULL;
LPALDISTANCEMODEL                alDistanceModel                = NULL;
LPALENABLE                       alEnable                       = NULL;
LPALDISABLE                      alDisable                      = NULL;
LPALISENABLED                    alIsEnabled                    = NULL;
LPALGETSTRING                    alGetString                    = NULL;
LPALGETBOOLEANV                  alGetBooleanv                  = NULL;
LPALGETINTEGERV                  alGetIntegerv                  = NULL;
LPALGETFLOATV                    alGetFloatv                    = NULL;
LPALGETDOUBLEV                   alGetDoublev                   = NULL;
LPALGETBOOLEAN                   alGetBoolean                   = NULL;
LPALGETINTEGER                   alGetInteger                   = NULL;
LPALGETFLOAT                     alGetFloat                     = NULL;
LPALGETDOUBLE                    alGetDouble                    = NULL;
LPALGETERROR                     alGetError                     = NULL;
LPALISEXTENSIONPRESENT           alIsExtensionPresent           = NULL;
LPALGETPROCADDRESS               alGetProcAddress               = NULL;
LPALGETENUMVALUE                 alGetEnumValue                 = NULL;
LPALLISTENERF                    alListenerf                    = NULL;
LPALLISTENER3F                   alListener3f                   = NULL;
LPALLISTENERFV                   alListenerfv                   = NULL;
LPALLISTENERI                    alListeneri                    = NULL;
LPALLISTENER3I                   alListener3i                   = NULL;
LPALLISTENERIV                   alListeneriv                   = NULL;
LPALGETLISTENERF                 alGetListenerf                 = NULL;
LPALGETLISTENER3F                alGetListener3f                = NULL;
LPALGETLISTENERFV                alGetListenerfv                = NULL;
LPALGETLISTENERI                 alGetListeneri                 = NULL;
LPALGETLISTENER3I                alGetListener3i                = NULL;
LPALGETLISTENERIV                alGetListeneriv                = NULL;
LPALGENSOURCES                   alGenSources                   = NULL;
LPALDELETESOURCES                alDeleteSources                = NULL;
LPALISSOURCE                     alIsSource                     = NULL;
LPALSOURCEF                      alSourcef                      = NULL;
LPALSOURCE3F                     alSource3f                     = NULL;
LPALSOURCEFV                     alSourcefv                     = NULL;
LPALSOURCEI                      alSourcei                      = NULL;
LPALSOURCE3I                     alSource3i                     = NULL;
LPALSOURCEIV                     alSourceiv                     = NULL;
LPALGETSOURCEF                   alGetSourcef                   = NULL;
LPALGETSOURCE3F                  alGetSource3f                  = NULL;
LPALGETSOURCEFV                  alGetSourcefv                  = NULL;
LPALGETSOURCEI                   alGetSourcei                   = NULL;
LPALGETSOURCE3I                  alGetSource3i                  = NULL;
LPALGETSOURCEIV                  alGetSourceiv                  = NULL;
LPALSOURCEPLAYV                  alSourcePlayv                  = NULL;
LPALSOURCESTOPV                  alSourceStopv                  = NULL;
LPALSOURCEREWINDV                alSourceRewindv                = NULL;
LPALSOURCEPAUSEV                 alSourcePausev                 = NULL;
LPALSOURCEPLAY                   alSourcePlay                   = NULL;
LPALSOURCESTOP                   alSourceStop                   = NULL;
LPALSOURCEREWIND                 alSourceRewind                 = NULL;
LPALSOURCEPAUSE                  alSourcePause                  = NULL;
LPALSOURCEQUEUEBUFFERS           alSourceQueueBuffers           = NULL;
LPALSOURCEUNQUEUEBUFFERS         alSourceUnqueueBuffers         = NULL;
LPALGENBUFFERS                   alGenBuffers                   = NULL;
LPALDELETEBUFFERS                alDeleteBuffers                = NULL;
LPALISBUFFER                     alIsBuffer                     = NULL;
LPALBUFFERDATA                   alBufferData                   = NULL;
LPALBUFFERF                      alBufferf                      = NULL;
LPALBUFFER3F                     alBuffer3f                     = NULL;
LPALBUFFERFV                     alBufferfv                     = NULL;
LPALBUFFERI                      alBufferi                      = NULL;
LPALBUFFER3I                     alBuffer3i                     = NULL;
LPALBUFFERIV                     alBufferiv                     = NULL;
LPALGETBUFFERF                   alGetBufferf                   = NULL;
LPALGETBUFFER3F                  alGetBuffer3f                  = NULL;
LPALGETBUFFERFV                  alGetBufferfv                  = NULL;
LPALGETBUFFERI                   alGetBufferi                   = NULL;
LPALGETBUFFER3I                  alGetBuffer3i                  = NULL;
LPALGETBUFFERIV                  alGetBufferiv                  = NULL;

//EFX
LPALGENEFFECTS                   alGenEffects                   = NULL;
LPALDELETEEFFECTS                alDeleteEffects                = NULL;
LPALISEFFECT                     alIsEffect                     = NULL;
LPALEFFECTI                      alEffecti                      = NULL;
LPALEFFECTIV                     alEffectiv                     = NULL;
LPALEFFECTF                      alEffectf                      = NULL;
LPALEFFECTFV                     alEffectfv                     = NULL;
LPALGETEFFECTI                   alGetEffecti                   = NULL;
LPALGETEFFECTIV                  alGetEffectiv                  = NULL;
LPALGETEFFECTF                   alGetEffectf                   = NULL;
LPALGETEFFECTFV                  alGetEffectfv                  = NULL;
LPALGENFILTERS                   alGenFilters                   = NULL;
LPALDELETEFILTERS                alDeleteFilters                = NULL;
LPALISFILTER                     alIsFilter                     = NULL;
LPALFILTERI                      alFilteri                      = NULL;
LPALFILTERIV                     alFilteriv                     = NULL;
LPALFILTERF                      alFilterf                      = NULL;
LPALFILTERFV                     alFilterfv                     = NULL;
LPALGETFILTERI                   alGetFilteri                   = NULL;
LPALGETFILTERIV                  alGetFilteriv                  = NULL;
LPALGETFILTERF                   alGetFilterf                   = NULL;
LPALGETFILTERFV                  alGetFilterfv                  = NULL;
LPALGENAUXILIARYEFFECTSLOTS      alGenAuxiliaryEffectSlots      = NULL;
LPALDELETEAUXILIARYEFFECTSLOTS   alDeleteAuxiliaryEffectSlots   = NULL;
LPALISAUXILIARYEFFECTSLOT        alIsAuxiliaryEffectSlot        = NULL;
LPALAUXILIARYEFFECTSLOTI         alAuxiliaryEffectSloti         = NULL;
LPALAUXILIARYEFFECTSLOTIV        alAuxiliaryEffectSlotiv        = NULL;
LPALAUXILIARYEFFECTSLOTF         alAuxiliaryEffectSlotf         = NULL;
LPALAUXILIARYEFFECTSLOTFV        alAuxiliaryEffectSlotfv        = NULL;
LPALGETAUXILIARYEFFECTSLOTI      alGetAuxiliaryEffectSloti      = NULL;
LPALGETAUXILIARYEFFECTSLOTIV     alGetAuxiliaryEffectSlotiv     = NULL;
LPALGETAUXILIARYEFFECTSLOTF      alGetAuxiliaryEffectSlotf      = NULL;
LPALGETAUXILIARYEFFECTSLOTFV     alGetAuxiliaryEffectSlotfv     = NULL;

//AL extensions
//AL_EXT_STATIC_BUFFER
PFNALBUFFERDATASTATICPROC        alBufferDataStatic             = NULL;
//AL_SOFT_buffer_sub_data
PFNALBUFFERSUBDATASOFTPROC       alBufferSubDataSOFT            = NULL;
//AL_EXT_FOLDBACK
LPALREQUESTFOLDBACKSTART         alRequestFoldbackStart         = NULL;
LPALREQUESTFOLDBACKSTOP          alRequestFoldbackStop          = NULL;
//AL_SOFT_buffer_samples
LPALBUFFERSAMPLESSOFT            alBufferSamplesSOFT            = NULL;
LPALBUFFERSUBSAMPLESSOFT         alBufferSubSamplesSOFT         = NULL;
LPALGETBUFFERSAMPLESSOFT         alGetBufferSamplesSOFT         = NULL;
LPALISBUFFERFORMATSUPPORTEDSOFT  alIsBufferFormatSupportedSOFT  = NULL;
//AL_SOFT_source_latency
LPALSOURCEDSOFT                  alSourcedSOFT                  = NULL;
LPALSOURCE3DSOFT                 alSource3dSOFT                 = NULL;
LPALSOURCEDVSOFT                 alSourcedvSOFT                 = NULL;
LPALGETSOURCEDSOFT               alGetSourcedSOFT               = NULL;
LPALGETSOURCE3DSOFT              alGetSource3dSOFT              = NULL;
LPALGETSOURCEDVSOFT              alGetSourcedvSOFT              = NULL;
LPALSOURCEI64SOFT                alSourcei64SOFT                = NULL;
LPALSOURCE3I64SOFT               alSource3i64SOFT               = NULL;
LPALSOURCEI64VSOFT               alSourcei64vSOFT               = NULL;
LPALGETSOURCEI64SOFT             alGetSourcei64SOFT             = NULL;
LPALGETSOURCE3I64SOFT            alGetSource3i64SOFT            = NULL;
LPALGETSOURCEI64VSOFT            alGetSourcei64vSOFT            = NULL;
//AL_SOFT_deferred_updates
LPALDEFERUPDATESSOFT             alDeferUpdatesSOFT             = NULL;
LPALPROCESSUPDATESSOFT           alProcessUpdatesSOFT           = NULL;
//AL_SOFT_source_resampler
LPALGETSTRINGISOFT               alGetStringiSOFT               = NULL;
//AL_SOFT_events
LPALEVENTCONTROLSOFT             alEventControlSOFT             = NULL;
LPALEVENTCALLBACKSOFT            alEventCallbackSOFT            = NULL;
LPALGETPOINTERSOFT               alGetPointerSOFT               = NULL;
LPALGETPOINTERVSOFT              alGetPointervSOFT              = NULL;
//AL_SOFT_callback_buffer
LPALBUFFERCALLBACKSOFT           alBufferCallbackSOFT           = NULL;
LPALGETBUFFERPTRSOFT             alGetBufferPtrSOFT             = NULL;
LPALGETBUFFER3PTRSOFT            alGetBuffer3PtrSOFT            = NULL;
LPALGETBUFFERPTRVSOFT            alGetBufferPtrvSOFT            = NULL;

//Core ALC
LPALCCREATECONTEXT               alcCreateContext               = NULL;
LPALCMAKECONTEXTCURRENT          alcMakeContextCurrent          = NULL;
LPALCPROCESSCONTEXT              alcProcessContext              = NULL;
LPALCSUSPENDCONTEXT              alcSuspendContext              = NULL;
LPALCDESTROYCONTEXT              alcDestroyContext              = NULL;
LPALCGETCURRENTCONTEXT           alcGetCurrentContext           = NULL;
LPALCGETCONTEXTSDEVICE           alcGetContextsDevice           = NULL;
LPALCOPENDEVICE                  alcOpenDevice                  = NULL;
LPALCCLOSEDEVICE                 alcCloseDevice                 = NULL;
LPALCGETERROR                    alcGetError                    = NULL;
LPALCISEXTENSIONPRESENT          alcIsExtensionPresent          = NULL;
LPALCGETPROCADDRESS              alcGetProcAddress              = NULL;
LPALCGETENUMVALUE                alcGetEnumValue                = NULL;
LPALCGETSTRING                   alcGetString                   = NULL;
LPALCGETINTEGERV                 alcGetIntegerv                 = NULL;
LPALCCAPTUREOPENDEVICE           alcCaptureOpenDevice           = NULL;
LPALCCAPTURECLOSEDEVICE          alcCaptureCloseDevice          = NULL;
LPALCCAPTURESTART                alcCaptureStart                = NULL;
LPALCCAPTURESTOP                 alcCaptureStop                 = NULL;
LPALCCAPTURESAMPLES              alcCaptureSamples              = NULL;

//ALC extensions
//ALC_EXT_thread_local_context
PFNALCSETTHREADCONTEXTPROC       alcSetThreadContext            = NULL;
PFNALCGETTHREADCONTEXTPROC       alcGetThreadContext            = NULL;
//ALC_SOFT_loopback
LPALCLOOPBACKOPENDEVICESOFT      alcLoopbackOpenDeviceSOFT      = NULL;
LPALCISRENDERFORMATSUPPORTEDSOFT alcIsRenderFormatSupportedSOFT = NULL;
LPALCRENDERSAMPLESSOFT           alcRenderSamplesSOFT           = NULL;
//ALC_SOFT_pause_device
LPALCDEVICEPAUSESOFT             alcDevicePauseSOFT             = NULL;
LPALCDEVICERESUMESOFT            alcDeviceResumeSOFT            = NULL;
//ALC_SOFT_HRTF
LPALCGETSTRINGISOFT              alcGetStringiSOFT              = NULL;
LPALCRESETDEVICESOFT             alcResetDeviceSOFT             = NULL;
//ALC_SOFT_device_clock
LPALCGETINTEGER64VSOFT           alcGetInteger64vSOFT           = NULL;
//ALC_SOFT_reopen_device
LPALCREOPENDEVICESOFT            alcReopenDeviceSOFT            = NULL;



//implemented down below
typedef void (*_alad_proc) (void);
void      *_alad_open (const char *path);
_alad_proc _alad_load (void *module, const char *name);
void       _alad_close (void *module);

        
//ISO C compatibility types for warning: ISO C forbids conversion of object pointer to function pointer type [-Wpedantic]
typedef _alad_proc         (AL_APIENTRY *_ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS)(const ALchar *fname);
typedef _alad_proc         (ALC_APIENTRY *_ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS)(ALCdevice *device, const ALCchar *funcname);

void       _alad_load_al_functions_contextfree_dlsym (void *module, ALboolean loadAll) {
        if (module == NULL) return;
        //Core AL functions without buffer, source, listener and doppler/distance functions, because none of these are necessary to create a context
        alGetProcAddress     = (LPALGETPROCADDRESS               ) _alad_load (module, "alGetProcAddress");
        alEnable             = (LPALENABLE                       ) _alad_load (module, "alEnable");
        alDisable            = (LPALDISABLE                      ) _alad_load (module, "alDisable");
        alIsEnabled          = (LPALISENABLED                    ) _alad_load (module, "alIsEnabled");
        alGetString          = (LPALGETSTRING                    ) _alad_load (module, "alGetString");
        alGetBooleanv        = (LPALGETBOOLEANV                  ) _alad_load (module, "alGetBooleanv");
        alGetIntegerv        = (LPALGETINTEGERV                  ) _alad_load (module, "alGetIntegerv");
        alGetFloatv          = (LPALGETFLOATV                    ) _alad_load (module, "alGetFloatv");
        alGetDoublev         = (LPALGETDOUBLEV                   ) _alad_load (module, "alGetDoublev");
        alGetBoolean         = (LPALGETBOOLEAN                   ) _alad_load (module, "alGetBoolean");
        alGetInteger         = (LPALGETINTEGER                   ) _alad_load (module, "alGetInteger");
        alGetFloat           = (LPALGETFLOAT                     ) _alad_load (module, "alGetFloat");
        alGetDouble          = (LPALGETDOUBLE                    ) _alad_load (module, "alGetDouble");
        alGetError           = (LPALGETERROR                     ) _alad_load (module, "alGetError");
        alIsExtensionPresent = (LPALISEXTENSIONPRESENT           ) _alad_load (module, "alIsExtensionPresent");
        alGetEnumValue       = (LPALGETENUMVALUE                 ) _alad_load (module, "alGetEnumValue");
        //the rest if wanted
        if (loadAll != AL_FALSE) {
                alDopplerFactor        = (LPALDOPPLERFACTOR                ) _alad_load (module, "alDopplerFactor");
                alDopplerVelocity      = (LPALDOPPLERVELOCITY              ) _alad_load (module, "alDopplerVelocity");
                alSpeedOfSound         = (LPALSPEEDOFSOUND                 ) _alad_load (module, "alSpeedOfSound");
                alDistanceModel        = (LPALDISTANCEMODEL                ) _alad_load (module, "alDistanceModel");
                alListenerf            = (LPALLISTENERF                    ) _alad_load (module, "alListenerf");
                alListener3f           = (LPALLISTENER3F                   ) _alad_load (module, "alListener3f");
                alListenerfv           = (LPALLISTENERFV                   ) _alad_load (module, "alListenerfv");
                alListeneri            = (LPALLISTENERI                    ) _alad_load (module, "alListeneri");
                alListener3i           = (LPALLISTENER3I                   ) _alad_load (module, "alListener3i");
                alListeneriv           = (LPALLISTENERIV                   ) _alad_load (module, "alListeneriv");
                alGetListenerf         = (LPALGETLISTENERF                 ) _alad_load (module, "alGetListenerf");
                alGetListener3f        = (LPALGETLISTENER3F                ) _alad_load (module, "alGetListener3f");
                alGetListenerfv        = (LPALGETLISTENERFV                ) _alad_load (module, "alGetListenerfv");
                alGetListeneri         = (LPALGETLISTENERI                 ) _alad_load (module, "alGetListeneri");
                alGetListener3i        = (LPALGETLISTENER3I                ) _alad_load (module, "alGetListener3i");
                alGetListeneriv        = (LPALGETLISTENERIV                ) _alad_load (module, "alGetListeneriv");
                alGenSources           = (LPALGENSOURCES                   ) _alad_load (module, "alGenSources");
                alDeleteSources        = (LPALDELETESOURCES                ) _alad_load (module, "alDeleteSources");
                alIsSource             = (LPALISSOURCE                     ) _alad_load (module, "alIsSource");
                alSourcef              = (LPALSOURCEF                      ) _alad_load (module, "alSourcef");
                alSource3f             = (LPALSOURCE3F                     ) _alad_load (module, "alSource3f");
                alSourcefv             = (LPALSOURCEFV                     ) _alad_load (module, "alSourcefv");
                alSourcei              = (LPALSOURCEI                      ) _alad_load (module, "alSourcei");
                alSource3i             = (LPALSOURCE3I                     ) _alad_load (module, "alSource3i");
                alSourceiv             = (LPALSOURCEIV                     ) _alad_load (module, "alSourceiv");
                alGetSourcef           = (LPALGETSOURCEF                   ) _alad_load (module, "alGetSourcef");
                alGetSource3f          = (LPALGETSOURCE3F                  ) _alad_load (module, "alGetSource3f");
                alGetSourcefv          = (LPALGETSOURCEFV                  ) _alad_load (module, "alGetSourcefv");
                alGetSourcei           = (LPALGETSOURCEI                   ) _alad_load (module, "alGetSourcei");
                alGetSource3i          = (LPALGETSOURCE3I                  ) _alad_load (module, "alGetSource3i");
                alGetSourceiv          = (LPALGETSOURCEIV                  ) _alad_load (module, "alGetSourceiv");
                alSourcePlayv          = (LPALSOURCEPLAYV                  ) _alad_load (module, "alSourcePlayv");
                alSourceStopv          = (LPALSOURCESTOPV                  ) _alad_load (module, "alSourceStopv");
                alSourceRewindv        = (LPALSOURCEREWINDV                ) _alad_load (module, "alSourceRewindv");
                alSourcePausev         = (LPALSOURCEPAUSEV                 ) _alad_load (module, "alSourcePausev");
                alSourcePlay           = (LPALSOURCEPLAY                   ) _alad_load (module, "alSourcePlay");
                alSourceStop           = (LPALSOURCESTOP                   ) _alad_load (module, "alSourceStop");
                alSourceRewind         = (LPALSOURCEREWIND                 ) _alad_load (module, "alSourceRewind");
                alSourcePause          = (LPALSOURCEPAUSE                  ) _alad_load (module, "alSourcePause");
                alSourceQueueBuffers   = (LPALSOURCEQUEUEBUFFERS           ) _alad_load (module, "alSourceQueueBuffers");
                alSourceUnqueueBuffers = (LPALSOURCEUNQUEUEBUFFERS         ) _alad_load (module, "alSourceUnqueueBuffers");
                alGenBuffers           = (LPALGENBUFFERS                   ) _alad_load (module, "alGenBuffers");
                alDeleteBuffers        = (LPALDELETEBUFFERS                ) _alad_load (module, "alDeleteBuffers");
                alIsBuffer             = (LPALISBUFFER                     ) _alad_load (module, "alIsBuffer");
                alBufferData           = (LPALBUFFERDATA                   ) _alad_load (module, "alBufferData");
                alBufferf              = (LPALBUFFERF                      ) _alad_load (module, "alBufferf");
                alBuffer3f             = (LPALBUFFER3F                     ) _alad_load (module, "alBuffer3f");
                alBufferfv             = (LPALBUFFERFV                     ) _alad_load (module, "alBufferfv");
                alBufferi              = (LPALBUFFERI                      ) _alad_load (module, "alBufferi");
                alBuffer3i             = (LPALBUFFER3I                     ) _alad_load (module, "alBuffer3i");
                alBufferiv             = (LPALBUFFERIV                     ) _alad_load (module, "alBufferiv");
                alGetBufferf           = (LPALGETBUFFERF                   ) _alad_load (module, "alGetBufferf");
                alGetBuffer3f          = (LPALGETBUFFER3F                  ) _alad_load (module, "alGetBuffer3f");
                alGetBufferfv          = (LPALGETBUFFERFV                  ) _alad_load (module, "alGetBufferfv");
                alGetBufferi           = (LPALGETBUFFERI                   ) _alad_load (module, "alGetBufferi");
                alGetBuffer3i          = (LPALGETBUFFER3I                  ) _alad_load (module, "alGetBuffer3i");
                alGetBufferiv          = (LPALGETBUFFERIV                  ) _alad_load (module, "alGetBufferiv");
        }
}

void _alad_load_al_functions () {
        if (alGetProcAddress == NULL) return;
        _ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS compat_alGetProcAddress = (_ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS) alGetProcAddress;
        //Core AL
        alDopplerFactor        = (LPALDOPPLERFACTOR                ) compat_alGetProcAddress ("alDopplerFactor");
        alDopplerVelocity      = (LPALDOPPLERVELOCITY              ) compat_alGetProcAddress ("alDopplerVelocity");
        alSpeedOfSound         = (LPALSPEEDOFSOUND                 ) compat_alGetProcAddress ("alSpeedOfSound");
        alDistanceModel        = (LPALDISTANCEMODEL                ) compat_alGetProcAddress ("alDistanceModel");
        alEnable               = (LPALENABLE                       ) compat_alGetProcAddress ("alEnable");
        alDisable              = (LPALDISABLE                      ) compat_alGetProcAddress ("alDisable");
        alIsEnabled            = (LPALISENABLED                    ) compat_alGetProcAddress ("alIsEnabled");
        alGetString            = (LPALGETSTRING                    ) compat_alGetProcAddress ("alGetString");
        alGetBooleanv          = (LPALGETBOOLEANV                  ) compat_alGetProcAddress ("alGetBooleanv");
        alGetIntegerv          = (LPALGETINTEGERV                  ) compat_alGetProcAddress ("alGetIntegerv");
        alGetFloatv            = (LPALGETFLOATV                    ) compat_alGetProcAddress ("alGetFloatv");
        alGetDoublev           = (LPALGETDOUBLEV                   ) compat_alGetProcAddress ("alGetDoublev");
        alGetBoolean           = (LPALGETBOOLEAN                   ) compat_alGetProcAddress ("alGetBoolean");
        alGetInteger           = (LPALGETINTEGER                   ) compat_alGetProcAddress ("alGetInteger");
        alGetFloat             = (LPALGETFLOAT                     ) compat_alGetProcAddress ("alGetFloat");
        alGetDouble            = (LPALGETDOUBLE                    ) compat_alGetProcAddress ("alGetDouble");
        alGetError             = (LPALGETERROR                     ) compat_alGetProcAddress ("alGetError");
        alIsExtensionPresent   = (LPALISEXTENSIONPRESENT           ) compat_alGetProcAddress ("alIsExtensionPresent");
        alGetEnumValue         = (LPALGETENUMVALUE                 ) compat_alGetProcAddress ("alGetEnumValue");
        alListenerf            = (LPALLISTENERF                    ) compat_alGetProcAddress ("alListenerf");
        alListener3f           = (LPALLISTENER3F                   ) compat_alGetProcAddress ("alListener3f");
        alListenerfv           = (LPALLISTENERFV                   ) compat_alGetProcAddress ("alListenerfv");
        alListeneri            = (LPALLISTENERI                    ) compat_alGetProcAddress ("alListeneri");
        alListener3i           = (LPALLISTENER3I                   ) compat_alGetProcAddress ("alListener3i");
        alListeneriv           = (LPALLISTENERIV                   ) compat_alGetProcAddress ("alListeneriv");
        alGetListenerf         = (LPALGETLISTENERF                 ) compat_alGetProcAddress ("alGetListenerf");
        alGetListener3f        = (LPALGETLISTENER3F                ) compat_alGetProcAddress ("alGetListener3f");
        alGetListenerfv        = (LPALGETLISTENERFV                ) compat_alGetProcAddress ("alGetListenerfv");
        alGetListeneri         = (LPALGETLISTENERI                 ) compat_alGetProcAddress ("alGetListeneri");
        alGetListener3i        = (LPALGETLISTENER3I                ) compat_alGetProcAddress ("alGetListener3i");
        alGetListeneriv        = (LPALGETLISTENERIV                ) compat_alGetProcAddress ("alGetListeneriv");
        alGenSources           = (LPALGENSOURCES                   ) compat_alGetProcAddress ("alGenSources");
        alDeleteSources        = (LPALDELETESOURCES                ) compat_alGetProcAddress ("alDeleteSources");
        alIsSource             = (LPALISSOURCE                     ) compat_alGetProcAddress ("alIsSource");
        alSourcef              = (LPALSOURCEF                      ) compat_alGetProcAddress ("alSourcef");
        alSource3f             = (LPALSOURCE3F                     ) compat_alGetProcAddress ("alSource3f");
        alSourcefv             = (LPALSOURCEFV                     ) compat_alGetProcAddress ("alSourcefv");
        alSourcei              = (LPALSOURCEI                      ) compat_alGetProcAddress ("alSourcei");
        alSource3i             = (LPALSOURCE3I                     ) compat_alGetProcAddress ("alSource3i");
        alSourceiv             = (LPALSOURCEIV                     ) compat_alGetProcAddress ("alSourceiv");
        alGetSourcef           = (LPALGETSOURCEF                   ) compat_alGetProcAddress ("alGetSourcef");
        alGetSource3f          = (LPALGETSOURCE3F                  ) compat_alGetProcAddress ("alGetSource3f");
        alGetSourcefv          = (LPALGETSOURCEFV                  ) compat_alGetProcAddress ("alGetSourcefv");
        alGetSourcei           = (LPALGETSOURCEI                   ) compat_alGetProcAddress ("alGetSourcei");
        alGetSource3i          = (LPALGETSOURCE3I                  ) compat_alGetProcAddress ("alGetSource3i");
        alGetSourceiv          = (LPALGETSOURCEIV                  ) compat_alGetProcAddress ("alGetSourceiv");
        alSourcePlayv          = (LPALSOURCEPLAYV                  ) compat_alGetProcAddress ("alSourcePlayv");
        alSourceStopv          = (LPALSOURCESTOPV                  ) compat_alGetProcAddress ("alSourceStopv");
        alSourceRewindv        = (LPALSOURCEREWINDV                ) compat_alGetProcAddress ("alSourceRewindv");
        alSourcePausev         = (LPALSOURCEPAUSEV                 ) compat_alGetProcAddress ("alSourcePausev");
        alSourcePlay           = (LPALSOURCEPLAY                   ) compat_alGetProcAddress ("alSourcePlay");
        alSourceStop           = (LPALSOURCESTOP                   ) compat_alGetProcAddress ("alSourceStop");
        alSourceRewind         = (LPALSOURCEREWIND                 ) compat_alGetProcAddress ("alSourceRewind");
        alSourcePause          = (LPALSOURCEPAUSE                  ) compat_alGetProcAddress ("alSourcePause");
        alSourceQueueBuffers   = (LPALSOURCEQUEUEBUFFERS           ) compat_alGetProcAddress ("alSourceQueueBuffers");
        alSourceUnqueueBuffers = (LPALSOURCEUNQUEUEBUFFERS         ) compat_alGetProcAddress ("alSourceUnqueueBuffers");
        alGenBuffers           = (LPALGENBUFFERS                   ) compat_alGetProcAddress ("alGenBuffers");
        alDeleteBuffers        = (LPALDELETEBUFFERS                ) compat_alGetProcAddress ("alDeleteBuffers");
        alIsBuffer             = (LPALISBUFFER                     ) compat_alGetProcAddress ("alIsBuffer");
        alBufferData           = (LPALBUFFERDATA                   ) compat_alGetProcAddress ("alBufferData");
        alBufferf              = (LPALBUFFERF                      ) compat_alGetProcAddress ("alBufferf");
        alBuffer3f             = (LPALBUFFER3F                     ) compat_alGetProcAddress ("alBuffer3f");
        alBufferfv             = (LPALBUFFERFV                     ) compat_alGetProcAddress ("alBufferfv");
        alBufferi              = (LPALBUFFERI                      ) compat_alGetProcAddress ("alBufferi");
        alBuffer3i             = (LPALBUFFER3I                     ) compat_alGetProcAddress ("alBuffer3i");
        alBufferiv             = (LPALBUFFERIV                     ) compat_alGetProcAddress ("alBufferiv");
        alGetBufferf           = (LPALGETBUFFERF                   ) compat_alGetProcAddress ("alGetBufferf");
        alGetBuffer3f          = (LPALGETBUFFER3F                  ) compat_alGetProcAddress ("alGetBuffer3f");
        alGetBufferfv          = (LPALGETBUFFERFV                  ) compat_alGetProcAddress ("alGetBufferfv");
        alGetBufferi           = (LPALGETBUFFERI                   ) compat_alGetProcAddress ("alGetBufferi");
        alGetBuffer3i          = (LPALGETBUFFER3I                  ) compat_alGetProcAddress ("alGetBuffer3i");
        alGetBufferiv          = (LPALGETBUFFERIV                  ) compat_alGetProcAddress ("alGetBufferiv");
}

void _alad_load_al_extension_functions () {
        if (alGetProcAddress == NULL) return;
        _ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS compat_alGetProcAddress = (_ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS) alGetProcAddress;
        //EFX
        alGenEffects                  = (LPALGENEFFECTS                   ) compat_alGetProcAddress ("alGenEffects");
        alDeleteEffects               = (LPALDELETEEFFECTS                ) compat_alGetProcAddress ("alDeleteEffects");
        alIsEffect                    = (LPALISEFFECT                     ) compat_alGetProcAddress ("alIsEffect");
        alEffecti                     = (LPALEFFECTI                      ) compat_alGetProcAddress ("alEffecti");
        alEffectiv                    = (LPALEFFECTIV                     ) compat_alGetProcAddress ("alEffectiv");
        alEffectf                     = (LPALEFFECTF                      ) compat_alGetProcAddress ("alEffectf");
        alEffectfv                    = (LPALEFFECTFV                     ) compat_alGetProcAddress ("alEffectfv");
        alGetEffecti                  = (LPALGETEFFECTI                   ) compat_alGetProcAddress ("alGetEffecti");
        alGetEffectiv                 = (LPALGETEFFECTIV                  ) compat_alGetProcAddress ("alGetEffectiv");
        alGetEffectf                  = (LPALGETEFFECTF                   ) compat_alGetProcAddress ("alGetEffectf");
        alGetEffectfv                 = (LPALGETEFFECTFV                  ) compat_alGetProcAddress ("alGetEffectfv");
        alGenFilters                  = (LPALGENFILTERS                   ) compat_alGetProcAddress ("alGenFilters");
        alDeleteFilters               = (LPALDELETEFILTERS                ) compat_alGetProcAddress ("alDeleteFilters");
        alIsFilter                    = (LPALISFILTER                     ) compat_alGetProcAddress ("alIsFilter");
        alFilteri                     = (LPALFILTERI                      ) compat_alGetProcAddress ("alFilteri");
        alFilteriv                    = (LPALFILTERIV                     ) compat_alGetProcAddress ("alFilteriv");
        alFilterf                     = (LPALFILTERF                      ) compat_alGetProcAddress ("alFilterf");
        alFilterfv                    = (LPALFILTERFV                     ) compat_alGetProcAddress ("alFilterfv");
        alGetFilteri                  = (LPALGETFILTERI                   ) compat_alGetProcAddress ("alGetFilteri");
        alGetFilteriv                 = (LPALGETFILTERIV                  ) compat_alGetProcAddress ("alGetFilteriv");
        alGetFilterf                  = (LPALGETFILTERF                   ) compat_alGetProcAddress ("alGetFilterf");
        alGetFilterfv                 = (LPALGETFILTERFV                  ) compat_alGetProcAddress ("alGetFilterfv");
        alGenAuxiliaryEffectSlots     = (LPALGENAUXILIARYEFFECTSLOTS      ) compat_alGetProcAddress ("alGenAuxiliaryEffectSlots");
        alDeleteAuxiliaryEffectSlots  = (LPALDELETEAUXILIARYEFFECTSLOTS   ) compat_alGetProcAddress ("alDeleteAuxiliaryEffectSlots");
        alIsAuxiliaryEffectSlot       = (LPALISAUXILIARYEFFECTSLOT        ) compat_alGetProcAddress ("alIsAuxiliaryEffectSlot");
        alAuxiliaryEffectSloti        = (LPALAUXILIARYEFFECTSLOTI         ) compat_alGetProcAddress ("alAuxiliaryEffectSloti");
        alAuxiliaryEffectSlotiv       = (LPALAUXILIARYEFFECTSLOTIV        ) compat_alGetProcAddress ("alAuxiliaryEffectSlotiv");
        alAuxiliaryEffectSlotf        = (LPALAUXILIARYEFFECTSLOTF         ) compat_alGetProcAddress ("alAuxiliaryEffectSlotf");
        alAuxiliaryEffectSlotfv       = (LPALAUXILIARYEFFECTSLOTFV        ) compat_alGetProcAddress ("alAuxiliaryEffectSlotfv");
        alGetAuxiliaryEffectSloti     = (LPALGETAUXILIARYEFFECTSLOTI      ) compat_alGetProcAddress ("alGetAuxiliaryEffectSloti");
        alGetAuxiliaryEffectSlotiv    = (LPALGETAUXILIARYEFFECTSLOTIV     ) compat_alGetProcAddress ("alGetAuxiliaryEffectSlotiv");
        alGetAuxiliaryEffectSlotf     = (LPALGETAUXILIARYEFFECTSLOTF      ) compat_alGetProcAddress ("alGetAuxiliaryEffectSlotf");
        alGetAuxiliaryEffectSlotfv    = (LPALGETAUXILIARYEFFECTSLOTFV     ) compat_alGetProcAddress ("alGetAuxiliaryEffectSlotfv");

        //AL extensions
        //AL_EXT_STATIC_BUFFER
        alBufferDataStatic            = (PFNALBUFFERDATASTATICPROC        ) compat_alGetProcAddress ("alBufferDataStatic");
        //AL_SOFT_buffer_sub_data
        alBufferSubDataSOFT           = (PFNALBUFFERSUBDATASOFTPROC       ) compat_alGetProcAddress ("alBufferSubDataSOFT");
        //AL_EXT_FOLDBACK
        alRequestFoldbackStart        = (LPALREQUESTFOLDBACKSTART         ) compat_alGetProcAddress ("alRequestFoldbackStart");
        alRequestFoldbackStop         = (LPALREQUESTFOLDBACKSTOP          ) compat_alGetProcAddress ("alRequestFoldbackStop");
        //AL_SOFT_buffer_samples
        alBufferSamplesSOFT           = (LPALBUFFERSAMPLESSOFT            ) compat_alGetProcAddress ("alBufferSamplesSOFT");
        alBufferSubSamplesSOFT        = (LPALBUFFERSUBSAMPLESSOFT         ) compat_alGetProcAddress ("alBufferSubSamplesSOFT");
        alGetBufferSamplesSOFT        = (LPALGETBUFFERSAMPLESSOFT         ) compat_alGetProcAddress ("alGetBufferSamplesSOFT");
        alIsBufferFormatSupportedSOFT = (LPALISBUFFERFORMATSUPPORTEDSOFT  ) compat_alGetProcAddress ("alIsBufferFormatSupportedSOFT");
        //AL_SOFT_source_latency
        alSourcedSOFT                 = (LPALSOURCEDSOFT                  ) compat_alGetProcAddress ("alSourcedSOFT");
        alSource3dSOFT                = (LPALSOURCE3DSOFT                 ) compat_alGetProcAddress ("alSource3dSOFT");
        alSourcedvSOFT                = (LPALSOURCEDVSOFT                 ) compat_alGetProcAddress ("alSourcedvSOFT");
        alGetSourcedSOFT              = (LPALGETSOURCEDSOFT               ) compat_alGetProcAddress ("alGetSourcedSOFT");
        alGetSource3dSOFT             = (LPALGETSOURCE3DSOFT              ) compat_alGetProcAddress ("alGetSource3dSOFT");
        alGetSourcedvSOFT             = (LPALGETSOURCEDVSOFT              ) compat_alGetProcAddress ("alGetSourcedvSOFT");
        alSourcei64SOFT               = (LPALSOURCEI64SOFT                ) compat_alGetProcAddress ("alSourcei64SOFT");
        alSource3i64SOFT              = (LPALSOURCE3I64SOFT               ) compat_alGetProcAddress ("alSource3i64SOFT");
        alSourcei64vSOFT              = (LPALSOURCEI64VSOFT               ) compat_alGetProcAddress ("alSourcei64vSOFT");
        alGetSourcei64SOFT            = (LPALGETSOURCEI64SOFT             ) compat_alGetProcAddress ("alGetSourcei64SOFT");
        alGetSource3i64SOFT           = (LPALGETSOURCE3I64SOFT            ) compat_alGetProcAddress ("alGetSource3i64SOFT");
        alGetSourcei64vSOFT           = (LPALGETSOURCEI64VSOFT            ) compat_alGetProcAddress ("alGetSourcei64vSOFT");
        //AL_SOFT_deferred_updates
        alDeferUpdatesSOFT            = (LPALDEFERUPDATESSOFT             ) compat_alGetProcAddress ("alDeferUpdatesSOFT");
        alProcessUpdatesSOFT          = (LPALPROCESSUPDATESSOFT           ) compat_alGetProcAddress ("alProcessUpdatesSOFT");
        //AL_SOFT_source_resampler
        alGetStringiSOFT              = (LPALGETSTRINGISOFT               ) compat_alGetProcAddress ("alGetStringiSOFT");
        //AL_SOFT_events
        alEventControlSOFT            = (LPALEVENTCONTROLSOFT             ) compat_alGetProcAddress ("alEventControlSOFT");
        alEventCallbackSOFT           = (LPALEVENTCALLBACKSOFT            ) compat_alGetProcAddress ("alEventCallbackSOFT");
        alGetPointerSOFT              = (LPALGETPOINTERSOFT               ) compat_alGetProcAddress ("alGetPointerSOFT");
        alGetPointervSOFT             = (LPALGETPOINTERVSOFT              ) compat_alGetProcAddress ("alGetPointervSOFT");
        //AL_SOFT_callback_buffer
        alBufferCallbackSOFT          = (LPALBUFFERCALLBACKSOFT           ) compat_alGetProcAddress ("alBufferCallbackSOFT");
        alGetBufferPtrSOFT            = (LPALGETBUFFERPTRSOFT             ) compat_alGetProcAddress ("alGetBufferPtrSOFT");
        alGetBuffer3PtrSOFT           = (LPALGETBUFFER3PTRSOFT            ) compat_alGetProcAddress ("alGetBuffer3PtrSOFT");
        alGetBufferPtrvSOFT           = (LPALGETBUFFERPTRVSOFT            ) compat_alGetProcAddress ("alGetBufferPtrvSOFT");
}

void _alad_load_alc_functions_contextfree_dlsym (void *module) {
        if (module == NULL) return;
        //Core ALC
        alcGetProcAddress     = (LPALCGETPROCADDRESS              ) _alad_load (module, "alcGetProcAddress");
        alcCreateContext      = (LPALCCREATECONTEXT               ) _alad_load (module, "alcCreateContext");
        alcMakeContextCurrent = (LPALCMAKECONTEXTCURRENT          ) _alad_load (module, "alcMakeContextCurrent");
        alcProcessContext     = (LPALCPROCESSCONTEXT              ) _alad_load (module, "alcProcessContext");
        alcSuspendContext     = (LPALCSUSPENDCONTEXT              ) _alad_load (module, "alcSuspendContext");
        alcDestroyContext     = (LPALCDESTROYCONTEXT              ) _alad_load (module, "alcDestroyContext");
        alcGetCurrentContext  = (LPALCGETCURRENTCONTEXT           ) _alad_load (module, "alcGetCurrentContext");
        alcGetContextsDevice  = (LPALCGETCONTEXTSDEVICE           ) _alad_load (module, "alcGetContextsDevice");
        alcOpenDevice         = (LPALCOPENDEVICE                  ) _alad_load (module, "alcOpenDevice");
        alcCloseDevice        = (LPALCCLOSEDEVICE                 ) _alad_load (module, "alcCloseDevice");
        alcGetError           = (LPALCGETERROR                    ) _alad_load (module, "alcGetError");
        alcIsExtensionPresent = (LPALCISEXTENSIONPRESENT          ) _alad_load (module, "alcIsExtensionPresent");
        alcGetEnumValue       = (LPALCGETENUMVALUE                ) _alad_load (module, "alcGetEnumValue");
        alcGetString          = (LPALCGETSTRING                   ) _alad_load (module, "alcGetString");
        alcGetIntegerv        = (LPALCGETINTEGERV                 ) _alad_load (module, "alcGetIntegerv");
        alcCaptureOpenDevice  = (LPALCCAPTUREOPENDEVICE           ) _alad_load (module, "alcCaptureOpenDevice");
        alcCaptureCloseDevice = (LPALCCAPTURECLOSEDEVICE          ) _alad_load (module, "alcCaptureCloseDevice");
        alcCaptureStart       = (LPALCCAPTURESTART                ) _alad_load (module, "alcCaptureStart");
        alcCaptureStop        = (LPALCCAPTURESTOP                 ) _alad_load (module, "alcCaptureStop");
        alcCaptureSamples     = (LPALCCAPTURESAMPLES              ) _alad_load (module, "alcCaptureSamples");
}


void _alad_load_alc_functions_from_al () {
        if (alGetProcAddress == NULL) return;
        _ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS compat_alGetProcAddress = (_ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS) alGetProcAddress;
        //Only load function loader once, and only if not already loaded from shared library
        if (alcGetProcAddress == NULL) {
                alcGetProcAddress = (LPALCGETPROCADDRESS              ) compat_alGetProcAddress ("alcGetProcAddress");
        }
        //Core ALC
        alcCreateContext      = (LPALCCREATECONTEXT               ) compat_alGetProcAddress ("alcCreateContext");
        alcMakeContextCurrent = (LPALCMAKECONTEXTCURRENT          ) compat_alGetProcAddress ("alcMakeContextCurrent");
        alcProcessContext     = (LPALCPROCESSCONTEXT              ) compat_alGetProcAddress ("alcProcessContext");
        alcSuspendContext     = (LPALCSUSPENDCONTEXT              ) compat_alGetProcAddress ("alcSuspendContext");
        alcDestroyContext     = (LPALCDESTROYCONTEXT              ) compat_alGetProcAddress ("alcDestroyContext");
        alcGetCurrentContext  = (LPALCGETCURRENTCONTEXT           ) compat_alGetProcAddress ("alcGetCurrentContext");
        alcGetContextsDevice  = (LPALCGETCONTEXTSDEVICE           ) compat_alGetProcAddress ("alcGetContextsDevice");
        alcOpenDevice         = (LPALCOPENDEVICE                  ) compat_alGetProcAddress ("alcOpenDevice");
        alcCloseDevice        = (LPALCCLOSEDEVICE                 ) compat_alGetProcAddress ("alcCloseDevice");
        alcGetError           = (LPALCGETERROR                    ) compat_alGetProcAddress ("alcGetError");
        alcIsExtensionPresent = (LPALCISEXTENSIONPRESENT          ) compat_alGetProcAddress ("alcIsExtensionPresent");
        alcGetEnumValue       = (LPALCGETENUMVALUE                ) compat_alGetProcAddress ("alcGetEnumValue");
        alcGetString          = (LPALCGETSTRING                   ) compat_alGetProcAddress ("alcGetString");
        alcGetIntegerv        = (LPALCGETINTEGERV                 ) compat_alGetProcAddress ("alcGetIntegerv");
        alcCaptureOpenDevice  = (LPALCCAPTUREOPENDEVICE           ) compat_alGetProcAddress ("alcCaptureOpenDevice");
        alcCaptureCloseDevice = (LPALCCAPTURECLOSEDEVICE          ) compat_alGetProcAddress ("alcCaptureCloseDevice");
        alcCaptureStart       = (LPALCCAPTURESTART                ) compat_alGetProcAddress ("alcCaptureStart");
        alcCaptureStop        = (LPALCCAPTURESTOP                 ) compat_alGetProcAddress ("alcCaptureStop");
        alcCaptureSamples     = (LPALCCAPTURESAMPLES              ) compat_alGetProcAddress ("alcCaptureSamples");
}


void _alad_load_alc_extension_functions_from_al () {
        if (alGetProcAddress == NULL) return;
        _ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS compat_alGetProcAddress = (_ALAD_ISO_C_COMPAT_LPALGETPROCADDRESS) alGetProcAddress;
        //ALC extensions
        //ALC_EXT_thread_local_context
        alcSetThreadContext            = (PFNALCSETTHREADCONTEXTPROC       ) compat_alGetProcAddress ("alcSetThreadContext");
        alcGetThreadContext            = (PFNALCGETTHREADCONTEXTPROC       ) compat_alGetProcAddress ("alcGetThreadContext");
        //ALC_SOFT_loopback
        alcLoopbackOpenDeviceSOFT      = (LPALCLOOPBACKOPENDEVICESOFT      ) compat_alGetProcAddress ("alcLoopbackOpenDeviceSOFT");
        alcIsRenderFormatSupportedSOFT = (LPALCISRENDERFORMATSUPPORTEDSOFT ) compat_alGetProcAddress ("alcIsRenderFormatSupportedSOFT");
        alcRenderSamplesSOFT           = (LPALCRENDERSAMPLESSOFT           ) compat_alGetProcAddress ("alcRenderSamplesSOFT");
        //ALC_SOFT_pause_device
        alcDevicePauseSOFT             = (LPALCDEVICEPAUSESOFT             ) compat_alGetProcAddress ("alcDevicePauseSOFT");
        alcDeviceResumeSOFT            = (LPALCDEVICERESUMESOFT            ) compat_alGetProcAddress ("alcDeviceResumeSOFT");
        //ALC_SOFT_HRTF
        alcGetStringiSOFT              = (LPALCGETSTRINGISOFT              ) compat_alGetProcAddress ("alcGetStringiSOFT");
        alcResetDeviceSOFT             = (LPALCRESETDEVICESOFT             ) compat_alGetProcAddress ("alcResetDeviceSOFT");
        //ALC_SOFT_device_clock
        alcGetInteger64vSOFT           = (LPALCGETINTEGER64VSOFT           ) compat_alGetProcAddress ("alcGetInteger64vSOFT");
        //ALC_SOFT_reopen_device
        alcReopenDeviceSOFT            = (LPALCREOPENDEVICESOFT            ) compat_alGetProcAddress ("alcReopenDeviceSOFT");
}

void _alad_load_alc_functions (ALCdevice *device) {
        //old loader code left for future use
        /*
    if((alGetProcAddress == NULL) && (alcGetProcAddress == NULL)) return;
    if(alcGetProcAddress == NULL) {
        alcGetProcAddress = (LPALCGETPROCADDRESS              ) alGetProcAddress("alcGetProcAddress");
    }
    */
        if (alcGetProcAddress == NULL) return;
        _ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS compat_alcGetProcAddress = (_ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS) alcGetProcAddress;
        //Core ALC
        alcCreateContext      = (LPALCCREATECONTEXT               ) compat_alcGetProcAddress (device, "alcCreateContext");
        alcMakeContextCurrent = (LPALCMAKECONTEXTCURRENT          ) compat_alcGetProcAddress (device, "alcMakeContextCurrent");
        alcProcessContext     = (LPALCPROCESSCONTEXT              ) compat_alcGetProcAddress (device, "alcProcessContext");
        alcSuspendContext     = (LPALCSUSPENDCONTEXT              ) compat_alcGetProcAddress (device, "alcSuspendContext");
        alcDestroyContext     = (LPALCDESTROYCONTEXT              ) compat_alcGetProcAddress (device, "alcDestroyContext");
        alcGetCurrentContext  = (LPALCGETCURRENTCONTEXT           ) compat_alcGetProcAddress (device, "alcGetCurrentContext");
        alcGetContextsDevice  = (LPALCGETCONTEXTSDEVICE           ) compat_alcGetProcAddress (device, "alcGetContextsDevice");
        alcOpenDevice         = (LPALCOPENDEVICE                  ) compat_alcGetProcAddress (device, "alcOpenDevice");
        alcCloseDevice        = (LPALCCLOSEDEVICE                 ) compat_alcGetProcAddress (device, "alcCloseDevice");
        alcGetError           = (LPALCGETERROR                    ) compat_alcGetProcAddress (device, "alcGetError");
        alcIsExtensionPresent = (LPALCISEXTENSIONPRESENT          ) compat_alcGetProcAddress (device, "alcIsExtensionPresent");
        alcGetEnumValue       = (LPALCGETENUMVALUE                ) compat_alcGetProcAddress (device, "alcGetEnumValue");
        alcGetString          = (LPALCGETSTRING                   ) compat_alcGetProcAddress (device, "alcGetString");
        alcGetIntegerv        = (LPALCGETINTEGERV                 ) compat_alcGetProcAddress (device, "alcGetIntegerv");
        alcCaptureOpenDevice  = (LPALCCAPTUREOPENDEVICE           ) compat_alcGetProcAddress (device, "alcCaptureOpenDevice");
        alcCaptureCloseDevice = (LPALCCAPTURECLOSEDEVICE          ) compat_alcGetProcAddress (device, "alcCaptureCloseDevice");
        alcCaptureStart       = (LPALCCAPTURESTART                ) compat_alcGetProcAddress (device, "alcCaptureStart");
        alcCaptureStop        = (LPALCCAPTURESTOP                 ) compat_alcGetProcAddress (device, "alcCaptureStop");
        alcCaptureSamples     = (LPALCCAPTURESAMPLES              ) compat_alcGetProcAddress (device, "alcCaptureSamples");
}

void _alad_load_alc_extension_functions (ALCdevice *device) {
        if (alcGetProcAddress == NULL) return;
        _ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS compat_alcGetProcAddress = (_ALAD_ISO_C_COMPAT_LPALCGETPROCADDRESS) alcGetProcAddress;
        //ALC extensions
        //ALC_EXT_thread_local_context
        alcSetThreadContext       = (PFNALCSETTHREADCONTEXTPROC       ) compat_alcGetProcAddress (device, "alcSetThreadContext");
        alcGetThreadContext       = (PFNALCGETTHREADCONTEXTPROC       ) compat_alcGetProcAddress (device, "alcGetThreadContext");
        //ALC_SOFT_loopback
        alcLoopbackOpenDeviceSOFT      = (LPALCLOOPBACKOPENDEVICESOFT      ) compat_alcGetProcAddress (device, "alcLoopbackOpenDeviceSOFT");
        alcIsRenderFormatSupportedSOFT = (LPALCISRENDERFORMATSUPPORTEDSOFT ) compat_alcGetProcAddress (device, "alcIsRenderFormatSupportedSOFT");
        alcRenderSamplesSOFT           = (LPALCRENDERSAMPLESSOFT           ) compat_alcGetProcAddress (device, "alcRenderSamplesSOFT");
        //ALC_SOFT_pause_device
        alcDevicePauseSOFT   = (LPALCDEVICEPAUSESOFT             ) compat_alcGetProcAddress (device, "alcDevicePauseSOFT");
        alcDeviceResumeSOFT  = (LPALCDEVICERESUMESOFT            ) compat_alcGetProcAddress (device, "alcDeviceResumeSOFT");
        //ALC_SOFT_HRTF
        alcGetStringiSOFT    = (LPALCGETSTRINGISOFT              ) compat_alcGetProcAddress (device, "alcGetStringiSOFT");
        alcResetDeviceSOFT   = (LPALCRESETDEVICESOFT             ) compat_alcGetProcAddress (device, "alcResetDeviceSOFT");
        //ALC_SOFT_device_clock
        alcGetInteger64vSOFT = (LPALCGETINTEGER64VSOFT           ) compat_alcGetProcAddress (device, "alcGetInteger64vSOFT");
        //ALC_SOFT_reopen_device
        alcReopenDeviceSOFT  = (LPALCREOPENDEVICESOFT            ) compat_alcGetProcAddress (device, "alcReopenDeviceSOFT");
}



//modelled after GLFW, see win32_module.c and posix_module.c specifically
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__)
#define NOMINMAX
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
void *_alad_open (const char *path) {
        return LoadLibraryA (path);
}
_alad_proc _alad_load (void *module, const char *name) {
        return (_alad_proc) GetProcAddress ((HMODULE) module, name);
}
void _alad_close (void *module) {
        FreeLibrary ((HMODULE) module);
}
#define _alad_LIB_NAME           "OpenAL32.dll"
#define _alad_SECONDARY_LIB_NAME "soft_oal.dll"
#else /* Unix defaults otherwise */
#include <dlfcn.h>
void *_alad_open (const char *path) {
        return dlopen (path, RTLD_LAZY | RTLD_LOCAL);
}
_alad_proc _alad_load (void *module, const char *name) {
        return dlsym (module, name);
}
void _alad_close (void *module) {
        dlclose (module);
}

//there are also libopenal.so.1.[X].[Y] and libopenal.1.[X].[Y].dylib respectively, but it would be difficult to look all of those up
#if defined(__APPLE__)
//not tested myself; the only references I could find are https://github.com/ToweOPrO/sadsad and https://pastebin.com/MEmh3ZFr, which is at least tenuous
#define _alad_LIB_NAME           "libopenal.1.dylib"
#define _alad_SECONDARY_LIB_NAME "libopenal.dylib"
#else
#define _alad_LIB_NAME           "libopenal.so.1"
#define _alad_SECONDARY_LIB_NAME "libopenal.so"
#endif

#endif /* _WIN32 */



void *_alad_module = NULL;

void  _alad_load_lib () {
        //don't load shared object twice
        if (_alad_module != NULL) return;
        //use fallback so name
        _alad_module = _alad_open (_alad_LIB_NAME);
        if (_alad_module == NULL) {
                _alad_module = _alad_open (_alad_SECONDARY_LIB_NAME);
        }
}

LPALGETPROCADDRESS _alad_initalloader () {
        _alad_load_lib();
        if (_alad_module == NULL) {
                return NULL;
        }
        return (LPALGETPROCADDRESS) _alad_load (_alad_module, "alGetProcAddress");
}

void _alad_unload_lib () {
        if (_alad_module != NULL) _alad_close (_alad_module);
}



//manual interface
void aladLoadALContextFree (ALboolean loadAll) {
        _alad_load_lib();
        _alad_load_al_functions_contextfree_dlsym (_alad_module, loadAll);
        _alad_load_alc_functions_contextfree_dlsym (_alad_module);
}

void aladLoadALFromLoaderFunction (LPALGETPROCADDRESS inital_loader) {
        if (inital_loader != NULL) {
                alGetProcAddress = inital_loader;
        } else if (alGetProcAddress == NULL) {
                alGetProcAddress = _alad_initalloader();
        }
        _alad_load_al_functions();
        _alad_load_alc_functions_from_al();
}


void aladUpdateALPointers (ALCcontext *context, ALboolean extensionsOnly) {
        ALboolean   switchContext = (context != NULL);
        ALCcontext *oldContext;
        if (switchContext) {
                oldContext = alcGetCurrentContext();
                alcMakeContextCurrent (context);
        }
        if (extensionsOnly == AL_FALSE) {
                _alad_load_al_functions();
        }
        _alad_load_al_extension_functions();
        if (switchContext) {
                alcMakeContextCurrent (oldContext);
        }
}

void aladUpdateALCPointersFromContext (ALCcontext *context, ALboolean extensionsOnly) {
        ALboolean   switchContext = (context != NULL);
        ALCcontext *oldContext;
        if (switchContext) {
                oldContext = alcGetCurrentContext();
                alcMakeContextCurrent (context);
        }
        if (extensionsOnly == AL_FALSE) {
                _alad_load_alc_functions_from_al();
        }
        _alad_load_alc_extension_functions_from_al();
        if (switchContext) {
                alcMakeContextCurrent (oldContext);
        }
}

void aladUpdateALCPointersFromDevice (ALCdevice *device, ALboolean extensionsOnly) {
        if (extensionsOnly == AL_FALSE) {
                _alad_load_alc_functions (device);
        }
        _alad_load_alc_extension_functions (device);
}

void aladTerminate () {
        _alad_unload_lib();
}



//simplified Interface
void aladLoadAL () {
        aladLoadALContextFree (AL_TRUE);
}
void aladUpdateAL () {
        //load extensions with alGetProcAddress
        _alad_load_al_extension_functions();
        //use current contexts device to load ALC extensions with alcGetProcAddress
        ALCdevice *device = alcGetContextsDevice (alcGetCurrentContext());
        _alad_load_alc_extension_functions (device);
}


#else        // that is when !defined(ALAD_IMPLEMENTATION)

//simplified Interface
extern void aladLoadAL();
extern void aladUpdateAL();

//manual interface
extern void aladLoadALContextFree (ALboolean loadAll);
extern void aladLoadALFromLoaderFunction (LPALGETPROCADDRESS inital_loader);
extern void aladUpdateALPointers (ALCcontext *context, ALboolean extensionsOnly);
extern void aladUpdateALCPointersFromContext (ALCcontext *context, ALboolean extensionsOnly);
extern void aladUpdateALCPointersFromDevice (ALCdevice *device, ALboolean extensionsOnly);

//shared function
extern void aladTerminate();

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

#endif        // ALAD_IMPLEMENTATION

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif        // ALAD_H
