/*
 * File:                hc05.h
 * Author:              Nicolas Stock
 * Comments:            
 * Revision history:    23.10.2026, 19:01
 */

#ifndef HC05_H
#define	HC05_H

void Init_HC05(void);
void HC05_SyncState(void);
unsigned char isBtConnected(void);
unsigned char hasBtStateChanged(void);

#endif	/* HC05_H */
