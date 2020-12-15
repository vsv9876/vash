#!/usr/bin/bash
# this is vash rc profile

# std.cmd

# included sections:
# shell (bash) command

-1!sh

# switch context to command editing, old content will be preserved
#:EX
:CA
:HE
:[1!]
	* * _sh

:[./a-z]
#       любая клавиша нижнего регистра - начало ввода команды sh.
	* *     _sh #k

:^Z
# Приостановить ash, вернуться в csh
#        * *     :/bin/csh -cf "suspend; exit 0;"
	* *     _sus
