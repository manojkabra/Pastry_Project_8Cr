/*
 * Helper.cpp
 *
 *  Created on: 09-Nov-2016
 *      Author: rohit
 */

#include "pastrylib.h"

string parseRouteInput(string row,int rowNo)
{
	string destination;
	int index=row.find("$");
	int columnIndex=0;
	while(index>0)
	{
		string temp=row.substr(0,index);
		cell node;
		if(!temp.empty() && temp!="NULL")
		{
			int breakIndex=temp.find(",");
			node.ipAddress=temp.substr(0,breakIndex);
			temp=temp.substr(breakIndex+1);
			breakIndex=temp.find(",");
			node.port=temp.substr(0,breakIndex);
			temp=temp.substr(breakIndex+1);
			breakIndex=temp.find(",");
			node.nodeId=temp.substr(0,breakIndex);
		}
		else
		{
			node.ipAddress="";
			node.port="";
			node.nodeId="";
		}
		routingTable[rowNo][columnIndex]=node;
		row=row.substr(index+1);
		index=row.find("$");
		columnIndex++;
	}
	string temp=row.substr(0,index);
	cell node;
	if(!temp.empty() && temp!="NULL")
	{
		int breakIndex=temp.find(",");
		node.ipAddress=temp.substr(0,breakIndex);
		temp=temp.substr(breakIndex+1);
		breakIndex=temp.find(",");
		node.port=temp.substr(0,breakIndex);
		temp=temp.substr(breakIndex+1);
		breakIndex=temp.find(",");
		node.nodeId=temp.substr(0,breakIndex);
	}
	else
	{
		node.ipAddress="";
		node.port="";
		node.nodeId="";
	}
	routingTable[rowNo][columnIndex]=node;
	cell dest=routingTable[rowNo][globalCell.nodeId[rowNo]-'0'];
	if(!dest.ipAddress.empty())
	{
		destination=dest.ipAddress+" "+dest.port;
	}
	dest.ipAddress=globalCell.ipAddress;
	dest.port=globalCell.port;
	dest.nodeId=globalCell.nodeId;
	routingTable[rowNo][globalCell.nodeId[rowNo]-'0']=dest;
	return destination;
}

int updateRouteT()
{
	for(unsigned int i=0;i<globalCell.nodeId.size();i++)
	{
		cell dest;
		dest.ipAddress=globalCell.ipAddress;
		dest.port=globalCell.port;
		dest.nodeId=globalCell.nodeId;
		routingTable[i][globalCell.nodeId[i]-'0']=dest;
	}
	return 0;
}

bool cmp(const cell & lhs, const cell & rhs)
{
  return num(lhs.nodeId) < num(rhs.nodeId);
}

int parseLeafInput(string row)
{
	string destination;
	int index=row.find("$");
	vector<cell> list;
	while(index>0)
	{
		string temp=row.substr(0,index);
		cell node;
		if(temp!="NULL")
		{
			int breakIndex=temp.find(",");
			node.ipAddress=temp.substr(0,breakIndex);
			temp=temp.substr(breakIndex+1);
			breakIndex=temp.find(",");
			node.port=temp.substr(0,breakIndex);
			temp=temp.substr(breakIndex+1);
			breakIndex=temp.find(",");
			node.nodeId=temp.substr(0,breakIndex);
		}
		else
		{
			node.ipAddress="";
			node.port="";
			node.nodeId="";
		}
		list.push_back(node);
		row=row.substr(index+1);
		index=row.find("$");
	}
	string temp=row.substr(0,index);
	cell node;
	int breakIndex=temp.find(",");
	node.ipAddress=temp.substr(0,breakIndex);
	temp=temp.substr(breakIndex+1);
	breakIndex=temp.find(",");
	node.port=temp.substr(0,breakIndex);
	temp=temp.substr(breakIndex+1);
	breakIndex=temp.find(",");
	node.nodeId=temp.substr(0,breakIndex);
	list.push_back(node);
	list.push_back(globalCell);
	sort(list.begin(), list.end(), cmp);
	unsigned int i=0;
	for(i=0;i<list.size();i++)
	{
		if(globalCell.nodeId==list[i].nodeId)
			break;
	}
	int j=i-1;
	int k=0;
	while(j>=0 && k<2)
	{
		if(!list[j].ipAddress.empty())
			leftLeafSet[k++]=list[j];
		j--;
	}
	k=0;
	j=i+1;
	while(j<list.size() && k<2)
	{
		if(!list[j].ipAddress.empty())
			rightLeafSet[k++]=list[j];
		j++;
	}
	return 0;
}

int updateMap(string line)
{
	int index=line.find("$");
	string temp,key,value;
	while(index>0)
	{
		temp=line.substr(0,index);
		cell node;
		if(!temp.empty())
		{
			int breakIndex=temp.find(",");
			key=temp.substr(0,breakIndex);
			temp=temp.substr(breakIndex+1);
			breakIndex=temp.find(",");
			value=temp.substr(0,breakIndex);
			dataMap[key]=value;
		}
		line=line.substr(index+1);
		index=line.find("$");
	}
	return 0;
}

int updateKeys()
{
	cell left=leftLeafSet[0];
	cell right=rightLeafSet[0];
	if(!left.ipAddress.empty())
	{
		string temp="6$"+globalCell.nodeId;
		string nodeInfo[3];
		nodeInfo[1]=left.ipAddress;
		nodeInfo[2]=left.port;
		int sockfd=connectToNode(3,nodeInfo);
		writeIntoSocket(temp,sockfd);
		temp=readFromSocket(sockfd);
		updateMap(temp);
		close(sockfd);
	}
	if(!right.ipAddress.empty())
	{
		string temp="6$"+globalCell.nodeId;
		string nodeInfo[3];
		nodeInfo[1]=right.ipAddress;
		nodeInfo[2]=right.port;
		int sockfd=connectToNode(3,nodeInfo);
		writeIntoSocket(temp,sockfd);
		temp=readFromSocket(sockfd);
		updateMap(temp);
		close(sockfd);
	}
	return 0;
}

int sendUpdateLeafRequest()
{
	string argv[3];
	string message="3$"+globalCell.ipAddress+","+globalCell.port+","+globalCell.nodeId;
	for(int i=0;i<2;i++)
	{
		if(!leftLeafSet[i].ipAddress.empty())
		{
			argv[1]=leftLeafSet[i].ipAddress;
			argv[2]=leftLeafSet[i].port;
			int sockfd=connectToNode(3,argv);
			writeIntoSocket(message,sockfd);
			close(sockfd);
		}
	}
	for(int i=0;i<2;i++)
	{
		if(!rightLeafSet[i].ipAddress.empty())
		{
			argv[1]=rightLeafSet[i].ipAddress;
			argv[2]=rightLeafSet[i].port;
			int sockfd=connectToNode(3,argv);
			writeIntoSocket(message,sockfd);
			close(sockfd);
		}
	}
	return 0;
}

