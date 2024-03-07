#!/bin/bash

# Define the path to the 'include' directory
INCLUDE_DIR="./include"

# Define the search and replace patterns
SEARCH_PATTERN="namespace simple_enum::inline v0_5"
REPLACE_PATTERN="namespace simple_enum::inline v0_6"

# Use find to recursively search for .h and .hpp files and sed to replace the text
find "$INCLUDE_DIR" \( -name "*.h" -o -name "*.hpp" \) -type f -exec sed -i "s/${SEARCH_PATTERN}/${REPLACE_PATTERN}/g" {} +

echo "Text replacement complete."
