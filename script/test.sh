#!/bin/bash

gnome-terminal -- bash -c "./server/build/server 8888; exit"
gnome-terminal -- bash -c "./client/build/client 127.0.0.1 8888; exit"