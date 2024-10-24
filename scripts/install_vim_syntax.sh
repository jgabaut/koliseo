#!/bin/bash

# Define paths
SYNTAX_DIR="$HOME/.vim/syntax"
AFTER_SYNTAX_DIR="$HOME/.vim/after/syntax"
C_VIM_FILE="$AFTER_SYNTAX_DIR/c.vim"
MYLIB_SYNTAX_FILE="$SYNTAX_DIR/libkoliseo.vim"

# Ensure the directories exist
mkdir -p "$SYNTAX_DIR"
mkdir -p "$AFTER_SYNTAX_DIR"

# Copy your syntax file
echo "Installing libkoliseo syntax file..."
cp ./scripts/libkoliseo.vim "$MYLIB_SYNTAX_FILE"

# Ensure the c.vim file exists
if [ ! -f "$C_VIM_FILE" ]; then
    touch "$C_VIM_FILE"
fi

# Add the line to source libkoliseo.vim if it doesn't already exist
if ! grep -q 'runtime syntax/libkoliseo.vim' "$C_VIM_FILE"; then
    echo "runtime syntax/libkoliseo.vim" >> "$C_VIM_FILE"
    echo "Added libkoliseo syntax sourcing to c.vim"
else
    echo "libkoliseo syntax is already sourced in c.vim"
fi

echo "Installation complete."
