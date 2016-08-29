#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include"bigdata.cpp"
const int _SIZE_=1024;

void bigdata_math(char* data)
{
	char* data1;
	char* data2;
	char* flag;
	while(*data!='=')
		data++;
	data++;
	data1=data;
	while(*data!='&')
		data++;
	*data++='\0';
	while(*data!='=')
		data++;
	data++;
	data2=data;
	while(*data!='&')
		data++;
	*data++='\0';
	flag=data;
	BigData B_data1(data1);
	BigData B_data2(data2);
	BigData ret("0");
	switch(*flag)
	{
		case 'A':
			ret=B_data1+B_data2;
			break;
		case 'S':
			ret=B_data1-B_data2;
			break;
		case 'M':
			ret=B_data1*B_data2;
			break;
		case 'D':
			ret=B_data1/B_data2;
			break;
		default:
			break;
	}
	cout<<"<h>"<<"This reslut is : "<<ret<<"</h>"<<endl;
}

int main()
{
	int content_len=-1;
	char* tmp;
	char method[_SIZE_];
	char query_string[_SIZE_];
	char post_data[4*_SIZE_];
	memset(query_string,'\0',sizeof(query_string));
	memset(post_data,'\0',sizeof(post_data));
	printf("<html>\n");
	printf("<body>\n");

	tmp=getenv("REQUEST_METHOD");
	if(tmp==NULL)
		exit(1);
	strcpy(method,tmp);
	
	printf("<p>method : %s</p><br/>",method);
	if(strcasecmp(method,"GET")==0)
	{
		tmp=getenv("QUERY_STRING");
		if(tmp!=NULL)
			strcpy(query_string,tmp);
		bigdata_math(query_string);
	}else if(strcasecmp(method,"POST")==0)
	{
		tmp=getenv("CONTENT_LENGTH");
		if(tmp==NULL)
			exit(1);
		content_len=atoi(tmp);
		int i=0;
		for(;i<content_len;++i)
		{
			read(0,&post_data[i],1);
		}
		post_data[i]='\0';
		bigdata_math(post_data);
	}
	printf("</body>\n");
	printf("</html>\n");
	return 0;
}
