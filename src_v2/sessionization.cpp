#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <climits>
#include <algorithm>

#include "dbLinkedList.h"

using namespace std;

void read_header(ifstream &csv_file, int &field_ip, int &field_date, int &field_time, int &field_cik, int &field_accession, int &field_extention, int &order)  //The header read function reads the first line of the saves the position of the interesting data fields
{
	string tmp_string;
	getline(csv_file, tmp_string);
	stringstream first_line(tmp_string);  //A stringstream is used to partition the string by a seperator char
	int counter=0;  //The counter highlights where the field is positioned in the header
	while(first_line.good())
	{
		getline(first_line,tmp_string,',');  //Partition string by comma
		if(tmp_string=="ip") field_ip=counter;  //Assign fields to key words
		else if(tmp_string=="date") field_date=counter;
  		else if(tmp_string=="time") field_time=counter;
		else if(tmp_string=="cik") field_cik=counter;  //The cik and the following two fields are not further required but probably interesting for other applications 	
		else if(tmp_string=="accession") field_accession=counter;
		else if(tmp_string=="extention") field_extention=counter;
		counter++;
	}
	if(field_ip==-1 || field_date==-1 || field_time==-1 || field_cik==-1 ||   
		   field_accession==-1 || field_extention==-1) cerr<<"Header Incomplete"<<endl;  //Error check if all fields are exsisting
	if(field_ip<field_date&&field_date<field_time) order =0;  //In case the fields are in order, order is 0
	else if(field_ip<field_time&&field_date>field_time) order=1;  //For the less common unordered fields, vector fields are exchanged. !!!Slower processing. If I had more time I would determine the case only once in the header function. But then a global variable or an extra parameter are required
	else if(field_ip>field_date&&field_ip<field_time) order=2;
	else if(field_ip>field_date&&field_date>field_time) order=3;
	else if(field_ip>field_time&&field_date>field_time) order=4;
	else if(field_ip>field_time&&field_date<field_time) order=5;
	
}

vector<string> read_line(ifstream &csv_file, int last_field,int field_ip, int field_date, int field_time, int order)  //The read line function reads subsequent lines of the file and pushes the required fields into the vector
{
	string tmp_string;
	vector<string> result;  //The vector will hold the key fields
	getline(csv_file, tmp_string);
	stringstream first_line(tmp_string);
	int counter=0;
	while(first_line.good())
	{
		getline(first_line,tmp_string,',');  //Partition string by comma
		if(counter==field_ip) result.push_back(tmp_string);  //Add IP address to vector
		else if(counter==field_date) result.push_back(tmp_string); //Add date to vector
  		else if(counter==field_time) result.push_back(tmp_string);  //Add time to vector
		if(counter==last_field)  //Stop if all required fields are accessed
		{
			switch(order)
			{
				case 0: return result;  //In case the fields are in order result is returned, this check requires two compariosns
				case 1: swap(result[2],result[1]); break;   //For the less common unordered fields, vector fields are exchanged. !!!Slower processing. If I had more time I would determine the case only once in the header function. But then a global variable or an extra parameter are required
				case 2: swap(result[0],result[1]); break;
				case 3: swap(result[0],result[2]); break;
			        case 4:	swap(result[0],result[1]); 
					swap(result[1],result[2]);
					break;
				case 5: swap(result[0],result[1]); 
					swap(result[0],result[2]);
					break;
			}
			return result;
		}
		counter++;
	}
}

int max_field(int &field_id, int &field_date, int &field_time)  //The maximum field function determines the last significant field (signigicant=field required for processing)
{
	if(field_id > field_date && field_id > field_time) return field_id;
	else if(field_date > field_id && field_date > field_time) return field_date;
	else if(field_time > field_id && field_time > field_date) return field_time;
	else return -1;
}	

int main(int argc, char* argv[]) //argv holds the input and output files
{
	string tmp_string;  //Temporary string
	string out, out2;  //Strings used to write out data
	int field_ip=-1, field_date=-1, field_time=-1, field_cik=-1, field_accession=-1, field_extention=-1;  //Variables are set to -1 so they have to be set before operation
	int inactivity_period=-1,last_field=-1;  
	string ip_to_delete;  //Variable used to delete elements in hash table
	int time_of_arrival;  //Variable used to sort vector/buffer after the input file is fully processed
	int order=0;
	ifstream csv_file(argv[1]);  //Input and output files
	ifstream inactivity_file(argv[2]);
	ofstream output_file(argv[3]);	
	vector<string> vec;  //Vector used to sort buffer at the end of the run
	unordered_map<string,node*> hmap;  //Hashtable to point to linked list elements
	unordered_map<string,node*>::const_iterator it;  //Iterator for hashtable used to find entries
	DBLinkedList dllist;  //Double linked list defined in dblinkedlist.cpp
	node* tail = NULL;  //Head and tail nodes of the linked list
	node* head = NULL;

	if(inactivity_file.is_open())  //Read inactivity period file
	{
		getline(inactivity_file, tmp_string);
		inactivity_period=atoi(tmp_string.c_str());  
	}

	if(csv_file.is_open())  //Read header and determine last significant field
	{	
		read_header(csv_file, field_ip, field_date, field_time, field_cik, field_accession, field_extention, order);
		last_field = max_field(field_ip, field_date, field_time);
		while(csv_file.good())  //Read lines til file is at end
		{
			vec = read_line(csv_file, last_field, field_ip, field_date, field_time, order);  //Read line and return result vector
			if(vec[0]=="") break;  //Skip if no IP address is given
			if(tail==NULL)  //Tail is NULL when linked list is empty
			{
				dllist.first_entry(head,tail,vec[0],vec[1],vec[2]); //Add first node
				hmap.insert({vec[0],head});  //Insert first entry in hashtable
			}	
			else
			{
				it=hmap.find(vec[0]);  //Check if new IP address is in hashtable (Have we seen this IP before?)
				if(it!=hmap.end())  //If found
				{
					dllist.delete_and_reinsert(it->second,head,tail,vec[1],vec[2]);  //Ddelete current node and move it to the header
				}
				else
				{
					dllist.add_new_entry(head,vec[0],vec[1],vec[2]);  //If new IP address, add to header
					hmap.insert({vec[0],head});  //Now as IP is in Linked List, add the pointer to the hash table
				}
			}
			out="first";  //Required to satisfy while loop condition
		        while(out!="")
			{
				out = dllist.extract_inactive_session(head,tail,inactivity_period,vec[1],vec[2],ip_to_delete,time_of_arrival);  //Make looop to write out all inactive entries;
				if(out!="") //Double check is required as this is maybe the last loop cycle  (all processes < inactivity time)
				{	
					output_file<<out<<endl;  //Write to file
					hmap.erase(ip_to_delete);  //Remove written entry from hash table
				}
			}
		}
	}
	//dllist.print_list(head);   //For debug purpose
        node* walker=tail;
	multimap<int,node*> ordered_map;
	int counter=0;
	int pos=0;
	int start_time=-1;
	while(walker!=NULL)
        { 		
		start_time=dllist.get_time_in_seconds(walker->start_t); //only orders after time, not date
		ordered_map.insert({start_time,walker});	
		walker=walker->previous;
	}
	for(auto x : ordered_map)
	{	
		out2=dllist.get_node_content(x.second);
        	if(out2!="")
               	{
			output_file<<out2<<endl;
               	}
	}

	//delete head;  //clear heap
	//delete tail;
	//delete walker;	
	output_file.close();  //Close files  
	csv_file.close();
	inactivity_file.close();
	
	return 1;
}
