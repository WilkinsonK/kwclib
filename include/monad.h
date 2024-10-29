#ifndef KWC_MONAD_H
#define KWC_MONAD_H
typedef enum MonadState {
    MS_OK,
    MS_ERR,
} MonadState;
typedef struct Monad {
    void*      value;
    MonadState state;
} Monad;
typedef Monad*(*Monad_Fn)(Monad*);
Monad* Monad_New(void);
Monad* Monad_Init(void*);
Monad* Monad_Map(Monad*, Monad_Fn);
void* Monad_Unwrap(Monad*);
void* Monad_UnwrapUnchecked(Monad*);
#endif
