#include <stdio.h>
#include <time.h>
#include <fcntl.h>

char data[1024*1024];

// sync the data just writted to hd
// and force kernel to drop all cahced files -- to make the read test fair
void syncAndDropCache(void)
{
	int fd;
	ssize_t s;

	sync();
	fd = open("/proc/sys/vm/drop_caches",O_WRONLY);
	if(fd<0){
		printf("open vm failed\n");
		return;
	}
	s = write(fd,"3",2);
	if(s<0){
		printf("write vm failed\n");
		close(fd);
		return;
	}

	close(fd);
	sleep(2);
}

void write100M(void)
{
	int i;
	int fd;
	ssize_t s;

	fd = open("/sdcard/testdata",O_RDWR | O_CREAT);
	if(fd<0) {
		printf("open /sdcard/testdata failed\n");
		return ;
	}

	for(i=0;i<100;i++){
		s = write(fd,&data,sizeof(data));
		if(s<0){
			printf("write failed %d\n",i);
			close(fd);
			return ;
		}
	}
	sync();
	close(fd);
}

void read100M(void)
{
	int i;
	int fd;
	ssize_t s;
	ssize_t total;
	
	fd = open("/sdcard/testdata",O_RDONLY);
	if(fd<0) {
		printf("open /sdcard/test failed\n");
		return ;
	}

	total = 100 * 1024 * 1024;

	while(total > 0){
		s = read(fd,&data,sizeof(data));
		if(s<0){
			printf("read failed %d\n",i);
			close(fd);
			return ;
		}
		total -= s;
	}

	close(fd);
}

void printSpeed(int MB,struct timespec Start, struct timespec End)
{
	int deltat;
	long deltanano;
	double speed;
	double dtime;

	deltat    = End.tv_sec  - Start.tv_sec;
	deltanano = End.tv_nsec - Start.tv_nsec;

	if(deltanano < 0){
		deltanano += 1000000000;
		deltat -=1;
	}
	//printf("%ld  %ld\n",deltat, deltanano);

	dtime = deltat + deltanano/1000000000.;
	printf("time: %lf sec    ",dtime);
	speed = MB / dtime;
	printf("speed :%lf MB/s\n",speed);
}

int main(int argc,char *argv[])
{

	struct timespec StartTime,EndTime;

	memset(&data,0xAA,sizeof(data));


	printf("sdwrite test\n");
	clock_gettime(CLOCK_MONOTONIC, &StartTime);

	write100M();

	clock_gettime(CLOCK_MONOTONIC, &EndTime);
	//printf("%ld %ld\n",StartTime.tv_sec,StartTime.tv_nsec);
	//printf("%ld %ld\n",  EndTime.tv_sec,  EndTime.tv_nsec);
	printSpeed(100,StartTime,EndTime);

	syncAndDropCache();

	printf("sdread test\n");
	clock_gettime(CLOCK_MONOTONIC, &StartTime);

	read100M();

	clock_gettime(CLOCK_MONOTONIC, &EndTime);
	//printf("%ld %ld\n",StartTime.tv_sec,StartTime.tv_nsec);
	//printf("%ld %ld\n",  EndTime.tv_sec,  EndTime.tv_nsec);
	printSpeed(100,StartTime,EndTime);


	return 0;
}
