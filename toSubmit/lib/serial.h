/* Final ECE230 Project: MSP serial.h
Authors: ECE230 example code, Jacob Consolvi, Evelyn Elmer, and Abigail Kurfman
Description: This code sets up and runs a UART communication rptotocal with the ESP. Some code from Ex602 is used, 
and some was modified for the project applications. 
Last Revised: 2/21/23 
*/
#ifndef SERIAL_H_
#define SERIAL_H_
//*****************************************************************************
#ifdef __cplusplus
extern "C" {
#endif

// Constructor for creating a Serial port that links the TX and RX pins of the MSP to the ESP
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
