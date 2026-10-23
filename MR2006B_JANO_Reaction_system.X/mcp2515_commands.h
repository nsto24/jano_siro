/* 
 * File:                mcp2515_commands.h
 * Author:              Nicolas Stock
 * Comments:
 * Revision history:    05.10.2026 15:05 - Created file
 */

#ifndef MCP2515_COMMANDS_H
#define	MCP2515_COMMANDS_H

// Commands

#define RESET                   0xC0
#define WRITE                   0x02
#define READ                    0x03
#define READ_RX_BUFFER_0SIDH    0x90    // 10010xx0 : xx=00
#define READ_RX_BUFFER_0D0      0x92    // 10010xx0 : xx=01
#define READ_RX_BUFFER_1SIDH    0x94    // 10010xx0 : xx=10
#define READ_RX_BUFFER_1D0      0x96    // 10010xx0 : xx=11
#define LOAD_TX_BUFFER_0SIDH    0x40    // 01000xxx : 000
#define LOAD_TX_BUFFER_0D0      0x42    // 01000xxx : 001
#define LOAD_TX_BUFFER_1SIDH    0x44    // 01000xxx : 010
#define LOAD_TX_BUFFER_1D0      0x46    // 01000xxx : 011
#define LOAD_TX_BUFFER_2SIDH    0x48    // 01000xxx : 100
#define LOAD_TX_BUFFER_2D0      0x4A    // 01000xxx : 101
#define MESSAGE_RTS_TXB0        0x81
#define MESSAGE_RTS_TXB1        0x82
#define MESSAGE_RTS_TXB2        0x84
#define READ_STATUS             0xA0
#define RX_STATUS               0xB0
#define BIT_MODIFY              0x05

// Registers

#define MCP_REGISTER_CNF3       0x28
#define MCP_REGISTER_CNF2       0x29
#define MCP_REGISTER_CNF1       0x2A
#define MCP_REGISTER_CANINTE    0x2B
#define MCP_REGISTER_CANINTF    0x2C
#define MCP_REGISTER_CANCTRL    0x0F

#endif	/* MCP2515_COMMANDS_H */