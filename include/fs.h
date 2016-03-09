#ifndef FS_H
#define FS

#include "types.h"



#define EXT2_NAME_LEN 255

/*
 * Maximal count of links to a file
 */
#define EXT2_LINK_MAX		32000

#define EXT2_SB_MAGIC_OFFSET	0x38
#define EXT2_SB_BLOCKS_OFFSET	0x04
#define EXT2_SB_BSIZE_OFFSET	0x18
#define EXT2_SUPER_MAGIC	0xEF53
#define EXT2_ROOT_INO		 2	/* Root inode */
/*
 * Constants relative to the data blocks
 */
#define	EXT2_NDIR_BLOCKS		12
#define	EXT2_IND_BLOCK			EXT2_NDIR_BLOCKS
#define	EXT2_DIND_BLOCK			(EXT2_IND_BLOCK + 1)
#define	EXT2_TIND_BLOCK			(EXT2_DIND_BLOCK + 1)
#define	EXT2_N_BLOCKS			(EXT2_TIND_BLOCK + 1)

extern uint32_t location;

struct ext2_super_block {
    uint32_t	s_inodes_count;		/* Inodes count */
    uint32_t	s_blocks_count;		/* Blocks count */
    uint32_t	s_r_blocks_count;	/* Reserved blocks count */
    uint32_t	s_free_blocks_count;	/* Free blocks count */
    uint32_t	s_free_inodes_count;	/* Free inodes count */
    uint32_t	s_first_data_block;	/* First Data Block */
    uint32_t	s_log_block_size;	/* Block size */
    uint32_t	s_log_frag_size;	/* Fragment size */
    uint32_t	s_blocks_per_group;	/* # Blocks per group */
    uint32_t	s_frags_per_group;	/* # Fragments per group */
    uint32_t	s_inodes_per_group;	/* # Inodes per group */
    uint32_t	s_mtime;		/* Mount time */
    uint32_t	s_wtime;		/* Write time */
    uint16_t	s_mnt_count;		/* Mount count */
    uint16_t	s_max_mnt_count;	/* Maximal mount count */
    uint16_t	s_magic;		/* Magic signature */
    uint16_t	s_state;		/* File system state */
    uint16_t	s_errors;		/* Behaviour when detecting errors */
    uint16_t	s_minor_rev_level; 	/* minor revision level */
    uint32_t	s_lastcheck;		/* time of last check */
    uint32_t	s_checkinterval;	/* max. time between checks */
    uint32_t	s_creator_os;		/* OS */
    uint32_t	s_rev_level;		/* Revision level */
    uint16_t	s_def_resuid;		/* Default uid for reserved blocks */
    uint16_t	s_def_resgid;		/* Default gid for reserved blocks */
    /*
     * These fields are for EXT2_DYNAMIC_REV superblocks only.
     *
     * Note: the difference between the compatible feature set and
     * the incompatible feature set is that if there is a bit set
     * in the incompatible feature set that the kernel doesn't
     * know about, it should refuse to mount the filesystem.
     *
     * e2fsck's requirements are more strict; if it doesn't know
     * about a feature in either the compatible or incompatible
     * feature set, it must abort and not try to meddle with
     * things it doesn't understand...
     */
    uint32_t	s_first_ino; 		/* First non-reserved inode */
    uint16_t   s_inode_size; 		/* size of inode structure */
    uint16_t	s_block_group_nr; 	/* block group # of this superblock */
    uint32_t	s_feature_compat; 	/* compatible feature set */
    uint32_t	s_feature_incompat; 	/* incompatible feature set */
    uint32_t	s_feature_ro_compat; 	/* readonly-compatible feature set */
    uint8_t	s_uuid[16];		/* 128-bit uuid for volume */
    char	s_volume_name[16]; 	/* volume name */
    char	s_last_mounted[64]; 	/* directory where last mounted */
    uint32_t	s_algorithm_usage_bitmap; /* For compression */
    /*
     * Performance hints.  Directory preallocation should only
     * happen if the EXT2_COMPAT_PREALLOC flag is on.
     */
    uint8_t	s_prealloc_blocks;	/* Nr of blocks to try to preallocate*/
    uint8_t	s_prealloc_dir_blocks;	/* Nr to preallocate for dirs */
    uint16_t	s_padding1;
    /*
     * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
     */
    uint8_t	s_journal_uuid[16];	/* uuid of journal superblock */
    uint32_t	s_journal_inum;		/* inode number of journal file */
    uint32_t	s_journal_dev;		/* device number of journal file */
    uint32_t	s_last_orphan;		/* start of list of inodes to delete */
    uint32_t	s_hash_seed[4];		/* HTREE hash seed */
    uint8_t	s_def_hash_version;	/* Default hash version to use */
    uint8_t	s_reserved_char_pad;
    uint16_t	s_reserved_word_pad;
    uint32_t	s_default_mount_opts;
    uint32_t	s_first_meta_bg; 	/* First metablock block group */
    uint32_t	s_reserved[190];	/* Padding to the end of the block */
};

