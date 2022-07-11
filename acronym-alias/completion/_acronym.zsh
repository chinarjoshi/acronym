#compdef acronym

_arguments "1: :((add\:'Add command to an alias' \
                 edit\:'Directly edit aliases file' \
                 rm\:'Remove alias(es) or section(s)' \
                 change\:'Rename an alias' \
                 suggest\:'Suggest pre-defined aliases based on cmd hist' \
                 print\:'Pretty prints contents of aliases file' \
                 help\:'Help page'))" \
            "--flags[Includes command line flags in auto-generated acronym]" \
            "--section[Deletes whole sections instead of keys]"
