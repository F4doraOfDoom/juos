#ifndef KERNEL_VFS_H_
#define KERNEL_VFS_H_

#include <kernel/kdef.h>
#include <kernel/data_structures/string.hpp>

using kernel::data_structures::String;

NAMESPACE_BEGIN(kernel)

/** 
 * @brief All implementations of a file system must provide a class
 * that implements this interface, as it is used by the VFS.
 */
class FsHandler
{
public:
    virtual void CreateFile(const String& filename) = 0;

    virtual void CreateFile(const char* filename) = 0;

    virtual void DeleteFile(const char* filename) = 0;

    virtual void DeleteFile(const String& filename) = 0;

    virtual void MakeNewFs() = 0;

    virtual ~FsHandler() = default;
};

NAMESPACE_END(kernel)

#endif // KERNEL_VFS_H_