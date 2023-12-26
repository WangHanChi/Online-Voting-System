#!/bin/bash

YELLOW='\e[33;01m'
UNDERLINE='\e[4m'
RESET='\e[0m'

if [ -d "build" ]; then
  echo "Removing existing build directory..."
  rm -r build/
fi

mkdir build && cd build
cmake ..
make
echo -e ""
echo -e "${YELLOW}${UNDERLINE}Now please enter <make test> to verify this system${RESET}"
echo -e ""