struct ext2_group_desc
{
    uint32_t	bg_block_bitmap;		/* Blocks bitmap block */
    uint32_t	bg_inode_bitmap;		/* Inodes bitmap block */
    uint32_t	bg_inode_table;		/* Inodes table block */
    uint16_t	bg_free_blocks_count;	/* Free blocks count */
    uint16_t	bg_free_inodes_count;	/* Free inodes count */
    uint16_t	bg_used_dirs_count;	/* Directories count */
    uint16_t	bg_pad;
    uint32_t	bg_reserved[3];
};

struct ext2_inode {
    uint16_t	i_mode;		/* File mode */
    uint16_t	i_uid;		/* Low 16 bits of Owner Uid */
    uint32_t	i_size;		/* Size in bytes */
    uint32_t	i_atime;	/* Access time */
    uint32_t	i_ctime;	/* Creation time */
    uint32_t	i_mtime;	/* Modification time */
    uint32_t	i_dtime;	/* Deletion Time */
    uint16_t	i_gid;		/* Low 16 bits of Group Id */
    uint16_t	i_links_count;	/* Links count */
    uint32_t	i_blocks;	/* Blocks count */
    uint32_t	i_flags;	/* File flags */
    union {
        struct {
            uint32_t  l_i_reserved1;
        } linux1;
        struct {
            uint32_t  h_i_translator;
        } hurd1;
        struct {
            uint32_t  m_i_reserved1;
        } masix1;
    } osd1;				/* OS dependent 1 */
    uint32_t	i_block[EXT2_N_BLOCKS];/* Pointers to blocks */
    uint32_t	i_generation;	/* File version (for NFS) */
    uint32_t	i_file_acl;	/* File ACL */
    uint32_t	i_dir_acl;	/* Directory ACL */
    uint32_t	i_faddr;	/* Fragment address */
    union {
        struct {
            uint8_t	l_i_frag;	/* Fragment number */
            uint8_t	l_i_fsize;	/* Fragment size */
            uint16_t	i_pad1;
            uint16_t	l_i_uid_high;	/* these 2 fields    */
            uint16_t	l_i_gid_high;	/* were reserved2[0] */
            uint32_t	l_i_reserved2;
        } linux2;
        struct {
            uint8_t	h_i_frag;	/* Fragment number */
            uint8_t	h_i_fsize;	/* Fragment size */
            uint16_t	h_i_mode_high;
            uint16_t	h_i_uid_high;
            uint16_t	h_i_gid_high;
            uint32_t	h_i_author;
        } hurd2;
        struct {
            uint8_t	m_i_frag;	/* Fragment number */
            uint8_t	m_i_fsize;	/* Fragment size */
            uint16_t	m_pad1;
            uint32_t	m_i_reserved2[2];
        } masix2;
    } osd2;				/* OS dependent 2 */
};

