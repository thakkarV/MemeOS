#ifndef SYS_UFS_1_0
#define SYS_UFS_1_0

#include <sys/types.h>

// block size of 256 bytes
#define UFS_BLOCK_SIZE 0x100UL

// disk size of 2 MB
#define UFS_DISK_SIZE 0x200000UL

// superblock maging number if and when serialized to disk
#define UFS_HEADER_MAGIC 0x1337D00DUL

// direct pointers
#define UFS_NUM_DIRECT_PTRS 8

// number of pointers in block
#define UFS_NUM_PTRS_PER_BLK UFS_BLOCK_SIZE/(sizeof(ufs_datablock_t *))

// length of inode arrya in number of blocks
#define UFS_SIZEOF_INODE_ARRAY 0x100UL

// (UFS_SIZEOF_INODE_ARRAY * UFS_BLOCK_SIZE) / sizeof(inode_t)
#define UFS_NUM_MAX_INODES 0x800UL

// number of blocks for the block bitmap
#define UFS_NUM_BITMAP_BLOCKS 4

// leftover lenght / UFS_BLOCK_SIZE
#define UFS_NUM_MAX_BLOCKS 0xB57UL

// max number of files a directory can have
#define UFS_MAX_FILE_IN_DIR 16

// TODO: max file size
#define UFS_MAX_FILESIZE 0x104800UL

#define UFS_DIR_DELIM "/"

/* LOGICAL LAYOUT OF UNIX FILESYSTEM - 256 byte block size
	I) Superblock
		blk number 0

	II) Index Nodes Array
		256 Blks if index nodes (inode_t) structs containing file attributes
		Each inode is 64 bytes in size. layout of the node is described in the struct code

	III) Block Bitmap
		After index blocks are 4 blocks of block bitmaps used to track allocated and unallocated blocks in the fs

	IV) Data
		Rest of the 2 MB holds the files stores in the file system.
		Files could be either direcotry entries or regular files.
		-> Regualr files hold arbitraty data
		-> dirents hold a touple of (filename, inode number) pair
			--> filename is a null padded 14 byte string
			--> inode number is a 16 bit int
*/

typedef enum ufs_blocktype_t
{
	FREE = 0,
	DIR = 1,
	REG = 2
} ufs_blocktype_t;

typedef enum inode_status_t
{
    OCCUPIED = false,
    FREE = true
} inode_status_t;

// SIZEOF(inode) = 64 BYTES
typedef struct inode_t
{
	/* inode_t layout
	 *	+------+------+-+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+--+
	 *	| Type | Size | 8 Direct BlkPtr | 1 Indirect BlkPtr | 1 Double Indirect BlkPtr |
	 *	+------+------+-+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+--+
	**/
	ufs_blocktype_t type;
	size_t size;
	uint32_t attrib;

	union
	{
		struct
		{
			ufs_dirblock_t *dirblock_ptr;
			uint32_t dir_attrib[9]; // padding in reality
		};
		struct
		{
			ufs_datablock_t *direct_block_ptrs[8];
			ufs_datablock_t **indirect_block_ptr;
			ufs_datablock_t ***double_indirect_block_ptr;
		};		
	};
	
	// padding at the end to ensure size of 64 bytes
	uint32_t padding[3];
} __attribute__((packed)) inode_t;


// FORMAT OF A SINGLE DIRECTORY ENTRY
typedef struct ufs_dirent_t
{
	char filename[14];
	uint16_t inode_num;
} __attribute__((packed)) ufs_dirent_t;


// DIRECTORY BLOCK
typedef struct ufs_dirblock_t
{
	// number of dir entries is equal to = FS_BLOCK_SIZE / sizeof(dirent)
	ufs_dirent_t entries[UFS_MAX_FILE_IN_DIR];
} __attribute__((packed)) ufs_dirblock_t;


// DATA BLOCK
typedef struct ufs_datablock_t
{
	char data[UFS_BLOCK_SIZE];
} __attribute__((packed)) ufs_datablock_t;


// SUPER BLOCK
typedef struct ufs_superblock_t
{
	uint32_t magic;
	uint32_t num_blocks;
	size_t   block_size;
	uint32_t num_inodes;
	uint32_t num_free_blocks;
	uint32_t num_free_inodes;
	uint32_t num_dirs;
	uint32_t num_files;

	inode_t         *inode_array;
	uint8_t         *blk_bitmap;
	ufs_datablock_t *root_blk;

	// padding at the end to ensure size of 64 bytes
	uint32_t padding[53];
} __attribute__((packed)) ufs_superblock_t;


#endif // SYS_UFS_1_0
