#ifndef DBLinkedList_h
#define DBLinkedList_h 1 
	
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

struct node
{
	string ip;
	string start_d;
	string start_t;
	string stop_d;
	string stop_t;
	int requests;
	node *next;
	node *previous;
};


class DBLinkedList
{
  public:
	DBLinkedList();
	~DBLinkedList();
	
	void first_entry(node* &head, node* &tail, string ip, string start_d, string start_t);
	void add_new_entry(node* &head, string ip, string start_d, string start_t);
	void delete_and_reinsert(node* n, node* &head, node* &tail, string stop_d, string stop_t);
	
	string extract_inactive_session(node* &head, node* &tail, int inactivity_period, string current_d, string current_t,string &ip_to_delete, int &time_of_arrival);
	string extract_residual_sessions(node* &head, node* &tail, string &ip_to_delete,int &time_of_arrival);
	//void print_list(node* n);
	//void print_node_IP(node* n);
	int  get_time_in_seconds(string time_str);
	int  get_date_in_days(string date_str);
	

  private:
	
	node* create_node(string ip, string start_t, string stop_t);
	int get_time_delay(string start_d, string start_t, string stop_d, string stop_t);		

};
	
#endif
