#ifndef IDE_H
#define IDE_H

#include "types.h"

// https://wiki.osdev.org/PCI_IDE_Controller

struct IDE_Channels {
	u16 base;		// I/O Base
	u16 control; 	// Control Base
	u16 bm_ide;		// Bus Master IDE
	u8	no_int;		// No Interrupt Port
};

struct IDE_Device {
	u8 reserved;		// 0 (Empty) or 1 (This drive really exists)
	u8 channel; 		// 0 (Primary) or 1 (Secondary)
	u8 drive;			// 0 (Master) or 1 (Slave)
	u16 type;			// 0 (ATA) or 1 (ATAPI)
	u16 signature;		// Drive signature
	u16 features;		// Drive features
	u32 command_sets;	// Supported command sets
	u32 size;			// Drive size in sectors
	unsigned char model[41]; 	// Drive name
};

#define MAXIMUM_CHANNELS	2
#define MAXIMUM_IDE_DEVICES	5

/* IDE types */
#define IDE_ATA      0x00
#define IDE_ATAPI    0x01

/* Directions */
#define ATA_READ     0x00
#define ATA_WRITE    0x01

/* Identify bytes */
#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

/* ATA drive error status */
#define ATA_ER_BBK      0x80    // Bad block
#define ATA_ER_UNC      0x40    // Uncorrectable data
#define ATA_ER_MC       0x20    // Media changed
#define ATA_ER_IDNF     0x10    // ID mark not found
#define ATA_ER_MCR      0x08    // Media change request
#define ATA_ER_ABRT     0x04    // Command aborted
#define ATA_ER_TK0NF    0x02    // Track 0 not found
#define ATA_ER_AMNF     0x01    // No address mark

#define ATA_SECTOR_SIZE 512

void init_IDE(u32 primary_channel_base, u32 primary_channel_control_base,
              u32 secondary_channel_base, u32 secondary_channel_control_base,
              u32 bus_master_base);

u8 IDE_ATA_access(u8 direction, u8 drive, u32 lba, u8 nb_sectors, u32 buffer);
void ide_wait_irq(void);

#endif