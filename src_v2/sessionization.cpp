#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <climits>
#include <algorithm>

#include "dbLinkedList.h"

using namespace std;


void read_header(ifstream &csv_file, int &field_ip, int &field_date, int &field_time, int &field_cik, int &field_accession, int &field_extention)
{
	string tmp_string;
	getline(csv_file, tmp_string);
	stringstream first_line(tmp_string);
	int counter=0;
	while(first_line.good())
	{
		getline(first_line,tmp_string,',');
		if(tmp_string=="ip") field_ip=counter;
		else if(tmp_string=="date") field_date=counter;
  		else if(tmp_string=="time") field_time=counter;
		else if(tmp_string=="cik") field_cik=counter;		
		else if(tmp_string=="accession") field_accession=counter;
		else if(tmp_string=="extention") field_extention=counter;
		counter++;
	}
	if(field_ip==-1 || field_date==-1 || field_time==-1 || field_cik==-1 ||
		   field_accession==-1 || field_extention==-1) cerr<<"Header Incomplete"<<endl;
}

vector<string> read_line(ifstream &csv_file, int &last_field,int &field_ip, int &field_date, int &field_time)
{
	string tmp_string;
	vector<string> result;
	getline(csv_file, tmp_string);
	stringstream first_line(tmp_string);
	int counter=0;
	while(first_line.good())
	{
		getline(first_line,tmp_string,',');
		//field_names.push_back(tmp_string);
		if(counter==field_ip) result.push_back(tmp_string);
		else if(counter==field_date) result.push_back(tmp_string);
  		else if(counter==field_time) result.push_back(tmp_string);
		if(counter==last_field) 
		{
			/*if(field_ip<field_date&&field_date<field_time)*/ return result; //in case the fields are in order result is returned, this check requires two compariosns
			/*else if(field_ip<field_time&&field_date>field_time) swap(result[1],result[2]);//for the less common unordered fields, vector fields are exchanged. !!!Slower processing
			else if(field_ip>field_date&&field_time>field_date) swap(result[0],result[1]);
			else if(field_ip>field_date&&field_date>field_time) swap(result[0],result[2]);
			else if(field_ip>field_time&&field_date>field_time) {swap(result[0],result[1]); swap(result[1],result[2]);}
			else if(field_ip>field_time&&field_date<field_time) {swap(result[0],result[1]); swap(result[0],result[2]);}
			return result;*/	
		}
		counter++;
	}
}

int max_field(int &field_id, int &field_date, int &field_time)
{
	if(field_id > field_date && field_id > field_time) return field_id;
	else if(field_date > field_id && field_date > field_time) return field_date;
	else if(field_time > field_id && field_time > field_date) return field_time;
	else return -1;
}	

int main()
{
	string tmp_string;
	string out, out2;
	int field_ip=-1, field_date=-1, field_time=-1, field_cik=-1, field_accession=-1, field_extention=-1;	
	int inactivity_period=-1,last_field=-1;
	bool first_line=true;
	string ip_to_delete;
	int time_of_arrival;
	ifstream csv_file("input/log.csv");
	ifstream inactivity_file("input/inactivity_period.txt");
	ofstream output_file("output/sessionization.txt");	
	vector<string> vec;
	unordered_map<string,node*> hmap;
	unordered_map<string,node*>::const_iterator it;
	DBLinkedList dllist;
	node* tail = new node; 
	node* head = new node;
	head=NULL;
	tail=NULL;

	if(inactivity_file.is_open())
	{
		getline(inactivity_file, tmp_string);
		inactivity_period=atoi(tmp_string.c_str()); //don't forget error checking if string is not a number
	}

	if(csv_file.is_open())
	{	
		read_header(csv_file, field_ip, field_date, field_time, field_cik, field_accession, field_extention);
		last_field = max_field(field_ip, field_date, field_time);
		while(csv_file.good())
		{
			vec = read_line(csv_file, last_field, field_ip, field_date, field_time);  // Make sure that this also works if fields are in decreasig order
			if(vec[0]=="") break;
			if(tail==NULL)
			{
				dllist.first_entry(head,tail,vec[0],vec[1],vec[2]);
				hmap.insert({vec[0],head}); 
			}	
			else
			{
				it=hmap.find(vec[0]);
				if(it!=hmap.end())
				{
					dllist.delete_and_reinsert(it->second,head,tail,vec[1],vec[2]);
				}
				else
				{
					dllist.add_new_entry(head,vec[0],vec[1],vec[2]);
					hmap.insert({vec[0],head});
				}
			}
			out="first"; //required to statisfy while loop condition
		        while(out!="")
			{
				out = dllist.extract_inactive_session(head,tail,inactivity_period,vec[1],vec[2],ip_to_delete,time_of_arrival); ///make looop to write out all inactive entries;
				if(out!="")	//double check is required as this is maybe the last loop cycle  (all processes < inactivity time)
				{	
					output_file<<out<<endl;     //write to file
					hmap.erase(ip_to_delete);   //remove written entry from hash table
				}
			}
		}
	}
	//dllist.print_list(head);   //for debug purpose
	vector<string> clear_LL;   //vector for residual List entries (Sort Double Linked List probably more efficient solution for large lists, but not tested because of lack of time)
        vector<int> start_time;   //use vector to find timing order
        while(tail!=NULL)
        {
        	out2 = dllist.extract_residual_sessions(head,tail,ip_to_delete,time_of_arrival);
        	if(out2!="")
                {
			clear_LL.push_back(out2);
			start_time.push_back(time_of_arrival);
                }
        }
	while(clear_LL.size()!=0)
	{
		int min_time=INT_MAX;
		int first=-1;
		for(int i=0; i<start_time.size(); i++)
		{
			if(start_time[i]<min_time)
                        {
				min_time=start_time[i];
				first=i;
                        }
		}
		output_file<<clear_LL[first]<<endl;
		clear_LL.erase(clear_LL.begin()+first);
		start_time.erase(start_time.begin()+first);
	}
	delete tail;	// clear heap
	delete head;
	output_file.close();
	csv_file.close();
	inactivity_file.close();
	
	return 1;
}
