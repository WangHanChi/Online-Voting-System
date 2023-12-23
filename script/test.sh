#!/bin/bash

gnome-terminal -- bash -c "./build/server 8888; exit"
gnome-terminal -- bash -c "./build/client 127.0.0.1 8888; exit"
