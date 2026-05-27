#!/usr/bin/vash
#
# this is vash rc profile included sections:
# shell command
#
|# -1 #
|$ -1 $

# repeat last command performed not limited by shell comand
##-@ repeat last cmd
##:2
:@
	* *	_shrpt

###
### experiment...
#:$
#	* * _rchelp __HELPRC__

# switch context to command editing, old content will be preserved
:CA
:[1!]
	* * _sh
	* * 
# begin new command from scratch
:[./a-z]
	* *     _sh #k

### EOF
