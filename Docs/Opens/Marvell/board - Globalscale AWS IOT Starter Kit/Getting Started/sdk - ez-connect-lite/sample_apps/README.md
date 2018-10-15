Create new application code
====

To create a new application called ```my-new-app```, follow these steps :

* Create a directory in *sample_apps* (or any other directory) called ```my-new-app```

   		  mkdir my-new-app
2. In this directory, create a ```build.mk```. Copy the following into
this file


		# Copyright (C) 2008-2016 Marvell International Ltd.
		# All Rights Reserved.
		#

		exec-y += my-new-app
		my-new-app-objs-y := src/main.c
		my-new-app-cflags-y := -I$(d)/src -DAPPCONFIG_DEBUG_ENABLE=1

		# Applications could also define custom linker files if required using following:
		#my-new-app-ld-y := /path/to/ldscript
		
		# Applications could also define custom board files if required using following:
		#my-new-app-board-y := /path/to/boardfile
* Put your source code in a directory called *src*

		$ tree
		.
		├── build.mk
		└── src
		    ├── aws_starter_root_ca_cert.h
		    └── main.c
		1 directory, 3 files

* From the root directory, build the your new app

		make APP=sample_apps/my-new-app
