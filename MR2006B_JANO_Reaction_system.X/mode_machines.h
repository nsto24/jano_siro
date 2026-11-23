/* 
 * File:                mode_machines.h
 * Author:              Nicolas Stock
 * Comments:
 * Revision history:    31.08.2026 16:59 - Created file
 */

#ifndef MODE_MACHINES_H
#define	MODE_MACHINES_H

void state_machine_menu(void);
void state_machine_synchronous(void);
void state_machine_holdrelease(void);
void state_machine_random(void);
void state_machine_vehiclecontrol(void);

void reset_mode_random(void);
void reset_mode_synchronous(void);
void reset_mode_holdrelease(void);
void reset_mode_vehiclecontrol(void);

#endif	/* MODE_MACHINES_H */

