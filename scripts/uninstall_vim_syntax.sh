#!/bin/bash

# Paths
C_VIM_FILE="$HOME/.vim/after/syntax/c.vim"

# Remove the line from c.vim
if grep -q 'runtime syntax/libkoliseo.vim' "$C_VIM_FILE"; then
    sed -i '/runtime syntax\/libkoliseo.vim/d' "$C_VIM_FILE"
    echo "Removed libkoliseo syntax from c.vim"
else
    echo "libkoliseo syntax is not sourced in c.vim"
fi

# Optionally remove the libkoliseo.vim file
# Uncomment the line below if you want to delete the syntax file as well
# rm -f ~/.vim/syntax/libkoliseo.vim

echo "Uninstallation complete."
