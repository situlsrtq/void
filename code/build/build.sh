#!/bin/bash

g++ -g src/glad.c src/test.cpp src/shader.cpp -Iinclude -lglfw -lGL -o test
