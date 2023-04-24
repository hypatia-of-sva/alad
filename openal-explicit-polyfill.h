#include "alad.h"

#ifndef OPENAL_EXPLICIT_POLYFILL
#define OPENAL_EXPLICIT_POLYFILL


/* to separate listener- and context-dependency */
typedef ALlistener *ALCdevice;



/*
 * Logic by which device, context or listener is added or not:
 * a) any function taking in a buffer id needs not to take a device, but might need a listener or context
 * b) any function taking in a source id needs not to take a device or context, but might need a listener
 * c) any function takinn a context needs not to take a device
 * c) any function takinn a listener needs not to take a device or context
 *    since they carry information about them by the creation of the id / context.
 * This is not true for an array of such objects (as e.g. in alSourcePlayv) since adding the dependency
 * information explicitly means it need not be checked if they are the same across all the elements of the array.
 * When Allistener and when ALCcontext* should be used, however, is not quite clear since it is currently the same
 * The idea behind this is to potentially allow multiple listeners operating on the same sources.
 * Therefore, the functions changing the listener state (like its position and direction) should take Allistener,
 * while the functions changing global context state (like Enable or DistanceModel) should take ALCcontext*.
 */


/* Function prototypes for added device references: */
void            alGenBuffersExplicitEXT         (ALCdevice* device, ALsizei n, ALuint *buffers);
void            alDeleteBuffersExplicitEXT      (ALCdevice* device, ALsizei n, const ALuint *buffers);
ALboolean       alIsBufferExplicitEXT           (ALCdevice* device, ALuint buffer);


/* Function prototypes for added context references: */
void            alDopplerFactorExplicitEXT      (ALCcontext* context, ALfloat value);
void            alDopplerVelocityExplicitEXT    (ALCcontext* context, ALfloat value);
void            alSpeedOfSoundExplicitEXT       (ALCcontext* context, ALfloat value);
void            alDistanceModelExplicitEXT      (ALCcontext* context, ALenum distanceModel);
void            alEnableExplicitEXT             (ALCcontext* context, ALenum capability);
void            alDisableExplicitEXT            (ALCcontext* context, ALenum capability);
ALboolean       alIsEnabledExplicitEXT          (ALCcontext* context, ALenum capability);
const ALchar*   alGetStringExplicitEXT          (ALCcontext* context, ALenum param);
void            alGetBooleanvExplicitEXT        (ALCcontext* context, ALenum param, ALboolean *values);
void            alGetIntegervExplicitEXT        (ALCcontext* context, ALenum param, ALint *values);
void            alGetFloatvExplicitEXT          (ALCcontext* context, ALenum param, ALfloat *values);
void            alGetDoublevExplicitEXT         (ALCcontext* context, ALenum param, ALdouble *values);
ALboolean       alGetBooleanExplicitEXT         (ALCcontext* context, ALenum param);
ALint           alGetIntegerExplicitEXT         (ALCcontext* context, ALenum param);
ALfloat         alGetFloatExplicitEXT           (ALCcontext* context, ALenum param);
ALdouble        alGetDoubleExplicitEXT          (ALCcontext* context, ALenum param);
ALenum          alGetErrorExplicitEXT           (ALCcontext* context, void);
ALboolean       alIsExtensionPresentExplicitEXT (ALCcontext* context, const ALchar *extname);
void*           alGetProcAddressExplicitEXT     (ALCcontext* context, const ALchar *fname);
ALenum          alGetEnumValueExplicitEXT       (ALCcontext* context, const ALchar *ename);


/* Function prototypes for added listener references: */

