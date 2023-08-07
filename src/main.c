#include <stdio.h>
#include "main.h"
#include "parse_args.h"
#include "ACEntry.h"

int main(int argc, char **argv) {
    Cli c = parse_args(argc, argv);
    printf("%d\n", c.type);
}
