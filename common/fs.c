/*
 * Чтение файловой системы ext2fs
 */

#include "stdlib.h"
#include "printf.h"

#include "fs.h"
#include "types.h"
#include "assert.h"
#include "string.h"

/* Временное ограничение на размер файла */
#define TEMP_SIZE_LIMIT 49152	// 12 блоков по 4096 байт

    struct ext2_super_block sb;	// здесь будет суперблок!

    int BLKSIZE;	// размер блока файловой системы

    u8 buff_grp[4096];	// а сюда поместим дескрипторы групп
    u8 buff[4096];

    u32 major = 0;	/* старший номер */
    u32 minor = 0;	/* младший номер */
    u64 start = 0;	/* стартовое смещение на разделе */
    u64 count = 0;	/* сколько байт считывать */

    uint32_t location = 0;

/* Запросить разрешение на доступ к портам ATA-контроллера */
/*void set_perm()
{
    ioperm(CH0,8,1);
    ioperm(CH1,8,1);
    ioperm(CH2,8,1);
    ioperm(CH3,8,1);

    return;
}

void release_perm()
{
    ioperm(CH0,8,0);
    ioperm(CH1,8,0);
    ioperm(CH2,8,0);
    ioperm(CH3,8,0);

    return;
}
*/

/* Чтение суперблока */
void read_sb()
{
    if(location == 0) panic("EXT2: Failed to read ext2 superblock - location is zero (0)", 3, -1);
    start = 1024;		/* смещение */
    count = sizeof(sb);		/* сколько байт считывать */

    //uint64_t hello = '0';
    //printf("\nLocat. at 0x%X\n", location);
    //sb = *(struct ext2_super_block*)(void*)(location + start);
    //for(int i = 0; i <  0xFFFFFFFF; i++) {
    //    if(hello == *(uint64_t*)i)
    //        printf("Hello  at 0x%X\n", i - 0x6035);
        //return;
    //}
    memset(&sb, 0, sizeof(struct ext2_super_block) );

    if(memcpy(&sb, (void*)(location + start), count) == 0) {
        panic("EXT2: Error read superblock\n", 2 ,-1);
    //exit(-1);
    }

/* Проверка идентификатора файловой системы (MAGIC-номер) */
    //printf("\nEXT2: Magic: 0x%x, loc: 0x%x", sb.s_magic, location);
    if(sb.s_magic != EXT2_SUPER_MAGIC) {
        panic("EXT2: Invalid sb magic!", 3, -1);
    //exit(-1);
    }

#ifdef DEBUG
    printf("\nSuperblock info\n-----------\n");
    printf("Inodes count\t\t-\t%u\n",sb.s_inodes_count);
    printf("Blocks count\t\t-\t%u\n",sb.s_blocks_count);
    printf("Block size\t\t-\t%u\n",1024 << sb.s_log_block_size);
    printf("First inode\t\t-\t%d\n",sb.s_first_ino);
    printf("Magic\t\t\t-\t0x%X\n",sb.s_magic);
    printf("Inode size\t\t-\t%d\n",sb.s_inode_size);
    printf("Inodes per group\t-\t%u\n",sb.s_inodes_per_group);
    printf("Blosks per group\t-\t%u\n",sb.s_blocks_per_group);
    printf("First data block\t-\t%u\n\n",sb.s_first_data_block);
#endif

    return;
}


/* Чтение дескрипторов групп */
void read_gd()
{

/* Размер блока файловой системы */
    BLKSIZE = 1024 << sb.s_log_block_size;

/* Смещение к блоку, в котором находятся дескрипторы групп */
    start = (sb.s_first_data_block + 1) * BLKSIZE;
    count = BLKSIZE;

    /* Считываем дескрипторы групп */
    if(memcpy(buff_grp, (void*)(location + start), count) == 0) {
        panic("EXT2: Error read group descriptor table\n", 3, -1);
    //exit(-1);
    }

    return;
}


