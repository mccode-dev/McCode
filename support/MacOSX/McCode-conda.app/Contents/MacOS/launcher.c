/*
 * Minimal native launcher for the McCode conda app bundle.
 *
 * macOS determines an app's architecture from the Mach-O slices of
 * CFBundleExecutable. A shell-script executable has none, which makes
 * LaunchServices on Apple Silicon ask for Rosetta. This stub gives the
 * bundle a real (universal) binary and simply execs mccodegui.sh
 * sitting next to it.
 */
#include <limits.h>
#include <libgen.h>
#include <mach-o/dyld.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    char exe[PATH_MAX], real[PATH_MAX], script[PATH_MAX];
    uint32_t sz = sizeof exe;
    (void)argc;
    if (_NSGetExecutablePath(exe, &sz) != 0 || !realpath(exe, real)) return 1;
    snprintf(script, sizeof script, "%s/mccodegui.sh", dirname(real));
    argv[0] = script;          /* keeps `dirname $0` in the script correct */
    execv(script, argv);
    perror("execv");
    return 127;
}
