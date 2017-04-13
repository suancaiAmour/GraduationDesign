#ifndef FAT_DIRITEM_H
#define FAT_DIRITEM_H

namespace nstd{
    
	#pragma pack(push)	/**< 保存对齐状态 */
	#pragma pack(1)		/**< 1 字对齐 */
	
	struct DPT_CHS{		// DPT 中的CHS
	protected:
		u8 head;
		leu16	cylinder_sector;
	public:
		finline u8 GetHead()		{ return head; }
		finline u8 GetSector()		{ return ((u16)cylinder_sector )>>2; }
		finline u16 GetCylinder()	{ u16 temp = cylinder_sector; return ((temp<<8)|(temp>>8))&BitFromTo(0,9); }
	};

	struct MbrPartition {

		/// status (0x80 = bootable (active), 0x00 = non-bootable, other = invalid )
		u8 boot_indicator;

		/// CHS address of first absolute sector in partition. The format is described by 3 bytes
		u8 chs_first_sector[3];

		/// Partition type (see documentation on web)
		u8 partition_type;

		/// CHS address of last sector in partition.
		u8 chs_last_sector[3];

		/// LBA of first absolute sector in the partition.
		u8 lba_first_sector;

		/// Number of sectors in partition, in little-endian format
		u8 sectors_num;
        

	};
	
	/**
	  * MBR, marter boot record, 主引导记录结构体
      * 
	  */
	struct MBR{

		/// Code area for boot loader.
		u8 code[440];

		/// Disk signature (optional).
		leu32 disk_signature;

		/// reserved word
		leu16 reserved1;

		/// table of primary partitions, 分区
		MbrPartition partitions[4];

		// 分区有效标志
		uint16_t signature;
        
        // is_valid
        
        // get_partitions_number()
	};
	
	#pragma		pack(pop)   /**< 恢复对齐状态 */
	
}	// namespace nstd

// file_system_type parser

// file syster interface (with first sector as the construcor)

enum{
	MBR_SIGNATURE=0xAA55
};

/*	注：部分代码参考自 http://andybrown.me.uk/ws/2011/12/28/stm32plus-a-c-library-for-stm32-development/
*	ANDY'S WORKSHOP
*/
#endif
