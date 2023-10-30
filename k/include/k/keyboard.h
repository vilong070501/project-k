#ifndef KEYBOARD_H
#define KEYHOARD_H

#define KEYBOARD_DATA_PORT      0x60
#define KEYBOARD_STATUS_PORT    0x64
#define KEYBOARD_COMMAND_PORT   0x64

void keyboard_init(void);
int getKey(void);
int keyboard_getScancode(void);
char keyboard_getChar(void);

#endif