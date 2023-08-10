#include <stdio.h>
#include "main.h"
#include "parse_args.h"
#include "entry.h"
#include "hash_table.h"

int main(int argc, char **argv) {
    Cli *c = parse_args(argc, argv);
    if (!c)
        return 0;

    printf("%d\n", c->type);
}
