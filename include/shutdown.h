#ifndef SHUTDOWN_H
#define SHUTDOWN_H

void acpiPowerOff(void);

static inline void shutdown() {acpiPowerOff();}

void reboot();

#endif
