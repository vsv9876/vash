#!/usr/bin/vash
#
# this is vash rc profile included sections:
# shell (bash) command
#
|# -1 #
|$ -1 $

#
:RE
	* * ;clear
###
### experiment...
#:$
#	* * _rchelp __HELPRC__

# switch context to command editing, old content will be preserved
:CA
:[1!]
	* * _sh
# begin an unix command from scratch
:[./a-z]
	* *     _sh #k
:^Z
#        * *     :/bin/csh -cf "suspend; exit 0;"
	* *     _sus

### EOF