/* Получить содержимое inode под номером i_num */
void get_inode(int i_num, struct ext2_inode *in)
{

/*
 * i_num - номер inode файла
 */

    struct ext2_group_desc gd;
    u64 group,
    index; //index - индекс в таблице inode_table

/* Номер группы, в которой находится inode под номером i_num */
    group = (i_num - 1) / sb.s_inodes_per_group;

    memset((void *)&gd, 0, sizeof(gd));
    memcpy((void *)&gd, buff_grp + group*(sizeof(gd)), sizeof(gd));

/* Индекс в таблице inode */
    index = (i_num - 1) % sb.s_inodes_per_group;

    start = (u64)(gd.bg_inode_table) * BLKSIZE + index * sb.s_inode_size;
    count = sb.s_inode_size;


    if(memcpy((u8*)in, (void*)location + start, count) == 0) {
        panic("EXT2: Error read inode\n", 3, -1);
    //exit(-1);
    }

    return;
}


/* Прочитать информационный блок # (blknum) из указанного inode */
void read_iblock(struct ext2_inode *i, int blknum)
{

    start = (u64)(i->i_block[blknum])*BLKSIZE;
    count = BLKSIZE;

    if(memcpy(buff, (void*)location + start, count) == 0) {
        panic("EXT2: read info block\n", 3, -1);
    //exit(-1);
    }

    return;
}


/* Прочитать корневой каталог */
void get_root_dentry()
{
    int outf;
    struct ext2_inode in;

    get_inode(EXT2_ROOT_INO, &in);
    read_iblock(&in, 0);

#ifdef DEBUG
    outf = open("root.dent",O_CREAT|O_RDWR,0600);
    write(outf,buff,BLKSIZE);
    close(outf);
#endif

    return;
}


/* Получить номер inode по имени файла */
int get_i_num(char *name)
{

#ifdef DEBUG
    int i = 0;
#endif

    int rec_len = 0, name_len = 0;

    name_len = strlen(name);
    if((!name_len) || (name_len > EXT2_NAME_LEN)) {
        printf("Invalid filename!\n");
        return -1;
    }

    struct ext2_dir_entry_2 *  dent;
    for(;;) {
        dent = (struct ext2_dir_entry_2*)(buff + rec_len);
        //memcpy((void *)&dent, (buff + rec_len), sizeof(dent));
        //((char*)(dent.name)) = (char*)((&dent.file_type)+1);

        //uint32_t addr = (uint32_t)(&dent.file_type)+1;
        //void * target = (void*)&dent;
        //*(uint32_t*)(target+8) = addr;
        char * dname = kmalloc(dent->name_len+1);
        memcpy(dname, dent->name, dent->name_len);
        dname[dent->name_len] = '\0';
    #ifdef DEBUG
        //printf("Record # %d\n",i++);
        printf("Record lenght - 0x%X\n", dent->rec_len);
        printf("File name - %s\n", dname);
        printf("Name lenght- 0x%X\n", dent->name_len);
    #endif

        if(!dent->rec_len) {
            kfree(dname);
            return (-1);
        }
        if(!memcmp(dname, name, strlen(name))) {
            kfree(dname);
            break;
        }
        kfree(dname);
        rec_len += dent->rec_len;

    }

    return dent->inode;
}



/* Инициализировать ATA устройства * /
int ata_init()
{
    int i = 0;
    u8 dev = GET_DEV(minor);	/* номер канала (0,1,2,3) * /

    if(blkdev_init() < 0) return -1;

    if(DEV_STAT(dev) != ATA) {
    perror("device type");
    exit(-1);
    }

#ifdef DEBUG
/* Информация об основных разделах на устройстве 0 * /
    for(; i < 4; i++) {
    printf("\nТип раздела %d - 0x%x\n",i,DEV_PT(dev,i).type_part);
    printf("Признак загрузки - 0x%x\n",DEV_PT(dev,i).bootable);
    printf("Секторов в разделе %d - %d\n",i,DEV_PT(dev,i).sect_total);
    printf("Размер раздела %d в блоках - %d\n",i,DEV_PT(dev,i).sect_total/(BLK_SIZE/512));
    printf("Секторов перед разделом %d - %d\n\n",i,DEV_PT(dev,i).sect_before);
    }
#endif

    return 0;
}*/


