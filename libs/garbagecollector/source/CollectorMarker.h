
// metadoc CollectorMarker copyright Steve Dekorte 2002
// metadoc CollectorMarker license BSD revised

#ifndef COLLECTORMARKER_DEFINED
#define COLLECTORMARKER_DEFINED 1

#include "Common.h"
#include "List.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(WIN32)
#if defined(BUILDING_COLLECTOR_DLL) || defined(BUILDING_IOVMALL_DLL)
#define COLLECTOR_API __declspec(dllexport)
#else
#define COLLECTOR_API __declspec(dllimport)
#endif

#else
#define COLLECTOR_API
#endif

typedef void(CollectorDoFunc)(void *);

typedef struct CollectorMarker CollectorMarker;

#define CollectorMarkerSansPointer                                             \
    CollectorMarker *prev;                                                     \
    CollectorMarker *next;                                                     \
    unsigned int color : 2;                                                    \
    unsigned int hash1;                                                        \
    unsigned int hash2;

/*
#if !defined(COLLECTOROBJECTTYPE)
#define COLLECTOROBJECTTYPE void
#endif
*/

typedef struct IoObjectData IoObjectData;
#define IoObjectDataDefined 1

struct CollectorMarker {
    CollectorMarkerSansPointer IoObjectData *object;
};

#define COLLECTOR_REF_TYPE(Type)                                               \
    typedef struct {                                                           \
        CollectorMarkerSansPointer;                                            \
        Type *object;                                                          \
    } Type##Ref;

#define CollectorMarker_setObject_(self, v)                                    \
    ((CollectorMarker *)self)->object = v;
#define CollectorMarker_object(self) (((CollectorMarker *)self)->object)

COLLECTOR_API CollectorMarker *CollectorMarker_new(void);

CollectorMarker *CollectorMarker_newWithColor_(unsigned int color);
COLLECTOR_API void CollectorMarker_free(CollectorMarker *self);

void CollectorMarker_loop(CollectorMarker *self);
void CollectorMarker_check(CollectorMarker *self);

// void CollectorMarker_removeAndInsertAfter_(CollectorMarker *self,
// CollectorMarker *other);
void CollectorMarker_removeAndInsertBefore_(CollectorMarker *self,
                                            CollectorMarker *other);
// void CollectorMarker_removeIfNeededAndInsertAfter_(CollectorMarker *self,
// CollectorMarker *other);

// void CollectorMarker_remove(CollectorMarker *self);
int CollectorMarker_count(CollectorMarker *self);
int CollectorMarker_colorSetIsEmpty(CollectorMarker *self);

#define CollectorMarker_setColor_(self, c) ((CollectorMarker *)self)->color = c;
#define CollectorMarker_color(self) ((CollectorMarker *)self)->color;

#define CollectorMarker_num(self) (((CollectorMarker *)self)->num);
#define CollectorMarker_setNext_(self, v) ((CollectorMarker *)self)->next = v;
#define CollectorMarker_setPrev_(self, v) ((CollectorMarker *)self)->prev = v;

#define MARKER(v) ((CollectorMarker *)v)

#define COLLECTMARKER_FOREACH(self, v, code)                                   \
    {                                                                          \
        CollectorMarker *v = self->next;                                       \
        CollectorMarker *_next;                                                \
        unsigned int c = self->color;                                          \
                                                                               \
        while (v->color == c) {                                                \
            _next = v->next;                                                   \
            code;                                                              \
            v = _next;                                                         \
        }                                                                      \
    }

#include "CollectorMarker_inline.h"

#ifdef __cplusplus
}
#endif
#endif
