#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<pthread.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<errno.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/sendfile.h>
#define _SIZE_ 1024
int get_line(int sock,char buf[],int len)
{
	if(!buf||len<0)
		return -1;
	int n=0;
	int i=0;
	char ch='\0';

	while((i<len-1) && ch!='\n')
	{
		n=recv(sock,&ch,1,0);
		if(n>0)
		{
			if(ch=='\r')
			{
				n=recv(sock,&ch,1,MSG_PEEK);
				if(n>0&&ch=='\n')
				{
					recv(sock,&ch,1,0);
				}else
				{
					ch='\n';
				}
			}
			buf[i++]=ch;
		}else
			ch='\n';
	}
	buf[i]='\0';
	return i;
}

void not_found(int sock)
{
	char buf[1024];
	memset(buf,'\0',sizeof(buf));
	sprintf(buf,"HTTP/1.0 404 NOT FOUND\r\n");
	send(sock,buf,strlen(buf),0);
	sprintf(buf,"Content-Type:type/html\r\n\r\n");
	send(sock,buf,strlen(buf),0);
	send(sock,"\r\n",strlen("\r\n"),0);
	sprintf(buf,"<HTML><HEAD><h2>Not Found</h2></HEAD><BODY></BODY>\r\n");
	send(sock,buf,strlen(buf),0);
}

void echo_errno(int sock,int number)
{
//	switch(number)
//	case 404:
//	{
//		not_found(sock);
//		break;
//	}
//	default:
//	{
//		break;
//	}
	not_found(sock);
}

void clear_head(int sock)
{
	int ret=-1;
	char buf[_SIZE_];
	memset(buf,'\0',sizeof(buf));
	do{
		ret=get_line(sock,buf,_SIZE_);
	}while((ret>0) && (strcmp(buf,"\n")!=0));
}

static void  echo_www(int sock,const char* path,ssize_t size)
{
	int fd=open(path,O_RDONLY);
	if(fd<0)
	{
		echo_errno(sock,0);
		return;
	}
	printf("get a new client :%d->%s\n",sock,path);
	char buf[_SIZE_];
	sprintf(buf,"HTTP/1.0 200 OK \r\n\r\n");
	send(sock,buf,strlen(buf),0);
	if(sendfile(sock,fd,NULL,size)<0)
	{
		echo_errno(sock,0);
		return;
	}
	close(fd);
}

static void execut_cgi(int sock,const char* path,const char* method,\
		const char* query_string)
{
	int content_length=-1;
	char buf[_SIZE_];

	int cgi_input[2]={0};
	int cgi_output[2]={0};
	char method_env[_SIZE_];
	char query_string_env[_SIZE_];
	char content_length_env[_SIZE_];
	int ret=-1;
//	printf("method:%s\n",method);
	if(strcasecmp(method,"GET")==0)
	{

		clear_head(sock);
	}
	else//POST
	{

		do{
			ret=get_line(sock,buf,sizeof(buf));
			if(strncasecmp(buf,"Content-Length: ",16)==0)
			{
				content_length=atoi(&(buf[16]));		
			}
		}while((ret>0) && (strcmp(buf,"\n")!=0));
		if(content_length==-1)
		{
			echo_errno(sock,404);
			return ;
		}
	}

	sprintf(buf,"HTTP/1.0 200 OK\r\n\r\n");
	send(sock,buf,strlen(buf),0);
	if(pipe(cgi_input)<0)
	{
		echo_errno(sock,404);
		return;
	}
	if(pipe(cgi_output)<0)
	{
		echo_errno(sock,404);
		return;
	}
	pid_t id=fork();
	if(id == 0)//child
	{

		close(cgi_input[1]);//read from father,close write
		close(cgi_output[0]);//write to father,close read
		dup2(cgi_input[0],0);//close 0,copy cgi_output[0] to 0,return 0
		dup2(cgi_output[1],1);

		printf("child\n");
		sprintf(method_env,"REQUEST_METHOD=%s",method);
		printf("method_env:%s",method_env);
		putenv(method_env);

		if(strcasecmp(method,"GET")==0)
		{
			sprintf(query_string_env,"QUERY_STRING=%s",query_string);
			printf("query_string_env:%s",query_string_env);
			putenv(query_string_env);
		}else
		{
			sprintf(content_length_env,"CONTENT_LENGTH=%s",content_length);
			printf("content_length_env:%s",content_length_env);
			putenv(content_length_env);
		}
		execl(path,path,NULL);
		close(cgi_input[0]);
		close(cgi_output[1]);
		exit(1);
	}else if( id > 0)//father
	{		
		printf("father\n");
		close(cgi_input[0]);
		close(cgi_output[1]);
		int i=0;
		char c='\0';
		if(strcasecmp(method,"POST")==0)
		{
			for(;i<content_length;++i)
			{
				recv(sock,&c,1,0);
				printf("%c ",c);
				write(cgi_input[1],&c,1);
			}
		}
//		printf("\n");
		int ret=0;
		while((ret=read(cgi_output[0],&c,1))>0)
		{
			send(sock,&c,1,0);
		}
		waitpid(id,NULL,0);

		close(cgi_input[1]);
		close(cgi_output[0]);
		close(sock);
	}else
	{
		printf("fork error!\n");
		exit(2);
	}
}


