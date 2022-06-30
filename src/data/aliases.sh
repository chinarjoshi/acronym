declare -A aliases=(
	'pS'	'pacman -Syu'
	'pQ'	'pacman -Quent'
)
for key value in "${(@kv)aliases[@]}"; do
    alias "$letter"="$command"
done
