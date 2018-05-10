/*
 * Execution.cpp
 *
 *  Created on: 08-Nov-2016
 *      Author: rohit
 */

#include "pastrylib.h"

/*Controller method for executing different commands */
int execute(string cmd)
{
	vector<string> cmdVector=parse(cmd);
	if(!cmdVector.empty())
	{
		if(cmdVector[0]=="port")
		{
			executePortCmd(cmdVector);
		}
		else if(cmdVector[0]=="create")
		{
			executeCreateCmd(cmdVector);
		}
		else if(cmdVector[0]=="lset")
		{
			executeLsetCmd(cmdVector);
		}
		else if(cmdVector[0]=="nset")
		{
			executeNsetCmd(cmdVector);
		}
		else if(cmdVector[0]=="data")
		{
			executeDataCmd(cmdVector);
		}
		else if(cmdVector[0]=="join")
		{
			executeJoinCmd(cmdVector);
		}
		else if(cmdVector[0]=="put")
		{
			executePutCmd(cmdVector);
		}
		else if(cmdVector[0]=="get")
		{
			executeGetCmd(cmdVector);
		}
		else if(cmdVector[0]=="routetable")
		{
			executeRouteCmd(cmdVector);
		}
		else if(cmdVector[0]=="quit")
		{
			executeQuitCmd(cmdVector);
		}
		else if(cmdVector[0]=="shutdown")
		{
			executeShutDownCmd(cmdVector);
		}
		else
			printErrorMessage("No such command found");
	}
	return 0;
}

/*Function for port command*/
int executePortCmd(vector<string> cmdVector)
{
	if(globalfd!=0)
		printErrorMessage("Port already binded");
	if(cmdVector.size()==2)
	{
		globalfd=bindPort(cmdVector[1]);
		globalCell.port=cmdVector[1];
		globalCell.ipAddress=getIP();
		globalCell.nodeId = getHashValue(globalCell.ipAddress+":"+globalCell.port);
	}
	else
		printErrorMessage("Invalid arguments for ports");
	return 0;
}

/*Function for creating a node*/
int executeCreateCmd(vector<string> cmdVector)
{
	if(cmdVector.size()>1)
		printErrorMessage("Invalid arguments for create");
	else if(globalfd==0)
		printErrorMessage("Create won't work before port command");
	else if(serverStartFlag)
		printErrorMessage("Node already created");
	else
	{
		updateRouteT();
		for(int i=0;i<2;i++)
		{
			cell leftNode,rightNode;
			leftNode.ipAddress="";
			leftNode.port="";
			leftNode.nodeId="";
			rightNode.ipAddress="";
			rightNode.port="";
			rightNode.nodeId="";
			leftLeafSet[i]=leftNode;
			rightLeafSet[i]=rightNode;
		}
		thread(startServer,globalfd).detach();
		serverStartFlag=true;
	}
	return 0;
}

/*Function for put request*/
int executePutCmd(vector<string> cmdVector)
{
	if(joinFlag)
	{
		string message="1$"+getHashValue(cmdVector[1])+","+cmdVector[2];
		string argv[3];
		argv[1]=globalCell.ipAddress;
		argv[2]=globalCell.port;
		int sockfd=connectToNode(3,argv);
		writeIntoSocket(message,sockfd);
		close(sockfd);
	}
	else
		printErrorMessage("Node has not joined the network");
	return 0;
}

/*Function for get request*/
int executeGetCmd(vector<string> cmdVector)
{
	if(joinFlag)
	{
		string requestMessage="2$"+getHashValue(cmdVector[1]);
		string message;
		string argv[3];
		argv[1]=globalCell.ipAddress;
		argv[2]=globalCell.port;
		int sockfd=connectToNode(3,argv);
		writeIntoSocket(requestMessage,sockfd);
		message=readFromSocket(sockfd);
		close(sockfd);
		while(message[0]=='1')
		{
			string temp=message.substr(2);
			if(temp=="NULL")
			{
				printErrorMessage("No such key found");
				break;
			}
			else
			{
				int breakIndex=temp.find(",");
				argv[1]=temp.substr(0,breakIndex);
				temp=temp.substr(breakIndex+1);
				breakIndex=temp.find(",");
				argv[2]=temp.substr(0,breakIndex);
				sockfd=connectToNode(3,argv);
				writeIntoSocket(requestMessage,sockfd);
				message=readFromSocket(sockfd);
				close(sockfd);
			}
		}
		if(message[0]=='2')
		{
			cout<<endl<<"The value of "<<cmdVector[1]<<" is "<<message.substr(2)<<endl;
		}
	}
	else
		printErrorMessage("Node has not joined the network");
	return 0;
}

