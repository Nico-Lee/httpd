#include"sql_api.h"

void my_select()
{
	sql_api* sql=new sql_api;
	sql->my_connect_mysql();
	sql->my_select();
//	std::string field_name[10];
//	std::string _out_data[1024][5];
//	int row;
//	conn.sql_select(field_name,_out_data,row,sql);
//	std::cout<<"<table border=\"1\">"<<std::endl;
//	std::cout<<"<tr>"<<std::endl;
//	for (int i = 0;i<row;i++)
//	{
//		std::cout<<"<th>"<<field_name[i]<<"</th>"<<std::endl;
//	}	
//	for (int i = 0;i<row;i++)
//	{
//		std::cout<<"<tr>"<<std::endl;
//		for(int j =0;j<5;j++)
//			std::cout<<"<td>"<<_out_data[i][j]<<"</td>"<<std::endl;
//		std::cout<<"</tr>"<<std::endl;
//	}
//	std::cout<<"</body>"<<"</html>"<<std::endl;
	delete sql;
	sql=NULL;
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
	printf("method:%s",method);
	if (strcasecmp(method,"GET") == 0)
	{
		tmp = getenv("QUERY_STRING");
		if (tmp != NULL)
			strcpy(query_string,tmp);
		char *sql = query_string;
		while(*sql != '=')
			sql++;
		sql++;
		if (*sql == '\0')
			sql = default_sql;
		my_select();
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
		my_select();
	}
	return 0;
}

