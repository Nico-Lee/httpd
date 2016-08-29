#include"sql_api.h"

void my_insert(char* arg)
{
	sql_api* sql=new sql_api;
	std::string cols ="(company_name,company_addr,company_information)";
	char* buf[4];
	buf[3]=NULL;
	int index=2;

	char* end=arg+strlen(arg)-1;
	while(end>arg)
	{
		if(*end =='=')
		{
			buf[index--]=end+1;
		}
		if(*end=='&')
		{
			*end='\0';
		}
		end--;
	}
	std::string data="(\"";
	data+=buf[0];
	data+="\",\"";
	data+=buf[2];
	data+="\")";
	sql->my_connect_mysql();
	sql->my_insert(cols,data);
	delete sql;
	std::cout<<"client info : "<<mysql_get_client_info()<<std::endl;

}

int main()
{
	int content_length = -1;
	char query_string[_SIZE_];
	char method[_SIZE_];
	char post_data[4*_SIZE_];
	char *default_sql =(char*)"SELECT * FROM company_info";
	bzero(query_string,sizeof(query_string));
	bzero(method,sizeof(method));
	bzero(post_data,sizeof(post_data));
	std::cout<<"<html>"<<std::endl;
	std::cout<<"<body>"<<std::endl;
	char *tmp = getenv("REQUEST_METHOD");
	if (tmp != NULL)
		strcpy(method,tmp);
	if (strcasecmp(method,"GET") == 0)
	{

		std::cout<<"*********************";
		tmp = getenv("QUERY_STRING");
		if (tmp != NULL)
			strcpy(query_string,tmp);
		char *sql = query_string;
		while(*sql != '=')
			sql++;
		sql++;
		if (*sql == '\0')
			sql = default_sql;
		my_insert(sql);
	}
	if (strcasecmp(method,"POST") == 0)
	{
		tmp = getenv("CONTENT_LENGTH");
		if (tmp != NULL)
		{
			content_length = atoi(tmp);
		}
		if (content_length != -1)
		{
			int i = 0;
			for (;i<content_length;i++)
			{
				read(0,&post_data[i],1);
			}
			post_data[i] = '\0';
		}
		char *sql = post_data;
		while(*sql != '=')
			sql++;
		sql++;
		if (*sql == '\0')
			sql = default_sql;
		my_insert(sql);
	}
	return 0;
}