/*Function for joining node to a network*/
int executeJoinCmd(vector<string> cmdVector)
{
	if(joinFlag)
		printErrorMessage("Node has already joined the network");
	else if(serverStartFlag)
	{
		string argv[3];
		argv[1]=cmdVector[1];
		argv[2]=cmdVector[2];
		int sockfd;
		for(int i=0;i<8;i++)
		{
			sockfd=connectToNode(3,argv);
			char c=i+'0';
			string temp="4$";
			temp.push_back(c);
			temp=temp+"$"+globalCell.ipAddress+","+globalCell.port+","+globalCell.nodeId;
			writeIntoSocket(temp,sockfd);
			temp=readFromSocket(sockfd);
			close(sockfd);
			temp=parseRouteInput(temp,i);
			if(!temp.empty())
			{
				vector<string> nodeInfo=parse(temp);
				if(nodeInfo[0]==globalCell.ipAddress && nodeInfo[1]==globalCell.port)
					break;
				argv[1]=nodeInfo[0];
				argv[2]=nodeInfo[1];
			}
			else
			{
				sockfd=connectToNode(3,argv);
				temp="5";
				writeIntoSocket(temp,sockfd);
				temp=readFromSocket(sockfd);
				close(sockfd);
				if(!temp.empty())
					temp=temp+"$";
				temp=temp+argv[1]+","+argv[2]+","+getHashValue(argv[1]+":"+argv[2]);
				parseLeafInput(temp);
				sendUpdateLeafRequest();
				updateKeys();
				break;
			}
		}
		joinFlag=true;
	}
	else
		printErrorMessage("Node is not created");
	return 0;
}

/*Function for printing the leaf set*/
int executeLsetCmd(vector<string> cmdVector)
{
	if(cmdVector.size()==1)
	{
		cout<<endl<<"Leaf set :\n"<<endl;
		for(int i=1;i>=0;i--)
		{
			if(leftLeafSet[i].ipAddress.empty())
				cout<<"--------"<<"\t";
			else
				cout<<leftLeafSet[i].nodeId<<"\t";
		}
		cout<<globalCell.nodeId<<"\t";
		for(int i=0;i<2;i++)
		{
			if(rightLeafSet[i].ipAddress.empty())
				cout<<"--------"<<"\t";
			else
				cout<<rightLeafSet[i].nodeId<<"\t";
		}
		cout<<endl;
	}
	else
		printErrorMessage("Invalid arguments for leaf set");
	return 0;
}

/*Function for printing the neighbour set*/
int executeNsetCmd(vector<string> cmdVector)
{
	if(cmdVector.size()==1)
	{
		cout<<endl<<"Neighbour set :\n"<<endl;
		for(unsigned int i=0;i<neighbourSet.size();i++)
		{
			cout<<neighbourSet[i].nodeId<<"\t";
		}
	}
	else
		printErrorMessage("Invalid arguments for neighbour set");
	return 0;
}

/*Function for printing the neighbour set*/
int executeDataCmd(vector<string> cmdVector)
{
	if(cmdVector.size()==1)
	{
		cout<<endl<<"Data Map :\n"<<endl;
		for(map<string,string>::iterator it=dataMap.begin();it!=dataMap.end();it++)
		{
			cout<<it->first<<"-->"<<it->second<<endl;
		}
		cout<<endl;
	}
	else
		printErrorMessage("Invalid arguments for neighbour set");
	return 0;
}

/*Function for printing the route table*/
int executeRouteCmd(vector<string> cmdVector)
{
	if(cmdVector.size()==1)
	{
		cout<<endl<<"Routing Table\n"<<endl;
		for(int i=0;i<8;i++)
		{
			for(int j=0;j<4;j++)
			{
				if(routingTable[i][j].ipAddress.empty())
					cout<<"--------"<<"\t";
				else
					cout<<routingTable[i][j].nodeId<<"\t";
			}
			cout<<endl;
		}
	}
	else
		printErrorMessage("Invalid arguments for route table");
	return 0;
}

/*Function for quiting the network*/
int executeQuitCmd(vector<string> cmdVector)
{
	exit(0);
	return 0;
}

/*Function for shutting down the network*/
int executeShutDownCmd(vector<string> cmdVector)
{
	exit(0);
	return 0;
}

