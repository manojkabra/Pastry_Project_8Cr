/*
 * Main.cpp
 *
 *  Created on: 07-Nov-2016
 *      Author: rohit
 */

#include "pastrylib.h"

int main()
{
	cout<<"\n*********************************************************\n";
	cout<<"\t\tWELCOME TO PASTRY NETWORK \n";
	cout<<"\n*********************************************************\n\n";
	string cmd;
	while (1)
	{
		cout<<endl<<"pastry : $  ";
		cmd = get_command();
		if(!cmd.empty())
			execute(cmd);
	}
	return 0;
}
