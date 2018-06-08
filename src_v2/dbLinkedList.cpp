#include "dbLinkedList.h"

using namespace std;

DBLinkedList::DBLinkedList()
{
}

DBLinkedList::~DBLinkedList()
{
}

node* DBLinkedList::create_node(string ip, string start_d, string start_t)
{
	node *newOne = new node;
        newOne->ip = ip;
	newOne->start_d = start_d;
	newOne->start_t = start_t;
	newOne->stop_d = start_d;
	newOne->stop_t = start_t;
        newOne->requests = 1;
        newOne->next = NULL;
        newOne->previous = NULL;
        return newOne;
}

void DBLinkedList::first_entry(node* &head, node* &tail, string ip, string start_d, string start_t)
{
        node *newOne = create_node(ip, start_d, start_t);
	head = newOne;
        tail = newOne;
}

void DBLinkedList::add_new_entry(node* &head, string ip, string start_d, string start_t)
{
	node *newOne = create_node(ip, start_d, start_t);
        newOne->next = head;
        if(head!=NULL) head->previous = newOne;
	head = newOne;
}

void DBLinkedList::delete_at_end(node* &tail)
{
	tail=tail->previous;
	tail->next->next = NULL;
        tail->next->ip ="";
	tail->next->start_d="";
        tail->next->start_t="";
	tail->next->stop_d="";
        tail->next->stop_t="";
	tail->next->requests=0;
	tail->next=NULL;
}

void DBLinkedList::delete_and_reinsert(node* n, node* &head, node* &tail, string stop_d, string stop_t)
{
	n->stop_d =stop_d; 
	n->stop_t =stop_t;
	n->requests++;
	if(n->ip==head->ip) return;  //if node is already head node, the job is done
	if(n->previous!=NULL&&n->next!=NULL) //Already at head of list or at end of list
	{
		//cout<<"Next node: "<<n->next->ip<<endl; 
		n->previous->next=n->next;
		n->next->previous=n->previous;
		n->previous=NULL;
		n->next = head;
		head->previous=n;
		head=n;
	}
	else if(n->next==NULL) //In this case the update node is the tail and the tail needs to be updated
	{
		n->previous->next = NULL;
		tail= n->previous;
		n->previous=NULL;
		n->next = head;
		head->previous=n;
		head=n;
	}
}

string DBLinkedList::extract_inactive_session(node* &head, node* &tail, int inactivity_period, string current_d, string current_t, string &ip_to_delete, int &time_of_arrival)
{
	if((get_time_in_seconds(current_t)-get_time_in_seconds(tail->stop_t)) > inactivity_period)
	{
		int sec=0;
		if(tail->start_t==tail->stop_t) 
		{
			sec=1;
		}
		else
		{
			sec=get_time_in_seconds(tail->stop_t)-get_time_in_seconds(tail->start_t)+1;
		}
		string result=tail->ip+","+tail->start_d+" "+tail->start_t+","+tail->stop_d+" "+tail->stop_t+","+to_string(sec)+","+to_string(tail->requests);
		ip_to_delete=tail->ip;
		time_of_arrival=get_time_in_seconds(tail->start_t);
		/*tail->ip="";
		tail->start_d="";
		tail->start_t="";
		tail->stop_d="";
		tail->stop_t="";
		tail->requests=0;*/
		if(tail->next!=NULL) tail->next=NULL;
		if(tail->previous!=NULL)
		{
			tail=tail->previous;
			tail->next=NULL;
		}
		else if(head!=NULL){
			head=NULL;
			return result;
		}	
		else{
			tail=NULL;
			return "";
		}
		return result;
	}
	return "";
}

string DBLinkedList::extract_residual_sessions(node* &head, node* &tail,string &ip_to_delete,int &time_of_arrival)
{
        int sec=0;
        if(tail->start_t==tail->stop_t)
        {
                sec=1;
        }
        else
        {
                sec=get_time_in_seconds(tail->stop_t)-get_time_in_seconds(tail->start_t)+1;
        }
        string result=tail->ip+","+tail->start_d+" "+tail->start_t+","+tail->stop_d+" "+tail->stop_t+","+to_string(sec)+","+to_string(tail->requests);
        ip_to_delete=tail->ip;
        time_of_arrival=get_time_in_seconds(tail->start_t);
        /*tail->ip="";
        tail->start_d="";
        tail->start_t="";
        tail->stop_d="";
        tail->stop_t="";
        tail->requests=0;*/
        if(tail->next!=NULL) tail->next=NULL;
	if(tail->previous!=NULL)
	{
		//cout<<"Prev "<<tail->ip<<endl;
		tail=tail->previous;
		tail->next=NULL;
	}
	else if(head!=NULL){
		head=NULL;
		return result;
	}
	else
	{
		//cout<<"Af "<<tail->ip<<endl;
		tail=NULL;
		return "";
	}

	return result;
}



void DBLinkedList::print_list(node* n)
{
	while(n!=NULL)
        {
                cout<<"Node IP: ";
                print_node_IP(n);
                cout<<endl;
                cout<<"Node previous IP: ";
                if(n->previous!=NULL) print_node_IP(n->previous);
                cout<<endl;
                n=n->next;
        }
        print_node_IP(n);
}

void DBLinkedList::print_node_IP(node* n)
{
	if(n!=NULL) cout<<n->ip<<endl;
}

int DBLinkedList::get_time_in_seconds(string time_str){
        istringstream time_ss(time_str);
        int hh,mm,ss;
        char dump1,dump2;
        time_ss>>hh>>dump1>>mm>>dump2>>ss;
        if(dump1!=':'&&dump2!=':') cerr<<"Time format is wrong"<<endl;
        return hh*60*24+mm*60+ss;
}




