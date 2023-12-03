alias build="meson compile -C ~/projects/acronym/build" ## dev
alias run="~/projects/acronym/build/acronym" ## dev
alias debug="gdb -q ~/projects/acronym/build/tests" ## dev
alias test="CK_RUN_SUITE=$SUITE CK_FORK=yes ~/projects/acronym/build/tests" ## dev
alias tests="CK_FORK=yes ~/projects/acronym/build/tests" ## dev
alias format="find . -name '*.c' -o -name '*.h' -exec clang-format -style='{BasedOnStyle: llvm, IndentWidth: 4, ColumnLimit: 80, UseTab: Never}' -i {} \;" ## dev
