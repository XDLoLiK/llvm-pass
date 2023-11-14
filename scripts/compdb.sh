#!/bin/bash

compiledb make all
compdb -p . list > compile_commands_.json
mv compile_commands_.json compile_commands.json
