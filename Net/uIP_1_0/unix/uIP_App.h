#ifndef UIP_APP_H
#define UIP_APP_H
//
// Macros definitions
//

//
// Exported functions declare
//
void eth_poll(void);
void UipPro(void);
void uIP_Net_Init(void);

extern void (*p_appcall)(void);
//
// Exported varialbes declare
//

#endif
