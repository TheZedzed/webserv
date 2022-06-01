# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <arpa/inet.h>
# include <sys/epoll.h>
# include <fcntl.h>
# include <unistd.h>
# include <signal.h>
# include <fstream>
# include <sstream>
# include <errno.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>


# include <iostream>
# include <cstring>
# include <cstdio>
# include <map>
# include <set>
# include <vector>
# include <algorithm>

int main(void)
{
	std::vector<const char*> envp;
	char *env[] =  {"REQUEST_METHOD=POST", "CONTENT_TYPE=application/x-www-form-urlencoded", }
	envp.push_back(std::string("REQUEST_METHOD=POST").c_str());
	envp.push_back(std::string("CONTENT_TYPE=application/x-www-form-urlencoded").c_str());
	envp.push_back(std::string("HTTP_COOKIE=").c_str());
	envp.push_back(std::string("CONTENT_LENGTH=32").c_str());
	envp.push_back(std::string("QUERY_STRING=").c_str());
	envp.push_back(std::string("PATH_INFO=submit.php").c_str());
	envp.push_back(std::string("DOCUMENT_URI=submit.php").c_str());
	envp.push_back(std::string("REQUEST_URI=submit.php?").c_str());
	envp.push_back(std::string("PATH_TRANSLATED=/home/tombanks/Desktop/42/webserv/www/form/submit.php").c_str());
	envp.push_back(std::string("SCRIPT_NAME=/home/tombanks/Desktop/42/webserv/www/form/submit.php").c_str());
	envp.push_back(std::string("SCRIPT_FILENAME=/home/tombanks/Desktop/42/webserv/www/form/submit.php").c_str());
	envp.push_back(std::string("PWD=/home/tombanks/Desktop/42/webserv").c_str());
	for(std::vector<const char*>::iterator it; it != envp.end(); it++)
		std::cout << "var: " << *it;
	char *argv [] = {(char*)"cgi", (char*)"/home/tombanks/Desktop/42/webserv/www/form/submit.php", NULL};
	int fd = open("/tmp/tempfile", O_WRONLY | O_CREAT , 0644);
	write(fd, "feild_one=first&feild_two=second", 32);
	close(fd);
	fd = open("/tmp/tempfile", O_WRONLY | O_CREAT , 0644);
	pid_t pid = fork();
	if (pid == -1)
		exit(0);
	else if (pid == 0)
	{
		if (dup2(fd, STDIN_FILENO) == -1)
			std::cout << "error\n";

		//dup2(out, STDOUT_FILENO);
		//close(fd);;
		//execve("/usr/bin/cat", argv, NULL );
		execve("/usr/bin/php-cgi8.1", argv, (char**)envp.begin().base());
		printf("error\n");
		exit(1);
	}
	//close(fd);
	int status;
	close(fd);
	wait(&status);
	std::cout << "finish\n";
	//int code = WEXITSTATUS(status);


}
