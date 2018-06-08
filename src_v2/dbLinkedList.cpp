#include "dbLinkedList.h"

using namespace std;


uint month_days[] = {0,31,59,90,120,151,181,212,243,273,304,334};

DBLinkedList::DBLinkedList()  //Constructor
{
}

DBLinkedList::~DBLinkedList()  //Deconstructor
{
}

node* DBLinkedList::create_node(string ip, string start_d, string start_t) //Initialize Node 
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

void DBLinkedList::first_entry(node* &head, node* &tail, string ip, string start_d, string start_t)  //Set first Node and assign to head and tail
{
        node *newOne = create_node(ip, start_d, start_t);
	head = newOne;
        tail = newOne;
}

void DBLinkedList::add_new_entry(node* &head, string ip, string start_d, string start_t)  //Insert new entry at the head of the linked list
{
	node *newOne = create_node(ip, start_d, start_t);
        newOne->next = head;
        if(head!=NULL) head->previous = newOne;
	head = newOne;
}

void DBLinkedList::delete_and_reinsert(node* n, node* &head, node* &tail, string stop_d, string stop_t)  //Remove node from the current position and insert it at the head. This happens it IP address reoccurs
{
	n->stop_d =stop_d;  //Update stop day
	n->stop_t =stop_t;  //Update stop time;
	n->requests++;  //Add a request
	if(n->ip==head->ip) return;  //if node is already head node, the job is done
	if(n->previous!=NULL&&n->next!=NULL) //Already at head of list or at end of list
	{
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

string DBLinkedList::extract_inactive_session(node* &head, node* &tail, int inactivity_period, string current_d, string current_t, string &ip_to_delete, int &time_of_arrival)  //Extract content of node in output string format during file run
{
	int sec = get_time_delay(tail->stop_d,tail->stop_t,current_d, current_t)-1;	 	
	if(sec > inactivity_period)
	{
		sec=get_time_delay(tail->start_d,tail->start_t,tail->stop_d,tail->stop_t);
		string result=tail->ip+","+tail->start_d+" "+tail->start_t+","+tail->stop_d+" "+tail->stop_t+","+to_string(sec)+","+to_string(tail->requests);
		ip_to_delete=tail->ip;
		time_of_arrival=get_time_in_seconds(tail->start_t);
		if(tail->next!=NULL) tail->next=NULL;
		if(tail->previous!=NULL)
		{
			tail=tail->previous;
			tail->next=NULL;
		}
		else if(head!=NULL) //Trick to release last element. Probably better solution available
		{
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

string DBLinkedList::extract_residual_sessions(node* &head, node* &tail,string &ip_to_delete,int &time_of_arrival)  //Extract content of node in output string format at the end of input file
{
        int sec=get_time_delay(tail->start_d,tail->start_t,tail->stop_d,tail->stop_t);
        string result=tail->ip+","+tail->start_d+" "+tail->start_t+","+tail->stop_d+" "+tail->stop_t+","+to_string(sec)+","+to_string(tail->requests);
        ip_to_delete=tail->ip;
        time_of_arrival=get_time_in_seconds(tail->start_t);
        if(tail->next!=NULL) tail->next=NULL;
	if(tail->previous!=NULL)
	{
		tail=tail->previous;
		tail->next=NULL;
	}
	else if(head!=NULL)  //Trick to release last element. Probably better solution available
	{
		head=NULL;
		return result;
	}
	else
	{
		tail=NULL;
		return "";
	}

	return result;
}



void DBLinkedList::print_list(node* n)   //for debugging
{
	while(n!=NULL)
        {
                print_node_IP(n);
                n=n->next;
        }
        print_node_IP(n);
}

void DBLinkedList::print_node_IP(node* n)
{
	if(n!=NULL) cout<<n->ip<<endl;
}

int DBLinkedList::get_time_in_seconds(string time_str)  //Convert time to comparable unit
{ 
        istringstream time_ss(time_str);  //use istringstream instead of stringstream to read numbers
        int hh,mm,ss;
        char dump1,dump2;
        time_ss>>hh>>dump1>>mm>>dump2>>ss;
        if(dump1!=':'&&dump2!=':') cerr<<"Time format is wrong"<<endl;
        return hh*60*60+mm*60+ss;
}

int DBLinkedList::get_date_in_days(string date_str)  //Convert date to comparable unit
{
        istringstream date_ss(date_str);
        int yy,mm,dd;
	int leapyears=0;
        char dump1,dump2;
        date_ss>>yy>>dump1>>mm>>dump2>>dd;
        if(dump1!='-'&&dump2!='-') cerr<<"Date format is wrong"<<endl;
        leapyears    = yy / 4;
        if (yy % 4 == 0 && mm < 3)
        {
            // If this is a leap year
            // And we have not passed Feburary then it does
            // not count.....
            leapyears   --;
        }
	
	return yy*365+month_days[mm-1]+dd+leapyears;   
}

int DBLinkedList::get_time_delay(string start_d, string start_t, string stop_d, string stop_t)  //Calculate time between two events. Required to check if session is inactive and to calculate time of session for output
{
	int sec=0;
	if(stop_d==start_d)
	{
		if(stop_t==start_t) 
		{
			sec=1;
		}
		else
		{
			sec=get_time_in_seconds(stop_t)-get_time_in_seconds(start_t)+1;		
		}
	}
	else{
		int days=get_date_in_days(stop_d)-get_date_in_days(start_d);           ////get number of days  !!!!!!!!!!!!!incorrect if month switch as not every month has 30 days
		sec=86400-get_time_in_seconds(start_t)+get_time_in_seconds(stop_t);	//seconds on the first_day and last day
		sec+=(days-1)*86400;   //add full days in between
	}
	return sec;
}

string DBLinkedList::get_node_content(node*n){
	int sec=get_time_delay(n->start_d,n->start_t,n->stop_d,n->stop_t);
        string result=n->ip+","+n->start_d+" "+n->start_t+","+n->stop_d+" "+n->stop_t+","+to_string(sec)+","+to_string(n->requests);
	return result;
}


