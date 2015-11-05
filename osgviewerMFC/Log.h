#include <stdio.h>
#include <time.h>

enum eLogLevel
{
	LOG_NONE = 0,
	LOG_ERROR = 1,
	LOG_WARNING = 2;
	LOG_INFO = 3,
	LOG_DEBUG = 4,
};

#define LOG_OUTPUT(str ,level, ...) log_append_to_file("c:\\test.txt", str, eLogLevel::LOG_INFO, __FILE__, __LINE__ );


#define INFO_LOG( str) LOG_OUTPUT( str, eLogLevel::LOG_INFO );
#define WARNING_LOG( str) LOG_OUTPUT( str, eLogLevel::LOG_WARNING );
#define ERROR_LOG( str) LOG_OUTPUT( str, eLogLevel::LOG_ERROR );
#define DEBUG_LOG( str) LOG_OUTPUT( str, eLogLevel::LOG_DEBUG );

void log_append_to_file(char* filename,char* str, const  eLogLevel logLevel, char* sourceFile,int fileLine)
{
	time_t t;
	time(&t);
	struct tm* tp= localtime(&t);
	printf("%x\n",tp);
	char now_str[100];
	strftime(now_str, 100, "%Y-%m-%d %H:%M:%S", tp);

	FILE *fo;
	fo = fopen(filename, "a");
	if (fo == 0) 
	{ 
		return;
	}

	std::string  eLogInfo("");
	if(logLevel ==0)
	{
		eLogInfo = "LOG_NONE";
	}
	else if(logLevel ==1)
	{
		eLogInfo = "LOG_ERROR";
	}
	else if(logLevel ==2)
	{
		eLogInfo = "LOG_WARNING";
	}
	else if(logLevel ==3)
	{
		eLogInfo = "LOG_INFO";
	}
	else if(logLevel ==4)
	{
		eLogInfo = "LOG_DEBUG";
	}

	fprintf(fo, "%s %s(:%d):%s_%s\r\n",now_str,sourceFile,fileLine, logLevel, str);
	fclose(fo);
}
