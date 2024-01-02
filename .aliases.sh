alias tests='CK_FORK=yes ~/projects/acronym/build/tests' ## dev
alias build='meson compile -C ~/projects/acronym/build' ## dev
alias test='CK_RUN_SUITE=$SUITE CK_FORK=yes ~/projects/acronym/build/tests' ## dev
alias debug='gdb -q ~/projects/acronym/build/tests' ## dev
alias acronym='~/projects/acronym/build/acronym' ## dev

# asdf
# asdf
# asdf
alias format="find . -name '*.c' -o -name '*.h' -exec clang-format -style='{BasedOnStyle: llvm, IndentWidth: 4, ColumnLimit: 80, UseTab: Never}' -i {} \;" ## dev
