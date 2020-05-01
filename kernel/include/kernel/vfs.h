#ifndef KERNEL_VFS_H_
#define KERNEL_VFS_H_

#include <kernel/kdef.h>
#include <kernel/data_structures/string.hpp>
#include <kernel/data_structures/vector.hpp>

using kernel::data_structures::String;
using kernel::data_structures::Vector;

NAMESPACE_BEGIN(kernel)

    NAMESPACE_BEGIN(vfs)

        struct Path
        {
            // the components that make up a path
            mutable Vector<String> components;

            bool current_directory;

            bool previous_directory;

            String ToString() const
            {
                String path("/");

                for (auto c : components)
                {
                    path.add(c);
                    path.add("/");
                }

                return path;
            }
        };

        /** 
         * @brief All implementations of a file system must provide a class
         * that implements this interface, as it is used by the VFS.
         */
        class FsHandler
        {
        public:
            virtual void CreateFile(const String& filename, const Path& path) = 0;

            virtual void CreateDirectory(const String& name, const Path& path) = 0;

            virtual void DeleteFile(const String& filename, const Path& path) = 0;

            virtual void WriteFile(const String& filename, const Path& path, const String& text) = 0;

            virtual void ReadFile(const String& filename, const Path& path, char* buffer, uint32_t max_size) = 0;

            virtual bool FileExists(const String& filename, const Path& path) const = 0;

            virtual void ReadFs(bool delete_cache) = 0;

            virtual void ListFs(const Path& path) = 0;

            virtual void Tree(const Path& path) = 0;

            virtual void MakeNewFs() = 0;

            virtual ~FsHandler() = default;
        };

    NAMESPACE_END(vfs)

NAMESPACE_END(kernel)

#endif // KERNEL_VFS_H_