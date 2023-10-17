#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int getuptime(void) {
    return uptime();
}