static void* accept_request(void* arg)
{
	int sock=*(int*)arg;
	int ret=0;
	char buf[_SIZE_];
	int len=sizeof(buf)/sizeof(buf[0]);
	char method[_SIZE_/10];
	char url[_SIZE_];
	char path[_SIZE_];
	char cgi=0;
	char* query_string=NULL;
	struct stat st;
	memset(method,'\0',sizeof(method));
	memset(url,'\0',sizeof(url));
	memset(buf,'\0',sizeof(buf));
	memset(path,'\0',sizeof(path));

	ret=get_line(sock,buf,len);
	if(ret<=0)
	{
		echo_errno(sock,404);
		return (void*)1;
	}
	//1.get method
	int i=0;//method index
	int j=0;//buf  index
	while(!isspace(buf[j])&&(i<sizeof(method)-1)&&(j<sizeof(buf)))
	{
		method[i]=buf[j];
		++i,++j;
	}
	method[i]='\0';
	//2.check method
	if(strcasecmp(method,"GET")!=0 && strcasecmp(method,"POST")!=0)
	{
		echo_errno(sock,404);
		return (void*)2;
	}

	//3.get url first step space
	while(isspace(buf[j]))
	{
		++j;
	}
	i=0;
	while(!isspace(buf[j])&&(i<sizeof(url)-1)&&(j<sizeof(buf)))
	{
		url[i]=buf[j];
		++i,++j;
	}
	url[i]='\0';
	if(strcasecmp(method,"POST")==0)
		cgi=1;
//	printf("method : %s,url_path:%s\n",method,url);
	if(strcasecmp(method,"GET")==0)
	{
		query_string=url;
		while(*query_string != '\0' && *query_string !='?')
			query_string++;
		if(*query_string == '?')
		{
			*query_string='\0';
			query_string++;
			cgi=1;
		}
	}
	sprintf(path,"htdoc%s",url);
	if(path[strlen(path)-1]=='/')
	{
		strcat(path,"index.html");
	}

	if(stat(path,&st)<0)
	{
		echo_errno(sock,0);
		return (void*)3;
	}else
	{

		if(S_ISDIR(st.st_mode))
		{
			strcat(path,"index.html");
		}else if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
		{
			cgi=1;
		}else
		{}
		if(cgi)//POST
		{
//			printf("query_string:%s\n",query_string);
			execut_cgi(sock,path,method,query_string);
		}else//GET
		{
			clear_head(sock);
			echo_www(sock,path,st.st_size);
		}
	}
	close(sock);//face NOT_LINK
	return (void*)0;
}

static void set_sock_noblock(int sock)
{
	int flag;
	flag=fcntl(sock,F_GETFL,0);
	if(flag<0)
	{
		perror("fcntl(F_GETFL)");
		exit(5);
	}
	if(fcntl(sock,F_SETFL,flag|O_NONBLOCK)<0)
	{
		perror("fcntl(F_SETFL)");
		exit(6);
	}
}
static int startup(const char* _ip,int _port)
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		perror("socket()");
		exit(2);
	}

	int opt=1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));


	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(_port);
	local.sin_addr.s_addr=inet_addr(_ip);

	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		perror("bind()");
		exit(3);
	}
	if(listen(sock,5)<0)
	{
		perror("listen()");
		exit(4);
	}

//	set_sock_noblock(sock);
	printf("server is running at port %d\n",_port);
	return sock;

}

static void Useage(const char* proc)
{
	printf("Useage:%s,[ip][port]\n",proc);
}
int main(int argc,char* argv[])
{
	if(argc!=3)
	{
		Useage(argv[0]);
		exit(1);
	}
	int listen_sock=startup(argv[1],atoi(argv[2]));
	struct sockaddr_in peer;
	socklen_t len=sizeof(peer);
	while(1)
	{
		int new_fd=accept(listen_sock,(struct sockaddr*)&peer,&len);
		if(new_fd>0)
		{
			pthread_t id;
			pthread_create(&id,NULL,accept_request,(void*)&new_fd);
			pthread_detach(id);
		}
	}
	return 0;
}
