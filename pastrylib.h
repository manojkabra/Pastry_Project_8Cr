/*
 * pastrylib.h
 *
 *  Created on: 07-Nov-2016
 *      Author: rohit
 */

#ifndef PASTRYLIB_H_
#define PASTRYLIB_H_

#include <bits/stdc++.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <map>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <netdb.h>
#include <string.h>
#include <sstream>
#include <arpa/inet.h>
#include <thread>
#include "md5.h"


using namespace std;

extern map<string,string> dataMap;
typedef struct cell
{
	string ipAddress;
	string port;
	string nodeId;
}cell;


extern cell globalCell;
extern int globalfd;
extern bool serverStartFlag;
extern bool joinFlag;
extern cell leftLeafSet[2];
extern cell rightLeafSet[2];

extern vector<cell> neighbourSet;

extern cell routingTable[8][4];

string get_command();
string getHashValue(string line);
int execute(string cmd);
vector<string> parse(string line);
int connectToNode(int argc, string argv[]);
int writeIntoSocket(string line,int sockfd);
string readFromSocket(int sockfd);
int bindPort(string serverPort);
int startServer(int sockfd);
int updateKeys();
string getIP();
int updateRouteT();
int updateMap(string line);
int parseLeafInput(string line);
string getRowString(string rowNo);
int updateRouteTable(string nodeId,string rowString);
int sendUpdateLeafRequest();
int getDifference(string nodeId1,string nodeId2);
string parseRouteInput(string row,int rowNo);
int requestController(string input,int sockfd);
int executePutRequest(string input,int sockfd);
int executeGetRequest(string input,int sockfd);
int executeDataCmd(vector<string> cmdVector);
int executeUpdateLeaf(string input,int sockfd);
int executeRowRequest(string input,int sockfd);
int executeLeafRequest(string input,int sockfd);
int executeKeyRequest(string input,int sockfd);
int executePortCmd(vector<string> cmdVector);
int executeCreateCmd(vector<string> cmdVector);
int executePutCmd(vector<string> cmdVector);
int executeGetCmd(vector<string> cmdVector);
int executeJoinCmd(vector<string> cmdVector);
int executeLsetCmd(vector<string> cmdVector);
int executeNsetCmd(vector<string> cmdVector);
int executeRouteCmd(vector<string> cmdVector);
int executeQuitCmd(vector<string> cmdVector);
int executeShutDownCmd(vector<string> cmdVector);
int printErrorMessage(string message);
cell MaxLeafSetFind(string NodeID, vector<cell> &LeafSet);
cell LeafSetFind(string NodeID, vector<cell> &LeafSet);
int putRequest(string key, string value);
string wrapperGetRequest(string key);
int GetRequest(string NodeID, cell &temp);
void LeafSort(vector<cell> &LeafSet);
cell RouteTableFind(string NodeID);
int num(string s);

#endif /* PASTRYLIB_H_ */
