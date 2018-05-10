/*
 * Utility.cpp
 *
 *  Created on: 07-Nov-2016
 *      Author: rohit
 */

#include "pastrylib.h"

map<string,string> dataMap;
int globalfd=0;
bool serverStartFlag=false;
bool joinFlag=false;
cell leftLeafSet[2];
cell rightLeafSet[2];
vector<cell> neighbourSet;
cell routingTable[8][4];
cell globalCell;

/*Function for connecting to the node*/
int connectToNode(int argc, string argv[])
{
	int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    if (argc < 3)
    {
       fprintf(stderr,"usage %s hostname port\n", argv[0].c_str());
       return -1;
    }
    portno = atoi(argv[2].c_str());
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        cout << "\nERROR opening socket\n";
        return -1;
    }
    server = gethostbyname(argv[1].c_str());
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        return -1;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        cout << "\nERROR connecting\n";
        return -1;
    }
    else
   	{
   		return sockfd;
   	}
}

/*Function for initializing server*/
int bindPort(string serverPort)
{
	int sockfd,portno;
	struct sockaddr_in serv_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("\nDownload Server:ERROR opening socket\n");
		exit(1);
	}
	portno = atoi(serverPort.c_str());
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("Download Server:ERROR on binding\n");
		exit(1);
	}
	return sockfd;
}

int startServer(int sockfd)
{
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	int newsockfd;
	listen(sockfd, 5);
	while(1)
	{
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		string input=readFromSocket(newsockfd);
		thread(requestController,input,newsockfd).detach();
	}
	return 0;
}

/*Function for fetching the IP*/
string getIP()
{
    setenv("LANG","C",1);
    FILE * fp = popen("ifconfig", "r");
    string ip;
    if (fp)
    {
        char *p=NULL, *e; size_t n;
        while((getline(&p, &n, fp) > 0) && p)
        {
            if (p = strstr(p, "inet "))
            {
                p+=5;
                if (p = strchr(p, ':'))
                {
                    ++p;
                    if (e = strchr(p, ' '))
                    {
                         *e='\0';
                         ip=p;
                         break;
                    }
                }
            }
        }
    }
    pclose(fp);
    return ip;
}

string get_command()
{
	string cmd;
	getline(cin,cmd);
	return cmd;
}

vector<string> parse(string line)
{
	vector<string> cmdVector;
	string buff{""};
	for(auto n:line)
	{
		if(n != ' ')
			buff+=n;
		else if(n == ' ' && buff != "")
		{
			cmdVector.push_back(buff);
			buff = "";
		}
	}
	if(buff != "")
		cmdVector.push_back(buff);
	return cmdVector;
}

int printErrorMessage(string message)
{
	int pid=fork();
	if(pid==0)
	{
		char *red[3];
		red[0]=strdup("tput");
		red[1]=strdup("setf");
		red[2]=strdup("4");
		execvp("tput", red);
	}
	wait(NULL);
	cout<<endl<<message<<endl;
	pid=fork();
	if(pid==0)
	{
		char *white[3];
		white[0]=strdup("tput");
		white[1]=strdup("setf");
		white[2]=strdup("7");
		execvp("tput", white);
		exit(0);
	}
	wait(NULL);
	return 0;
}

int writeIntoSocket(string line,int sockfd)
{
	unsigned int i=0;
	char buffer[1];
	while(i<line.size())
	{
		buffer[0]=line[i++];
		write(sockfd,buffer,1);
	}
	buffer[0]='\0';
	write(sockfd,buffer,1);
	return 0;
}

string readFromSocket(int sockfd)
{
	string temp;
	char buffer[1];
	while (read(sockfd, buffer, 1) && buffer[0] != '\0')
	{
		temp.push_back(buffer[0]);
	}
	return temp;
}
//Computes Hash Value of the given Key
string getHashValue(string line)
{
	string hexHash = md5(line);
	string s = "";
	for(int i= 0; i<4;i++)
	{
		switch(hexHash[i])
		{
			case 'a':
						s = s + "22";
						break;
			case 'b' :
						s = s + "23";
						break;
			case 'c':
						s = s + "30";
						break;
			case 'd':
						s = s + "31";
						break;

			case 'e':
						s = s + "32";
						break;
			case 'f':
						s = s + "33";
						break;
			case '4':
						s = s + "10";
						break;
			case '5':
						s = s + "11";
						break;
			case '6':
						s = s + "12";
						break;
			case '7':
						s = s + "13";
						break;
			case '8':
						s = s + "20";
						break;
			case '9':
						s = s + "21";
						break;
			default:
						s = s + "0" + hexHash[i];
		}

	}
	return s;
}

int num(string s)
{
	if(s.empty())
		return 0;
	int num = 0;
	for(int i=0;i<8;i++)
	{
		num = num * 4 + s[i]-'0';
	}
	return num;
}