/* Чтение информационных блоков файла */
int read_file_blocks(struct ext2_inode *in, u8 *data_buff, u32 *num, u32 seek)
{
    int i = 0, n = 0;
    u16 start_block, end_block, num_block, tail;
    u8 *cache_buff;

/*
 * start_block - стартовый блок
 * end_block - конечный блок
 * num_block - число блоков для считывания
 * tail - смещение к данным
 */


    if(seek >= (in->i_size)) return -1;
    if(seek >= TEMP_SIZE_LIMIT) return -1;
    if(((*num) + seek) > (in->i_size)) (*num) = (in->i_size) - seek;
    if(((*num) + seek) > TEMP_SIZE_LIMIT) (*num) = TEMP_SIZE_LIMIT - seek;

    start_block = seek/BLKSIZE;
    end_block = (seek + (*num))/BLKSIZE;
    tail = seek % BLKSIZE;
    num_block = (end_block - start_block) + 1;

    cache_buff = (u8 *)kmalloc(num_block * BLKSIZE);
    memset(cache_buff, 0, num_block * BLKSIZE);

    i = start_block;

    for(; n < num_block; n++) {
        read_iblock(in,i);
        memcpy((cache_buff + (n * BLKSIZE)), buff, BLKSIZE);
        i += 1;
    }

    memcpy(data_buff, (cache_buff + tail), (*num));

    kfree(cache_buff);
    return 0;
}



int ext2_read_file(u32 maj_num, u32 min_num, u8 *full_path, u8 *data_buff, u32 *num, u32 seek)
{

/*
 * Параметры функции:
 * full_path - абсолютное путевое имя файла
 * data_buff - буфер для данных
 * num - сколько байт считывать из файла
 * seek - смещение в файле
 */

    struct ext2_inode in;
    unsigned char tmp_buff[EXT2_NAME_LEN];
    static int i = 1;
    int n, i_num, type;


    if(full_path[0] != '/') {
        panic("EXT2: slash", 3, -1);
        //exit(-1);
    }

    major = maj_num;
    minor = min_num;

    //set_perm();

    //if(ata_init() < 0) {
    //perror("ata_init");
    //exit(-1);
    //}

/*
 * Прочитать суперблок, таблицу дескрипторов групп,
 * и получить содержимое корневого каталога
 */
    read_sb();
    read_gd();
    get_root_dentry();

    while(1) {

        memset(tmp_buff, 0, sizeof(tmp_buff));

        for(n = 0 ; n < EXT2_NAME_LEN; n++, i++) {
            tmp_buff[n] = full_path[i];
            if((tmp_buff[n] == '/') || (tmp_buff[n] == '\0')) {
            i++;
            break;
            }
        }
        tmp_buff[n] = '\0';

        i_num = get_i_num(tmp_buff);
        if(i_num < 0) {
            printf("No such file\n");
            return -1;
            //exit(-1);
        }
        get_inode(i_num, &in);

    #ifdef DEBUG
        printf("Inode number - %u\n", i_num);
        printf("File name - %s\n", tmp_buff);
        printf("File size - %u\n",in.i_size);
    #endif

        type = ((in.i_mode & 0xF000) >> 12);

        if(type & 0x04) {
            read_iblock(&in, 0);
            continue;
        }

        if(type & 0x08) {
            if(read_file_blocks(&in, data_buff, num, seek) < 0) return -1;
            break;
        }
    }

    //release_perm();
    return 0;
}

FILE* fopen(const char* name) {
    int in = get_i_num(name);
    if(in == -1) return NULL;

    struct ext2_inode node;
    get_inode(in, &node);
    uint32_t type;
    type = ((node.i_mode & 0xF000) >> 12);

    if(type & 0x04) {
        //read_iblock(&in, 0);
        //continue;
        return NULL;
    }

    //if(type & 0x08) {
    //    if(read_file_blocks(&in, data_buff, num, seek) < 0) return -1;
    //    break;
    //}

    FILE * f = (FILE*)kmalloc(sizeof(FILE));
    f->_fileno = in;

    f->_IO_read_base = 0;
    f->_IO_read_end = node.i_size;
    f->_IO_read_ptr = 0;

    return f;
}

size_t fread( void *buffer, size_t size, size_t count, FILE *stream ) {

}

int fgetc( FILE *stream ) {

}

int fclose( FILE *stream) {
    kfree(stream);
}


