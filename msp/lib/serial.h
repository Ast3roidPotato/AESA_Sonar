#ifndef SERIAL_H_
#define SERIAL_H_
//*****************************************************************************
#ifdef __cplusplus
extern "C" {
#endif

struct Serial {
    void (*print)(char *string, ...);
    void (*println)(char *string, ...);
    void (*printChar)(char c);
    char (*readChar)(void);
};

extern const struct SerialClass {
    struct Serial (*new)(void);
} Serial;

#ifdef __cplusplus
}
#endif

#endif