struct ext2_dir_entry_2 {
    uint32_t	inode;			/* Inode number */
    uint16_t	rec_len;		/* Directory entry length */
    uint8_t	name_len;		/* Name length */
    uint8_t	file_type;
    char	name[];			/* File name, up to EXT2_NAME_LEN */
};

int ext2_read_file(u32 maj_num, u32 min_num, u8 *full_path, u8 *data_buff, u32 *num, u32 seek);

//struct __IO_BUF;

struct __IO_BUF{
  int _flags;		/* High-order word is _IO_MAGIC; rest is flags. */
#define _IO_file_flags _flags

  /* The following pointers correspond to the C++ streambuf protocol. */
  /* Note:  Tk uses the _IO_read_ptr and _IO_read_end fields directly. */
  char* _IO_read_ptr;	/* Current read pointer */
  char* _IO_read_end;	/* End of get area. */
  char* _IO_read_base;	/* Start of putback+get area. */
  char* _IO_write_base;	/* Start of put area. */
  char* _IO_write_ptr;	/* Current put pointer. */
  char* _IO_write_end;	/* End of put area. */
  char* _IO_buf_base;	/* Start of reserve area. */
  char* _IO_buf_end;	/* End of reserve area. */
  /* The following fields are used to support backing up and undo. */
  char *_IO_save_base; /* Pointer to start of non-current get area. */
  char *_IO_backup_base;  /* Pointer to first valid character of backup area */
  char *_IO_save_end; /* Pointer to end of non-current get area. */

  //struct _IO_marker *_markers;

  //__IO_BUF *_chain;

  int _fileno;
  int _flags2;
  //_IO_off_t _old_offset; /* This used to be _offset but it's too small.  */

#define __HAVE_COLUMN /* temporary */
  /* 1+column number of pbase(); 0 is unknown. */
  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];

  /*  char* _save_gptr;  char* _save_egptr; */

  //_IO_lock_t *_lock;
};

typedef struct __IO_BUF FILE;

/*
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08 // Файл является активной точкой монтирования?

typedef uint32_t (*read_type_t)(struct fs_node*,uint32_t,uint32_t,uint8_t*);
typedef uint32_t (*write_type_t)(struct fs_node*,uint32_t,uint32_t,uint8_t*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct dirent * (*readdir_type_t)(struct fs_node*,uint32_t);
typedef struct fs_node * (*finddir_type_t)(struct fs_node*,char *name); 

typedef struct fs_node
{
   char name[128];     // Имя файла.
   uint32_t mask;        // Маска прав доступа.
   uint32_t uid;         // Пользователь, владеющий файлом.
   uint32_t gid;         // Группа, владеющая файлом.
   uint32_t flags;       // Включает тип нода. Смотрите определение #defines, приведенное выше.
   uint32_t inode;       // Зависит от устройства, позволяет файловой системе идентифицировать файлы.
   uint32_t length;      // Размер файла в байтах.
   uint32_t impl;        // Номер,б зависящий от реализации.
   read_type_t read;
   write_type_t write;
   open_type_t open;
   close_type_t close;
   readdir_type_t readdir;
   finddir_type_t finddir;
   struct fs_node *ptr; // Используется для точек монтирования и символических ссылок.
   uint32_t count;
} fs_node_t; 

struct dirent // One of these is returned by the readdir call, according to POSIX.
{
  char name[128]; // Имя файла..
  uint32_t ino;     // Номер inode. Требеся для POSIX.
}; 

extern fs_node_t *fs_root; // The root of the filesystem.

// Стандартные функции чтения, записи, открытия, закрытия. Обратите внимание,
//  что у них всех используется суффикс _fs с тем, чтобы отличать от функций чтения,
// записи, открытия и закрытия дескрипторов файлов, а не нодов файлов.
uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
void open_fs(fs_node_t *node, uint8_t read, uint8_t write);
void close_fs(fs_node_t *node);
struct dirent *readdir_fs(fs_node_t *node, uint32_t index);
fs_node_t *finddir_fs(fs_node_t *node, char *name);
*/

#endif // FS_H
