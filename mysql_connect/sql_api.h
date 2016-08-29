#pragma once
#include<iostream>
#include<stdio.h>
#include<string.h>
#include"mysql/mysql.h"
#include<stdlib.h>

const int _SIZE_=1024;

class sql_api
{
	public:
		sql_api(const std::string &_host="127.0.0.1",\
				const std::string &_user="root",\
				const std::string &_passwd="",\
				const std::string &_db="httpd");
		int my_connect_mysql();
		int my_insert(std::string &cols,std::string &data);
		int my_select();
		~sql_api();
	private:
		MYSQL *conn;
		MYSQL_RES *res;
		std::string host;
		std::string user;
		std::string passwd;
		std::string db;
		int port;
};
