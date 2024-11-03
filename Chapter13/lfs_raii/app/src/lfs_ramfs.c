#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <lfs_ramfs.h>

#define BLOCK_SZ 256
#define BLOCK_NUM 32

#define LFS_LOOKAHEAD_SIZE 8
#define LFS_BLOCK_CYCLES 500
#define LFS_READ_SIZE 16
#define LFS_PROG_SIZE 16

static uint8_t mem[BLOCK_NUM * BLOCK_SZ] = {0};

/*
 * Read a region in a block. Negative error codes are propogated
 * to the user.
 */
static int lfs_ramfs_read(const struct lfs_config *c, lfs_block_t block,
                          lfs_off_t off, void *buffer, lfs_size_t size);

/*
 * Program a region in a block. The block must have previously
 * been erased. Negative error codes are propogated to the user.
 * May return LFS_ERR_CORRUPT if the block should be considered bad.
 */
static int lfs_ramfs_prog(const struct lfs_config *c, lfs_block_t block,
                          lfs_off_t off, const void *buffer, lfs_size_t size);

/*
 * Erase a block. A block must be erased before being programmed.
 * The state of an erased block is undefined. Negative error codes
 * are propogated to the user.
 * May return LFS_ERR_CORRUPT if the block should be considered bad.
 * Not needed for ramfs.
 */
static int lfs_ramfs_erase(const struct lfs_config *c, lfs_block_t block);

/*
 * Sync the state of the underlying block device. Negative error codes
 * are propogated to the user.
 * Not needed for ramfs.
 */
static int lfs_ramfs_sync(const struct lfs_config *c);

/* configuration of the filesystem is provided by this struct */
const struct lfs_config lfs_ramfs_cfg = {
    .read = lfs_ramfs_read,
    .prog = lfs_ramfs_prog,
    .erase = lfs_ramfs_erase,
    .sync = lfs_ramfs_sync,

    .read_size = LFS_READ_SIZE,
    .prog_size = LFS_PROG_SIZE,
    .block_size = BLOCK_SZ,
    .block_count = BLOCK_NUM,
    .cache_size = BLOCK_SZ,
    .lookahead_size = LFS_LOOKAHEAD_SIZE,
    .block_cycles = LFS_BLOCK_CYCLES,
};

struct lfs_config * get_ramfs_lfs_config() {
    return &lfs_ramfs_cfg;
}

static int lfs_ramfs_read(const struct lfs_config *c, lfs_block_t block,
                          lfs_off_t off, void *buffer, lfs_size_t size)
{
        assert(buffer != NULL);
        memcpy(buffer, mem + (block * c->block_size) + off, size);
        return 0;
}

static int lfs_ramfs_prog(const struct lfs_config *c, lfs_block_t block,
                          lfs_off_t off, const void *buffer, lfs_size_t size)
{
        assert(buffer != NULL);
        memcpy(mem + (block * c->block_size) + off, buffer, size);
        return LFS_ERR_OK;
}

static int lfs_ramfs_erase(const struct lfs_config *c, lfs_block_t block)
{
        (void)c;
        (void)block;
        return LFS_ERR_OK;
}

static int lfs_ramfs_sync(const struct lfs_config *c)
{
        (void)c;
        return LFS_ERR_OK;
}
