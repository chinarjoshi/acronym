_acronym() 
{
    local keywords='add edit rm change suggest print help'
    local cur=${COMP_WORDS[COMP_CWORD]}
    local prev=${COMP_WORDS[COMP_CWORD-1]}
    case ${COMP_CWORD} in
        1)
            COMPREPLY=( $(compgen -W "${keywords}" -- ${cur}) )
            ;;
        2)
            case ${prev} in
                add)
                    COMPREPLY=( $(compgen -W "--flags" -- ${cur}) )
                    ;;
                rm)
                    COMPREPLY=( $(compgen -W "--section" -- ${cur}) )
                    ;;
            esac
            ;;
        *)
            COMPREPLY=()
            ;;
    esac
}
complete -F _acronym acronym
