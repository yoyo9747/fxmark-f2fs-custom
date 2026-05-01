/**
 * Nanobenchmark: Read operation with a background inhibitor
 *   RF. PROCESS = {read private file}
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>
#include "fxmark.h"
#include "util.h"

static void set_test_root(struct worker *worker, char *test_root)
{
    struct fx_opt *fx_opt = fx_opt_worker(worker);
    sprintf(test_root, "%s/%d", fx_opt->root, worker->id);
}

static int pre_work(struct worker *worker)
{
    char *page = NULL;
    struct bench *bench = worker->bench;
    char test_root[PATH_MAX];
    char file[PATH_MAX];
    int fd, rc = 0;

    /* create test root */
    set_test_root(worker, test_root);
    rc = mkdir_p(test_root);
    if (rc) return rc;

    /* create a test file */
    snprintf(file, PATH_MAX, "%s/n_file_rd_bg.dat", test_root);
    if ((fd = open(file, O_CREAT | O_RDWR, S_IRWXU)) == -1)
        goto err_out;

    if (posix_memalign((void **)&(worker->page), PAGE_SIZE, PAGE_SIZE))
        goto err_out;

    page = worker->page;
    if (!page)
        goto err_out;

    /* set flag with O_DIRECT if necessary */
    if (bench->directio && (fcntl(fd, F_SETFL, O_DIRECT) == -1))
        goto err_out;

    /* populate one block */
    if (write(fd, page, PAGE_SIZE) != PAGE_SIZE)
        goto err_out;

out:
    /* put fd to worker's private */
    worker->private[0] = (uint64_t)fd;
    return rc;

err_out:
    bench->stop = 1;
    rc = errno;
    if (page)
        free(page);
    goto out;
}

static int fg_work(struct worker *worker)
{
    char *page = worker->page;
    struct bench *bench = worker->bench;
    int fd, rc = 0;
    uint64_t iter = 0;

    assert(page);

    fd = (int)worker->private[0];
    for (iter = 0; !bench->stop; ++iter) {
        if (pread(fd, page, PAGE_SIZE, 0) != PAGE_SIZE)
            goto err_out;
    }

out:
    close(fd);
    worker->works = (double)iter;
    free(page);
    return rc;

err_out:
    bench->stop = 1;
    rc = errno;
    goto out;
}

static int bg_work(struct worker *worker)
{
    char *page = worker->page;
    struct bench *bench = worker->bench;
    int fd, rc = 0;
    uint64_t iter = 0;

    assert(page);

    fd = (int)worker->private[0];
    for (iter = 0; !bench->stop; ++iter) {
        if (pwrite(fd, page, PAGE_SIZE, 0) != PAGE_SIZE)
            goto err_out;
    }

out:
    close(fd);
    worker->works = (double)iter;
    free(page);
    return rc;

err_out:
    bench->stop = 1;
    rc = errno;
    goto out;
}

static int main_work(struct worker *worker)
{
    if (worker->is_bg)
        return bg_work(worker);
    return fg_work(worker);
}

struct bench_operations n_file_rd_bg_ops = {
    .pre_work  = pre_work,
    .main_work = main_work,
};
