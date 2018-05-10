/*
 * ServerRequest.cpp
 *
 *  Created on: 09-Nov-2016
 *      Author: rohit
 */

#include "pastrylib.h"

int requestController(string input,int sockfd)
{
	switch(input[0])
	{
		case '1':	executePutRequest(input,sockfd);
					break;
		case '2':	executeGetRequest(input,sockfd);
					break;
		case '3':	executeUpdateLeaf(input,sockfd);
					break;
		case '4':	executeRowRequest(input,sockfd);
					break;
		case '5':	executeLeafRequest(input,sockfd);
					break;
		case '6':	executeKeyRequest(input,sockfd);
					break;
		default:	printErrorMessage("Invalid request");
					break;
	}
	return 0;
}

/*Function for executing put request*/
int executePutRequest(string input,int sockfd)
{
	string temp=input.substr(2);
	int index=temp.find(",");
	string key=temp.substr(0,index);
	string value=temp.substr(index+1);
	close(sockfd);
	putRequest(key,value);
	return 0;
}

/*Function for executing get request*/
int executeGetRequest(string input,int sockfd)
{
	string key=input.substr(2);
	string message = wrapperGetRequest(key);
	writeIntoSocket(message,sockfd);
	close(sockfd);
	return 0;
}

/*Function for updating the leaf set*/
int executeUpdateLeaf(string input,int sockfd)
{
	string cellInfo=input.substr(2);
	string leafString;
	for(int i=0;i<2;i++)
	{
		if(leftLeafSet[i].ipAddress.empty())
			leafString=leafString+"$NULL";
		else
			leafString=leafString+"$"+leftLeafSet[i].ipAddress+","+leftLeafSet[i].port+","+leftLeafSet[i].nodeId;
	}
	for(int i=0;i<2;i++)
	{
		if(rightLeafSet[i].ipAddress.empty())
			leafString=leafString+"$NULL";
		else
			leafString=leafString+"$"+rightLeafSet[i].ipAddress+","+rightLeafSet[i].port+","+rightLeafSet[i].nodeId;
	}
	int breakIndex=cellInfo.find(",");
	string ipAddress=cellInfo.substr(0,breakIndex);
	cellInfo=cellInfo.substr(breakIndex+1);
	breakIndex=cellInfo.find(",");
	string port=cellInfo.substr(0,breakIndex);
	cellInfo=cellInfo.substr(breakIndex+1);
	breakIndex=cellInfo.find(",");
	string nodeId=cellInfo.substr(0,breakIndex);
	leafString=leafString+"$"+ipAddress+","+port+","+nodeId;
	leafString=leafString.substr(1);
	parseLeafInput(leafString);
	return 0;
}

/*Function for executing the fetch row request*/
int executeRowRequest(string input,int sockfd)
{
	joinFlag=true;
	string rowNo;
	rowNo.push_back(input[2]);
	string rowString=getRowString(rowNo);
	writeIntoSocket(rowString,sockfd);
	close(sockfd);
	string cellInfo=input.substr(4);
	updateRouteTable(cellInfo,rowNo);
	return 0;
}

/*Function for executing the fetch leaf set request*/
int executeLeafRequest(string input,int sockfd)
{
	string temp;
	for(int i=0;i<2;i++)
	{
		if(leftLeafSet[i].ipAddress.empty())
			temp=temp+"$NULL";
		else
			temp=temp+"$"+leftLeafSet[i].ipAddress+","+leftLeafSet[i].port+","+leftLeafSet[i].nodeId;
	}
	for(int i=0;i<2;i++)
	{
		if(rightLeafSet[i].ipAddress.empty())
			temp=temp+"$NULL";
		else
			temp=temp+"$"+rightLeafSet[i].ipAddress+","+rightLeafSet[i].port+","+rightLeafSet[i].nodeId;
	}
	temp=temp.substr(1);
	writeIntoSocket(temp,sockfd);
	close(sockfd);
	return 0;
}

/*Function for executing the fetch key request*/
int executeKeyRequest(string input,int sockfd)
{
	string temp;
	string nodeId=input.substr(2);
	for(map<string,string>::iterator it=dataMap.begin();it!=dataMap.end();it++)
	{
		if(getDifference(it->first,globalCell.nodeId)>getDifference(it->first,nodeId))
		{
			temp=temp+"$"+it->first+","+it->second;
			dataMap.erase(it);
		}
	}
	if(!temp.empty())
		temp=temp.substr(0,temp.length()-1);
	writeIntoSocket(temp,sockfd);
	close(sockfd);
	return 0;
}
