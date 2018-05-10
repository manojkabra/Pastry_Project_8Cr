/*
 * ServerUtility.cpp
 *
 *  Created on: 08-Nov-2016
 *      Author: gaurav
 */
/*
 * heckInRange(string start, string end, string str){  }
 * sort ( )
*/
//string Route()
#include "pastrylib.h"

cell RouteTableFind(string NodeID)
{
	cell node;
	int diff = INT_MAX;
	for(int j=0;j<8;j++)
	{
		for(int i=0;i<4;i++)
		{
			if(!routingTable[j][i].nodeId.empty() && diff >= getDifference(NodeID, routingTable[j][i].nodeId))
			{
				diff = getDifference(NodeID, routingTable[j][i].nodeId);
				node=routingTable[j][i];
			}
		}
	}
	return node;
}


void copy(cell &temp, cell &leaf)
{
	temp.ipAddress = leaf.ipAddress;
	temp.port = leaf.port;
	temp.nodeId = leaf.nodeId;
	return;
}

void LeafSort(cell LeafSet[])
{
	cell temp;
	if(num(LeafSet[1].nodeId)< num(LeafSet[0].nodeId))
	{
		copy(temp,LeafSet[0]);
		copy(LeafSet[0],LeafSet[1]);
		copy(LeafSet[1],temp);
	}
}


int GetRequest(string NodeID, cell &temp)
{
	int myval = num(globalCell.nodeId);
	int tval = num(NodeID);
	if(myval - tval == 0)
		return 2;
	else
	{
		vector <cell> complete;
		for(unsigned int i =0;i<2;i++)
		{
			if(!leftLeafSet[i].ipAddress.empty())
				complete.push_back(leftLeafSet[i]);
		}
		for(unsigned int i =0;i<2;i++)
		{
			if(!rightLeafSet[i].ipAddress.empty())
				complete.push_back(rightLeafSet[i]);
		}
		if(complete.size()==0)
			return 2;
		else if(complete.size() == 1)
		{
			if(abs(tval -myval)>abs(tval - num(complete[0].nodeId))){
				copy(temp,complete[0]);
				return 1;
			}
			else{

				return 2;
			}
		}
		else if(num(complete.front().nodeId)<num(NodeID) &&  num(complete.back().nodeId)>num(NodeID))
		{
			int diff = abs(myval - tval);
			int loc = -1;
			for(unsigned int i= 0 ;i<complete.size();i++)
			{
				if(diff > abs(tval - num(complete[i].nodeId)))
				{
					diff = abs(tval - num(complete[i].nodeId));
					loc = i;
				}
			}
			if(loc == -1)
			{
				return 2;
			}
			else
			{
				copy(temp, complete[loc]);
				return 1;
			}

		}
		else
		{
			cell rTF = RouteTableFind(NodeID);
			copy(temp, rTF);
			return 1;
		}

	}
}

string wrapperGetRequest(string key)
{
	cell temp;
	temp.ipAddress="";
	temp.port="";
	temp.nodeId="";
	int res = GetRequest(key, temp);
	string message = "";
	if(res == 2)
	{
		//Search In Map
		if(dataMap.find(key) == dataMap.end())
		{
			message = "1$NULL";
		}
		else
		{
			message = "2$"+ (*(dataMap.find(key))).second;
		}

	}
	else
	{
		//Check if Null else send the Node
		if(temp.ipAddress.empty() || temp.nodeId == globalCell.nodeId )
		{
			if(dataMap.find(key) == dataMap.end())
			{
				message = "1$NULL";
			}
			else
			{
				message = "2$"+ (*(dataMap.find(key))).second;
			}
		}
		else
		{
			message = "1$"+temp.ipAddress+","+temp.port+","+temp.nodeId;
		}
	}
	return message;
}

int putRequest(string key, string value)
{
	cell temp;
	temp.ipAddress="";
	int res = GetRequest(key,temp);
	if(res == 2)
	{
		dataMap[key]=value;
	}
	else
	{
		if(temp.nodeId==globalCell.nodeId)
		{
			dataMap[key]=value;
		}
		else
		{
			string message = "1$" + key + "," + value;
			string nodeInfo[3];
			nodeInfo[1]=temp.ipAddress;
			nodeInfo[2]=temp.port;
			int sockfd=connectToNode(3,nodeInfo);
			writeIntoSocket(message,sockfd);
			close(sockfd);
		}
	}
	return 0;
}
/*Function for fetching a row from routing table*/
string getRowString(string rowNo)
{
	string temp;
	int rowNum=atoi(rowNo.c_str());
	if(!routingTable[rowNum][0].ipAddress.empty())
	{
		temp=routingTable[rowNum][0].ipAddress+","+routingTable[rowNum][0].port+","+routingTable[rowNum][0].nodeId;
	}
	else
		temp="NULL";
	for(int i=1;i<4;i++)
	{
		if(!routingTable[rowNum][i].ipAddress.empty())
		{
			temp=temp+"$"+routingTable[rowNum][i].ipAddress+","+routingTable[rowNum][i].port+","+routingTable[rowNum][i].nodeId;
		}
		else
			temp=temp+"$NULL";
	}
	return temp;
}


int updateRouteTable(string cellInfo,string rowString)
{
	int rowNo=atoi(rowString.c_str());
	int index=cellInfo.find(",");
	string ipAddress=cellInfo.substr(0,index);
	cellInfo=cellInfo.substr(index+1);
	index=cellInfo.find(",");
	string port=cellInfo.substr(0,index);
	string nodeId=cellInfo.substr(index+1);
	if(routingTable[rowNo][nodeId[rowNo]-'0'].ipAddress.empty())
	{
		cell node;
		node.ipAddress=ipAddress;
		node.port=port;
		node.nodeId=nodeId;
		routingTable[rowNo][nodeId[rowNo]-'0']=node;
	}
	else if(getDifference(routingTable[rowNo][nodeId[rowNo]-'0'].nodeId,globalCell.nodeId)>getDifference(nodeId,globalCell.nodeId))
	{
		cell node;
		node.ipAddress=ipAddress;
		node.port=port;
		node.nodeId=nodeId;
		routingTable[rowNo][nodeId[rowNo]-'0']=node;
	}
	return 0;
}

int getDifference(string nodeId1,string nodeId2)
{
	return abs(num(nodeId1) - num(nodeId2));
}
