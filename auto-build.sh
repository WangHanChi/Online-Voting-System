#!/bin/bash

# Define color codes
YELLOW='\e[33;01m'
UNDERLINE='\e[4m'
RESET='\e[0m'

# Function to remove existing build directory
remove_build_directory() {
  if [ -d "$1" ]; then
    echo "Removing existing build directory: $1"
    rm -r "$1"
  fi
}

# Build server
remove_build_directory "server/build"
mkdir -p server/build && cd server/build
echo -e "${YELLOW}${UNDERLINE}Building server...${RESET}"
cmake ..
make
cd ../..

# Build client
remove_build_directory "client/build"
mkdir -p client/build && cd client/build
echo -e "${YELLOW}${UNDERLINE}Building client...${RESET}"
cmake ..
make
cd ../..


echo -e ""
echo -e "${YELLOW}${UNDERLINE}Now please enter <./script/test.sh> to verify this system${RESET}"
echo -e ""
