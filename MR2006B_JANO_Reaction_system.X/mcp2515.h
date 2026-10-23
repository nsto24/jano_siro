/* 
 * File:                mcp2515.h
 * Author:              Nicolas Stock
 * Comments:
 * Revision history:    05.10.2026 18:01 - Created file
 */

#ifndef MCP2515_H
#define	MCP2515_H

void Init_MCP(void);

void MCP_Setup(void);

void MCP_TransmitMessage(unsigned int standard_id, unsigned char *data, unsigned char length);

unsigned char MCP_ReceiveMessage(unsigned int *standard_id, unsigned char *data, unsigned char *length);

void MCP_ClearInterruptFlag(void);

#endif	/* MCP2515_H */

