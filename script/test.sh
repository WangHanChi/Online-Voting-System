#!/bin/bash

gnome-terminal -- bash -c "./server/server 8888; exit"
gnome-terminal -- bash -c "./client/client 127.0.0.1 8888; exit"