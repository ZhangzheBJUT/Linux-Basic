#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>



typedef struct {
    int integer;
	char string[24];
}RECORD;

#define NRECORDS (100)

int main(int argc,char**argv)
{
    RECORD record,*mapped;
	int i,f;
	FILE *fp;

	//打开初始化文件
	fp = fopen("record.dat","w+");
	for (i=0;i<NRECORDS;i++)
	{
		record.integer = i;
		sprintf(record.string,"RECORD-%d",i);
		fwrite(&record,sizeof(record),1,fp);
	}
	fclose(fp);

	//把第43条记录中的整数值由43修改为143,并把它写入第43条记录中的字符串
	fp = fopen("record.dat","r+");
	fseek(fp,43*sizeof(record),SEEK_SET);
	fread(&record,sizeof(record),1,fp);

	record.integer = 143;
	sprintf(record.string,"RECORD-%d",record.integer);

	fseek(fp,43*sizeof(record),SEEK_SET);
    fwrite(&record,sizeof(record),1,fp);
    fclose(fp);

    //把这些记录映射到内存中，然后访问第43条记录，把它的整数值修改为243
	f = open("record.dat",O_RDWR);
	mapped = (RECORD*)mmap(0,NRECORDS*sizeof(record),PROT_READ|PROT_WRITE,MAP_SHARED,f,0);
	mapped[43].integer = 243;
	sprintf(mapped[43].string,"RECORD-%d",mapped[43].integer);

	msync((void*)mapped,NRECORDS*sizeof(record),MS_ASYNC);
	munmap((void*)mapped,NRECORDS*sizeof(record));
	close(f);

	exit(0);

	return 0;
}