void            alListenerfExplicitEXT          (ALlistener listener, ALenum param, ALfloat value);
void            alListener3fExplicitEXT         (ALlistener listener, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
void            alListenerfvExplicitEXT         (ALlistener listener, ALenum param, const ALfloat *values);
void            alListeneriExplicitEXT          (ALlistener listener, ALenum param, ALint value);
void            alListener3iExplicitEXT         (ALlistener listener, ALenum param, ALint value1, ALint value2, ALint value3);
void            alListenerivExplicitEXT         (ALlistener listener, ALenum param, const ALint *values);
void            alGetListenerfExplicitEXT       (ALlistener listener, ALenum param, ALfloat *value);
void            alGetListener3fExplicitEXT      (ALlistener listener, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
void            alGetListenerfvExplicitEXT      (ALlistener listener, ALenum param, ALfloat *values);
void            alGetListeneriExplicitEXT       (ALlistener listener, ALenum param, ALint *value);
void            alGetListener3iExplicitEXT      (ALlistener listener, ALenum param, ALint *value1, ALint *value2, ALint *value3);
void            alGetListenerivExplicitEXT      (ALlistener listener, ALenum param, ALint *values);
void            alGenSourcesExplicitEXT         (ALlistener listener, ALsizei n, ALuint *sources);
void            alDeleteSourcesExplicitEXT      (ALlistener listener, ALsizei n, const ALuint *sources);
ALboolean       alIsSourceExplicitEXT           (ALlistener listener, ALuint source);
void            alSourcePlayvExplicitEXT        (ALlistener listener, ALsizei n, const ALuint *sources);
void            alSourceStopvExplicitEXT        (ALlistener listener, ALsizei n, const ALuint *sources);
void            alSourceRewindvExplicitEXT      (ALlistener listener, ALsizei n, const ALuint *sources);
void            alSourcePausevExplicitEXT       (ALlistener listener, ALsizei n, const ALuint *sources);





/* New functions that currently do basically nothing, but could be expanded upon */
ALlistener      alCreateListenerEXT             (ALCcontext* context);
ALboolean       alIsListenerinContextEXT        (ALCcontext* context, ALlistener listener);
void            alDestroyListenerEXT            (ALCcontext* context, ALlistener listener);



#ifdef OPENAL_EXPLICIT_POLYFILL_IMPLEMENTATION


#define ALEXP_DEVICE_WRAP(device, code)                                     \
            do {                                                            \
                ALCcontext* oldContext, helperContext;                      \
                ALCdevice*  oldDevice;                                      \
                                                                            \
                oldContext = alcGetCurrentContext();                        \
                oldDevice = alcGetContextsDevice(oldContext);               \
                if(oldDevice != device) {                                   \
                    helperContext = alcCreateContext(device, NULL);         \
                    alcMakeContextCurrent(helperContext);                   \
                }                                                           \
                                                                            \
                (code);                                                     \
                                                                            \
                if(oldDevice != device) {                                   \
                    alcMakeContextCurrent(oldContext);                      \
                    alcDestroyContext(helperContext);                       \
                }                                                           \
            } while(0);                                                     
            

#define ALEXP_CONTEXT_WRAP(context, code)                                   \
            do {                                                            \
              ALCcontext* oldContext;                                       \
                                                                            \
              oldContext = alcGetCurrentContext();                          \
              if(oldContext != context) {                                   \
                  alcMakeContextCurrent(context);                           \
              }                                                             \
                                                                            \
              (code);                                                       \
                                                                            \
              if(oldContext != context) {                                   \
                  alcMakeContextCurrent(oldContext);                        \
              }                                                             \
            } while(0);                                                     

/* defined to be the same since there is currently no difference in implementation */
#define ALEXP_LISTENER_WRAP(listener, code                                  \
            ALEXP_CONTEXT_WRAP(listener, code)


void            alGenBuffersExplicitEXT         (ALCdevice* device, ALsizei n, ALuint *buffers) {
    ALEXP_DEVICE_WRAP(device, (alGenBuffers(n, buffers)));
}
void            alDeleteBuffersExplicitEXT      (ALCdevice* device, ALsizei n, const ALuint *buffers) {
    ALEXP_DEVICE_WRAP(device, (alDeleteBuffers(n, buffers)));
}
ALboolean       alIsBufferExplicitEXT           (ALCdevice* device, ALuint buffer) {
    ALboolean isBuffer;
    ALEXP_DEVICE_WRAP(device, (isBuffer = alIsBuffer(buffer)));
    return isBuffer;
}




void            alDopplerFactorExplicitEXT      (ALCcontext* context, ALfloat value) {
    ALEXP_CONTEXT_WRAP(context, (alDopplerFactor(value)))
}
void            alDopplerVelocityExplicitEXT    (ALCcontext* context, ALfloat value) {
    ALEXP_CONTEXT_WRAP(context, (alDopplerVelocity(value)))
}
void            alSpeedOfSoundExplicitEXT       (ALCcontext* context, ALfloat value) {
    ALEXP_CONTEXT_WRAP(context, (alSpeedOfSound(value)))
}
void            alDistanceModelExplicitEXT      (ALCcontext* context, ALenum distanceModel) {
    ALEXP_CONTEXT_WRAP(context, (alDistanceModel(distanceModel)))
}
void            alEnableExplicitEXT             (ALCcontext* context, ALenum capability) {
    ALEXP_CONTEXT_WRAP(context, (alEnable(capability)))
}
void            alDisableExplicitEXT            (ALCcontext* context, ALenum capability) {
    ALEXP_CONTEXT_WRAP(context, (alDisable(capability)))
}
ALboolean       alIsEnabledExplicitEXT          (ALCcontext* context, ALenum capability) {
    ALboolean isEnabled;
    ALEXP_CONTEXT_WRAP(context, (isEnabled = alIsEnabled(capability)))
    return isEnabled;
}
const ALchar*   alGetStringExplicitEXT          (ALCcontext* context, ALenum param) {
    const ALchar* str;
    ALEXP_CONTEXT_WRAP(context, (str = alGetString(param)))
    return str;
}
void            alGetBooleanvExplicitEXT        (ALCcontext* context, ALenum param, ALboolean *values) {
    ALEXP_CONTEXT_WRAP(context, (alGetBooleanv(param, values)))
}
void            alGetIntegervExplicitEXT        (ALCcontext* context, ALenum param, ALint *values) {
    ALEXP_CONTEXT_WRAP(context, (alGetIntegerv(param, values)))
}
void            alGetFloatvExplicitEXT          (ALCcontext* context, ALenum param, ALfloat *values) {
    ALEXP_CONTEXT_WRAP(context, (alGetFloatv(param,values)))
}
void            alGetDoublevExplicitEXT         (ALCcontext* context, ALenum param, ALdouble *values) {
    ALEXP_CONTEXT_WRAP(context, (alGetDoublev(param, values)))
}
ALboolean       alGetBooleanExplicitEXT         (ALCcontext* context, ALenum param) {
    ALboolean b;
    ALEXP_CONTEXT_WRAP(context, (b = alGetBoolean(param)))
    return b;
}
ALint           alGetIntegerExplicitEXT         (ALCcontext* context, ALenum param) {
    ALint i;
    ALEXP_CONTEXT_WRAP(context, (i = alGetInteger(param)))
    return i;
}
ALfloat         alGetFloatExplicitEXT           (ALCcontext* context, ALenum param) {
    ALfloat f;
    ALEXP_CONTEXT_WRAP(context, (f = alGetFloat(param)))
    return f;
}
ALdouble        alGetDoubleExplicitEXT          (ALCcontext* context, ALenum param) {
    ALdouble d;
    ALEXP_CONTEXT_WRAP(context, (d = alGetDouble(param)))
    return d;
}
ALenum          alGetErrorExplicitEXT           (ALCcontext* context, void) {
    ALenum e;
    ALEXP_CONTEXT_WRAP(context, (e = alGetError()))
    return e;
}
ALboolean       alIsExtensionPresentExplicitEXT (ALCcontext* context, const ALchar *extname) {
    ALboolean isPresent;
    ALEXP_CONTEXT_WRAP(context, (isPresent = alIsExtensionPresent(extname)))
    return isPresent;
}
void*           alGetProcAddressExplicitEXT     (ALCcontext* context, const ALchar *fname) {
    void *pfn;
    ALEXP_CONTEXT_WRAP(context, (pfn =  alGetProcAddress(fname)))
    return pfn;
}
ALenum          alGetEnumValueExplicitEXT       (ALCcontext* context, const ALchar *ename) {
    ALenum e;
    ALEXP_CONTEXT_WRAP(context, (e = alGetEnumValue(ename)))
    return e;
}










ALlistener      alCreateListenerEXT             (ALCcontext* context) {
  return context;
}
ALboolean       alsIListenerinContextEXT        (ALCcontext* context, ALlistener listener) {
  return context == listener;
}
void            alDestroyListenerEXT            (ALCcontext* context, ALlistener listener) {
        /* currently not needed, but could perform this check among other things */
   /* assert(alIsListenerinContextEXT(context,listener)); */ 
   (void)0;
}

#endif /* OPENAL_EXPLICIT_POLYFILL_IMPLEMENTATION */

#endif /* OPENAL_EXPLICIT_POLYFILL */
