#include "platform_utils.h"

#include <limits.h>
#include <unistd.h>
#include <libgen.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

void SetWorkingDirectoryToExecutable(void)
{
#ifdef __APPLE__
    char path[PATH_MAX];
    uint32_t size = sizeof(path);

    if (_NSGetExecutablePath(path, &size) == 0)
    {
        char *dir = dirname(path);
        chdir(dir);
    }
#endif
}