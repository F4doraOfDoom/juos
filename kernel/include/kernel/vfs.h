#ifndef KERNEL_VFS_H_
#define KERNEL_VFS_H_

#include <kernel/kdef.h>

NAMESPACE_BEGIN(kernel)

/** 
 * @brief All implementations of a file system must provide a class
 * that implements this interface, as it is used by the VFS.
 */
class FsHandler
{
public:
    virtual void create_file(const char* filename) = 0;

    virtual void delete_file(const char* filename) = 0;

    virtual ~FsHandler() = default;
};

NAMESPACE_END(kernel)

#endif // KERNEL_VFS_H_