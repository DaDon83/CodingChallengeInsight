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
	void delete_at_end(node* &tail);
	void delete_and_reinsert(node* n, node* &head, node* &tail, string stop_d, string stop_t);
	
	string extract_inactive_session(node* &head, node* &tail, int inactivity_period, string current_d, string current_t,string &ip_to_delete, int &time_of_arrival);
	string extract_residual_sessions(node* &head, node* &tail, string &ip_to_delete,int &time_of_arrival);
	void print_list(node* n);
	void print_node_IP(node* n);
	string get_ip(node* n) { return n->ip; }
	string get_start_d(node* n){ return n->start_d; }
        string get_start_t(node* n){ return n->start_t; }
	string get_stop_d(node* n) { return n->stop_d; }
	string get_stop_t(node* n) { return n->stop_t; }
	int    get_nb_requests(node* n) { return n->requests; }
	int    get_time_in_seconds(string time_str);
	

  private:
	
	node* create_node(string ip, string start_t, string stop_t);
		

};
	
#endif
