/*****************************************************************************\
*                     EFSL - Embedded Filesystems Library                     *
*                     -----------------------------------                     *
*                                                                             *
* Filename : cpi.c                                                            *
* Release  : 0.3 - devel                                                      *
* Description : This file is part of the linux utilities, main purpose        *
*               is to test and debug the library                              *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; version 2                     *
* of the License.                                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* As a special exception, if other files instantiate templates or             *
* use macros or inline functions from this file, or you compile this          *
* file and link it with other works to produce a work based on this file,     *
* this file does not by itself cause the resulting work to be covered         *
* by the GNU General Public License. However the source code for this         *
* file must still be made available in accordance with section (3) of         *
* the GNU General Public License.                                             *
*                                                                             *
* This exception does not invalidate any other reasons why a work based       *
* on this file might be covered by the GNU General Public License.            *
*                                                                             *
*                                                    (c)2006 Lennart Yseboodt *
*                                                    (c)2006 Michael De Nil   *
\*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "efs.h"

int main(int argc, char** argv)
{
	EmbeddedFileSystem efs;
	EmbeddedFile file;	
	unsigned short e;
	unsigned short bufsize;
	signed short ret;
	char *buf;
	FILE *localfile;
	
	if(argc<4){
		fprintf(stderr,"Argument error : cpi <fs> <local_read> <file_write> [bufsize]\n");
		exit(-1);
	}

	if(argc==5)
		bufsize=atoi(argv[4]);
	else
		bufsize=4096;
	buf=malloc(bufsize);
	
	if(efs_init(&efs,argv[1])!=0){
		printf("Could not open filesystem.\n");
		return(-1);
	}
	
	ret=file_fopen(&file,&efs.myFs,argv[3],'w');
	if((ret!=0)){
		printf("Could not open file %s (return value: %d).\n",argv[3],ret);
		return(-2);
	}

	if(!(localfile=fopen(argv[2],"r"))){
		printf("Could not open local file.\n");
		return(-3);
	}
	
	while((e=fread(buf,1,bufsize,localfile))){
		file_write(&file,e,buf);
	}

	file_fclose(&file);
	fclose(localfile);
	fs_umount(&(efs.myFs));

	return(0);
}
