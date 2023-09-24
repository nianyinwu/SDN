#include <iostream>
#include <map>
#include <queue>
#include <utility>
#include <climits>
#include <functional>
#include <iomanip>
#include <stack>
#include <string>
#include <algorithm>
#define MAX INT_MAX

using namespace std;

#define SET(func_name,type,var_name,_var_name) void func_name(type _var_name) { var_name = _var_name ;}
#define GET(func_name,type,var_name) type func_name() const { return var_name ;}
#define Type vector<pair<unsigned int,unsigned int>>

class header;
class payload;
class packet;
class node;
class event;
class link; // new

// for simplicity, we use a const int to simulate the delay
// if you want to simulate the more details, you should revise it to be a class
const unsigned int ONE_HOP_DELAY = 10;
const unsigned int BROCAST_ID = UINT_MAX;
vector<vector<pair<int,int>>> updateTable; // store the update order of nodes
vector<unsigned int>dstID; // store the dst nodes
static int totalRound = 0;
unsigned int nodes;
unsigned int con_id;
// BROCAST_ID means that all neighbors are receivers; UINT_MAX is the maximum value of unsigned int

class header {
    public:
        virtual ~header() {}

        SET(setSrcID, unsigned int , srcID, _srcID);
        SET(setDstID, unsigned int , dstID, _dstID);
        SET(setPreID, unsigned int , preID, _preID);
        SET(setNexID, unsigned int , nexID, _nexID);
        GET(getSrcID, unsigned int , srcID);
        GET(getDstID, unsigned int , dstID);
        GET(getPreID, unsigned int , preID);
        GET(getNexID, unsigned int , nexID);

        virtual string type() = 0;

        // factory concept: generate a header
        class header_generator {
                // lock the copy constructor
                header_generator(header_generator &){}
                // store all possible types of header
                static map<string,header_generator*> prototypes;
            protected:
                // allow derived class to use it
                header_generator() {}
                // after you create a new header type, please register the factory of this header type by this function
                void register_header_type(header_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your header
                virtual header* generate() = 0 ;
            public:
                // you have to implement your own type() to return your header type
        	    virtual string type() = 0 ;
        	    // this function is used to generate any type of header derived
        	    static header * generate (string type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            			return prototypes[type]->generate(); // generate it!!
            		}
            		std::cerr << "no such header type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered header types: " << endl;
            	    for (map<string,header::header_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~header_generator(){};
        };

    protected:
        header():srcID(BROCAST_ID),dstID(BROCAST_ID),preID(BROCAST_ID),nexID(BROCAST_ID){} // this constructor cannot be directly called by users

    private:
        unsigned int srcID;
        unsigned int dstID;
        unsigned int preID;
        unsigned int nexID;
        header(header&){} // this constructor cannot be directly called by users
};
map<string,header::header_generator*> header::header_generator::prototypes;

class SDN_data_header : public header{
        SDN_data_header(SDN_data_header&){} // cannot be called by users

    protected:
        SDN_data_header(){} // this constructor cannot be directly called by users

    public:
        ~SDN_data_header(){}
        string type() { return "SDN_data_header"; }

        class SDN_data_header_generator;
        friend class SDN_data_header_generator;
        // SDN_data_header_generator is derived from header_generator to generate a header
        class SDN_data_header_generator : public header_generator{
                static SDN_data_header_generator sample;
                // this constructor is only for sample to register this header type
                SDN_data_header_generator() { /*cout << "SDN_data_header registered" << endl;*/ register_header_type(&sample); }
            protected:
                virtual header * generate(){
                    // cout << "SDN_data_header generated" << endl;
                    return new SDN_data_header;
                }
            public:
                virtual string type() { return "SDN_data_header";}
                ~SDN_data_header_generator(){}

        };
};
SDN_data_header::SDN_data_header_generator SDN_data_header::SDN_data_header_generator::sample;

class SDN_ctrl_header : public header{
        SDN_ctrl_header(SDN_ctrl_header&){} // cannot be called by users

    protected:
        SDN_ctrl_header(){} // this constructor cannot be directly called by users

    public:
        ~SDN_ctrl_header(){}
        string type() { return "SDN_ctrl_header"; }

        class SDN_ctrl_header_generator;
        friend class SDN_ctrl_header_generator;
        // SDN_ctrl_header_generator is derived from header_generator to generate a header
        class SDN_ctrl_header_generator : public header_generator{
                static SDN_ctrl_header_generator sample;
                // this constructor is only for sample to register this header type
                SDN_ctrl_header_generator() { /*cout << "SDN_ctrl_header registered" << endl;*/ register_header_type(&sample); }
            protected:
                virtual header * generate(){
                    // cout << "SDN_ctrl_header generated" << endl;
                    return new SDN_ctrl_header;
                }
            public:
                virtual string type() { return "SDN_ctrl_header";}
                ~SDN_ctrl_header_generator(){}

        };
};
SDN_ctrl_header::SDN_ctrl_header_generator SDN_ctrl_header::SDN_ctrl_header_generator::sample;

class SDN_stat_header : public header{
        SDN_stat_header(SDN_stat_header&){} // cannot be called by users

    protected:
        SDN_stat_header(){} // this constructor cannot be directly called by users

    public:
        ~SDN_stat_header(){}
        string type() { return "SDN_stat_header"; }

        class SDN_stat_header_generator;
        friend class SDN_stat_header_generator;
        // SDN_ctrl_header_generator is derived from header_generator to generate a header
        class SDN_stat_header_generator : public header_generator{
                static SDN_stat_header_generator sample;
                // this constructor is only for sample to register this header type
                SDN_stat_header_generator() { /*cout << "SDN_ctrl_header registered" << endl;*/ register_header_type(&sample); }
            protected:
                virtual header * generate(){
                    // cout << "SDN_ctrl_header generated" << endl;
                    return new SDN_stat_header;
                }
            public:
                virtual string type() { return "SDN_stat_header";}
                ~SDN_stat_header_generator(){}

        };
};
SDN_stat_header::SDN_stat_header_generator SDN_stat_header::SDN_stat_header_generator::sample;

class payload {
        payload(payload&){} // this constructor cannot be directly called by users

        string msg;

    protected:
        payload(){}
    public:
        virtual ~payload(){}
        virtual string type() = 0;

        SET(setMsg,string,msg,_msg);
        GET(getMsg,string,msg);

        class payload_generator {
                // lock the copy constructor
                payload_generator(payload_generator &){}
                // store all possible types of header
                static map<string,payload_generator*> prototypes;
            protected:
                // allow derived class to use it
                payload_generator() {}
                // after you create a new payload type, please register the factory of this payload type by this function
                void register_payload_type(payload_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your payload
                virtual payload* generate() = 0;
            public:
                // you have to implement your own type() to return your header type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of header derived
        	    static payload * generate (string type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            			return prototypes[type]->generate(); // generate it!!
            		}
            		std::cerr << "no such payload type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered payload types: " << endl;
            	    for (map<string,payload::payload_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~payload_generator(){};
        };
};
map<string,payload::payload_generator*> payload::payload_generator::prototypes;


class SDN_data_payload : public payload {
        SDN_data_payload(SDN_data_payload&){}

    protected:
        SDN_data_payload(){} // this constructor cannot be directly called by users
    public:
        ~SDN_data_payload(){}

        string type() { return "SDN_data_payload"; }

        class SDN_data_payload_generator;
        friend class SDN_data_payload_generator;
        // SDN_data_payload is derived from payload_generator to generate a payload
        class SDN_data_payload_generator : public payload_generator{
                static SDN_data_payload_generator sample;
                // this constructor is only for sample to register this payload type
                SDN_data_payload_generator() { /*cout << "SDN_data_payload registered" << endl;*/ register_payload_type(&sample); }
            protected:
                virtual payload * generate(){
                    // cout << "SDN_data_payload generated" << endl;
                    return new SDN_data_payload;
                }
            public:
                virtual string type() { return "SDN_data_payload";}
                ~SDN_data_payload_generator(){}
        };
};
SDN_data_payload::SDN_data_payload_generator SDN_data_payload::SDN_data_payload_generator::sample;

class SDN_ctrl_payload : public payload {
        SDN_ctrl_payload(SDN_ctrl_payload&){}

        unsigned int matID; // match: target
        unsigned int actID; // action: the next hop

    protected:
        SDN_ctrl_payload(){} // this constructor cannot be directly called by users
    public:
        ~SDN_ctrl_payload(){}

        string type() { return "SDN_ctrl_payload"; }

        SET(setMatID,unsigned int,matID,_matID);
        GET(getMatID,unsigned int,matID);
        SET(setActID,unsigned int,actID,_actID);
        GET(getActID,unsigned int,actID);


        class SDN_ctrl_payload_generator;
        friend class SDN_ctrl_payload_generator;
        // SDN_data_payload is derived from payload_generator to generate a payload
        class SDN_ctrl_payload_generator : public payload_generator{
                static SDN_ctrl_payload_generator sample;
                // this constructor is only for sample to register this payload type
                SDN_ctrl_payload_generator() { /*cout << "SDN_ctrl_payload registered" << endl;*/ register_payload_type(&sample); }
            protected:
                virtual payload * generate(){
                    // cout << "SDN_ctrl_payload generated" << endl;
                    return new SDN_ctrl_payload;
                }
            public:
                virtual string type() { return "SDN_ctrl_payload";}
                ~SDN_ctrl_payload_generator(){}
        };
};
SDN_ctrl_payload::SDN_ctrl_payload_generator SDN_ctrl_payload::SDN_ctrl_payload_generator::sample;


class SDN_stat_payload : public payload {
        SDN_stat_payload(SDN_stat_payload&){}

        //unsigned int matID;
        //unsigned int actID;
        vector<pair<unsigned int, unsigned int>> info;

    protected:
        SDN_stat_payload(){} // this constructor cannot be directly called by users
    public:
        ~SDN_stat_payload(){}

        string type() { return "SDN_stat_payload"; }

        SET(setInfo,Type,info,_statinfo);
        GET(getInfo,Type,info);



        class SDN_stat_payload_generator;
        friend class SDN_stat_payload_generator;
        // SDN_data_payload is derived from payload_generator to generate a payload
        class SDN_stat_payload_generator : public payload_generator{
                static SDN_stat_payload_generator sample;
                // this constructor is only for sample to register this payload type
                SDN_stat_payload_generator() { /*cout << "SDN_stat_payload registered" << endl;*/ register_payload_type(&sample); }
            protected:
                virtual payload * generate(){
                    // cout << "SDN_stat_payload generated" << endl;
                    return new SDN_stat_payload;
                }
            public:
                virtual string type() { return "SDN_stat_payload";}
                ~SDN_stat_payload_generator(){}
        };
};
SDN_stat_payload::SDN_stat_payload_generator SDN_stat_payload::SDN_stat_payload_generator::sample;


class packet{
        // a packet usually contains a header and a payload
        header *hdr;
        payload *pld;
        unsigned int p_id;
        static unsigned int last_packet_id ;

        packet(packet &) {}
        static int live_packet_num ;
    protected:
        // these constructors cannot be directly called by users
        packet(): hdr(nullptr), pld(nullptr) { p_id=last_packet_id++; live_packet_num ++; }
        packet(string _hdr, string _pld, bool rep = false, unsigned int rep_id = 0) {
            if (! rep ) // a duplicated packet does not have a new packet id
                p_id = last_packet_id ++;
            else
                p_id = rep_id;
            hdr = header::header_generator::generate(_hdr);
            pld = payload::payload_generator::generate(_pld);
            live_packet_num ++;
        }
    public:
        virtual ~packet(){
            // cout << "packet destructor begin" << endl;
            if (hdr != nullptr)
                delete hdr;
            if (pld != nullptr)
                delete pld;
            live_packet_num --;
            // cout << "packet destructor end" << endl;
        }

        SET(setHeader,header*,hdr,_hdr);
        GET(getHeader,header*,hdr);
        SET(setPayload,payload*,pld,_pld);
        GET(getPayload,payload*,pld);
        GET(getPacketID,unsigned int,p_id);

        static void discard ( packet* &p ) {
            // cout << "checking" << endl;
            if (p != nullptr) {
                // cout << "discarding" << endl;
                // cout << p->type() << endl;
                delete p;
                // cout << "discarded" << endl;
            }
            p = nullptr;
            // cout << "checked" << endl;
        }
        virtual string type () = 0;

        static int getLivePacketNum () { return live_packet_num; }

        class packet_generator;
        friend class packet_generator;
        class packet_generator {
                // lock the copy constructor
                packet_generator(packet_generator &){}
                // store all possible types of packet
                static map<string,packet_generator*> prototypes;
            protected:
                // allow derived class to use it
                packet_generator() {}
                // after you create a new packet type, please register the factory of this payload type by this function
                void register_packet_type(packet_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your payload
                virtual packet* generate ( packet *p = nullptr) = 0;
            public:
                // you have to implement your own type() to return your packet type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of packet derived
        	    static packet * generate (string type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            			return prototypes[type]->generate(); // generate it!!
            		}
            		std::cerr << "no such packet type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static packet * replicate (packet *p) {
            	    if(prototypes.find(p->type()) != prototypes.end()){ // if this type derived exists
            			return prototypes[p->type()]->generate(p); // generate it!!
            		}
            		std::cerr << "no such packet type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered packet types: " << endl;
            	    for (map<string,packet::packet_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~packet_generator(){};
        };
};
map<string,packet::packet_generator*> packet::packet_generator::prototypes;
unsigned int packet::last_packet_id = 0 ;
int packet::live_packet_num = 0;


// this packet is used to tell the destination the msg
class SDN_data_packet: public packet {
        SDN_data_packet(SDN_data_packet &) {}

    protected:
        SDN_data_packet(){} // this constructor cannot be directly called by users
        SDN_data_packet(packet*p): packet(p->getHeader()->type(), p->getPayload()->type(), true, p->getPacketID()) {
            *(dynamic_cast<SDN_data_header*>(this->getHeader())) = *(dynamic_cast<SDN_data_header*> (p->getHeader()));
            *(dynamic_cast<SDN_data_payload*>(this->getPayload())) = *(dynamic_cast<SDN_data_payload*> (p->getPayload()));
            //DFS_path = (dynamic_cast<SDN_data_header*>(p))->DFS_path;
            //isVisited = (dynamic_cast<SDN_data_header*>(p))->isVisited;
        } // for duplicate
        SDN_data_packet(string _h, string _p): packet(_h,_p) {}

    public:
        virtual ~SDN_data_packet(){}
        string type() { return "SDN_data_packet"; }

        class SDN_data_packet_generator;
        friend class SDN_data_packet_generator;
        // SDN_data_packet is derived from packet_generator to generate a pub packet
        class SDN_data_packet_generator : public packet_generator{
                static SDN_data_packet_generator sample;
                // this constructor is only for sample to register this packet type
                SDN_data_packet_generator() { /*cout << "SDN_data_packet registered" << endl;*/ register_packet_type(&sample); }
            protected:
                virtual packet *generate (packet *p = nullptr){
                    // cout << "SDN_data_packet generated" << endl;
                    if ( nullptr == p )
                        return new SDN_data_packet("SDN_data_header","SDN_data_payload");
                    else
                        return new SDN_data_packet(p); // duplicate
                }
            public:
                virtual string type() { return "SDN_data_packet";}
                ~SDN_data_packet_generator(){}
        };
};
SDN_data_packet::SDN_data_packet_generator SDN_data_packet::SDN_data_packet_generator::sample;

class SDN_ctrl_packet: public packet {
        SDN_ctrl_packet(SDN_ctrl_packet &) {}

    protected:
        SDN_ctrl_packet(){} // this constructor cannot be directly called by users
        SDN_ctrl_packet(packet*p): packet(p->getHeader()->type(), p->getPayload()->type(), true, p->getPacketID()) {
            *(dynamic_cast<SDN_ctrl_header*>(this->getHeader())) = *(dynamic_cast<SDN_ctrl_header*> (p->getHeader()));
            *(dynamic_cast<SDN_ctrl_payload*>(this->getPayload())) = *(dynamic_cast<SDN_ctrl_payload*> (p->getPayload()));
            //DFS_path = (dynamic_cast<SDN_ctrl_header*>(p))->DFS_path;
            //isVisited = (dynamic_cast<SDN_ctrl_header*>(p))->isVisited;
        } // for duplicate
        SDN_ctrl_packet(string _h, string _p): packet(_h,_p) {}

    public:
        virtual ~SDN_ctrl_packet(){}
        string type() { return "SDN_ctrl_packet"; }

        class SDN_ctrl_packet_generator;
        friend class SDN_ctrl_packet_generator;
        // SDN_ctrl_packet is derived from packet_generator to generate a pub packet
        class SDN_ctrl_packet_generator : public packet_generator{
                static SDN_ctrl_packet_generator sample;
                // this constructor is only for sample to register this packet type
                SDN_ctrl_packet_generator() { /*cout << "SDN_ctrl_packet registered" << endl;*/ register_packet_type(&sample); }
            protected:
                virtual packet *generate (packet *p = nullptr){
                    // cout << "SDN_ctrl_packet generated" << endl;
                    if ( nullptr == p )
                        return new SDN_ctrl_packet("SDN_ctrl_header","SDN_ctrl_payload");
                    else
                        return new SDN_ctrl_packet(p); // duplicate
                }
            public:
                virtual string type() { return "SDN_ctrl_packet";}
                ~SDN_ctrl_packet_generator(){}
        };
};
SDN_ctrl_packet::SDN_ctrl_packet_generator SDN_ctrl_packet::SDN_ctrl_packet_generator::sample;

class SDN_stat_packet: public packet {
        SDN_stat_packet(SDN_stat_packet &) {}

    protected:
        SDN_stat_packet(){} // this constructor cannot be directly called by users
        SDN_stat_packet(packet*p): packet(p->getHeader()->type(), p->getPayload()->type(), true, p->getPacketID()) {
            *(dynamic_cast<SDN_stat_header*>(this->getHeader())) = *(dynamic_cast<SDN_stat_header*> (p->getHeader()));
            *(dynamic_cast<SDN_stat_payload*>(this->getPayload())) = *(dynamic_cast<SDN_stat_payload*> (p->getPayload()));
            //DFS_path = (dynamic_cast<SDN_ctrl_header*>(p))->DFS_path;
            //isVisited = (dynamic_cast<SDN_ctrl_header*>(p))->isVisited;
        } // for duplicate
        SDN_stat_packet(string _h, string _p): packet(_h,_p) {}

    public:
        virtual ~SDN_stat_packet(){}
        string type() { return "SDN_stat_packet"; }

        class SDN_stat_packet_generator;
        friend class SDN_stat_packet_generator;
        // SDN_stat_packet is derived from packet_generator to generate a pub packet
        class SDN_stat_packet_generator : public packet_generator{
                static SDN_stat_packet_generator sample;
                // this constructor is only for sample to register this packet type
                SDN_stat_packet_generator() { /*cout << "SDN_stat_packet registered" << endl;*/ register_packet_type(&sample); }
            protected:
                virtual packet *generate (packet *p = nullptr){
                    // cout << "SDN_stat_packet generated" << endl;
                    if ( nullptr == p )
                        return new SDN_stat_packet("SDN_stat_header","SDN_stat_payload");
                    else
                        return new SDN_stat_packet(p); // duplicate
                }
            public:
                virtual string type() { return "SDN_stat_packet";}
                ~SDN_stat_packet_generator(){}
        };
};
SDN_stat_packet::SDN_stat_packet_generator SDN_stat_packet::SDN_stat_packet_generator::sample;



class node {
        // all nodes created in the program
        static map<unsigned int, node*> id_node_table;

        unsigned int id;
        map<unsigned int,bool> phy_neighbors;

    protected:
        node(node&){} // this constructor should not be used
        node(){} // this constructor should not be used
        node(unsigned int _id): id(_id) { id_node_table[_id] = this; }
    public:

        map<unsigned int, unsigned int> oldW; // store the old weight
        map<unsigned int, unsigned int> newW; // store the new weight
        map<unsigned int, unsigned int> oldm; // store the old weight's entry< DstID, NextID >
        map<unsigned int, unsigned int> newm; // store the new weight's entry< DstID, NextID >
        //vector<vector<unsigned int >> level; // <DstID, level>
        vector<pair<unsigned int,int>> level; // <DstID, level>


        virtual ~node() { // erase the node
            id_node_table.erase (id) ;
        }
        virtual string type() = 0; // please define it in your derived node class

        void add_phy_neighbor (unsigned int _id, string link_type = "simple_link"); // we only add a directed link from id to _id
        void del_phy_neighbor (unsigned int _id); // we only delete a directed link from id to _id

        // you can use the function to get the node's neigbhors at this time
        // but in the project 3, you are not allowed to use this function
        const map<unsigned int,bool> & getPhyNeighbors () {
            return phy_neighbors;
        }


        void recv (packet *p) {
            packet *tp = p;
            recv_handler(tp);
            packet::discard(p);
        } // the packet will be directly deleted after the handler
        void send (packet *p);

        // receive the packet and do something; this is a pure virtual function
        virtual void recv_handler(packet *p) = 0;
        void send_handler(packet *P);

        static node * id_to_node (unsigned int _id) { return ((id_node_table.find(_id)!=id_node_table.end()) ? id_node_table[_id]: nullptr) ; }
        GET(getNodeID,unsigned int,id);

        static void del_node (unsigned int _id) {
            if (id_node_table.find(_id) != id_node_table.end())
                id_node_table.erase(_id);
        }
        static unsigned int getNodeNum () { return id_node_table.size(); }

        class node_generator {
                // lock the copy constructor
                node_generator(node_generator &){}
                // store all possible types of node
                static map<string,node_generator*> prototypes;
            protected:
                // allow derived class to use it
                node_generator() {}
                // after you create a new node type, please register the factory of this node type by this function
                void register_node_type(node_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your node
                virtual node* generate(unsigned int _id) = 0;
            public:
                // you have to implement your own type() to return your node type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of node derived
        	    static node * generate (string type, unsigned int _id) {
        	        if(id_node_table.find(_id)!=id_node_table.end()){
        	            std::cerr << "duplicate node id" << std::endl; // node id is duplicated
        	            return nullptr;
        	        }
        	        else if ( BROCAST_ID == _id ) {
        	            cerr << "BROCAST_ID cannot be used" << endl;
        	            return nullptr;
        	        }
            		else if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            		    node * created_node = prototypes[type]->generate(_id);
            			return created_node; // generate it!!
            		}
            		std::cerr << "no such node type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered node types: " << endl;
            	    for (map<string,node::node_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~node_generator(){};
        };
};
map<string,node::node_generator*> node::node_generator::prototypes;
map<unsigned int,node*> node::id_node_table;

class SDN_switch: public node {
        // map<unsigned int,bool> one_hop_neighbors; // you can use this variable to record the node's 1-hop neighbors

        //bool hi; // this is used for example; you can remove it when doing hw2
        map<unsigned int, unsigned int> route; // store route <DstID, NextID>

    protected:
        SDN_switch() {} // it should not be used
        SDN_switch(SDN_switch&) {} // it should not be used
        SDN_switch(unsigned int _id): node(_id) {} // this constructor cannot be directly called by users

    public:
        ~SDN_switch(){}
        string type() { return "SDN_switch"; }

        // please define recv_handler function to deal with the incoming packet
        virtual void recv_handler (packet *p);

        // void add_one_hop_neighbor (unsigned int n_id) { one_hop_neighbors[n_id] = true; }
        // unsigned int get_one_hop_neighbor_num () { return one_hop_neighbors.size(); }

        class SDN_switch_generator;
        friend class SDN_switch_generator;
        // SDN_switch is derived from node_generator to generate a node
        class SDN_switch_generator : public node_generator{
                static SDN_switch_generator sample;
                // this constructor is only for sample to register this node type
                SDN_switch_generator() { /*cout << "SDN_switch registered" << endl;*/ register_node_type(&sample); }
            protected:
                virtual node * generate(unsigned int _id){ /*cout << "SDN_switch generated" << endl;*/ return new SDN_switch(_id); }
            public:
                virtual string type() { return "SDN_switch";}
                ~SDN_switch_generator(){}
        };
};
SDN_switch::SDN_switch_generator SDN_switch::SDN_switch_generator::sample;

class SDN_controller: public node {
        // map<unsigned int,bool> one_hop_neighbors; // you can use this variable to record the node's 1-hop neighbors

        vector<unsigned int>visit;
    protected:
        SDN_controller() {} // it should not be used
        SDN_controller(SDN_controller&) {} // it should not be used
        SDN_controller(unsigned int _id): node(_id) {} // this constructor cannot be directly called by users

    public:
        int round = 0;
        ~SDN_controller(){}
        string type() { return "SDN_controller"; }

        // please define recv_handler function to deal with the incoming packet
        virtual void recv_handler (packet *p);
        // void add_one_hop_neighbor (unsigned int n_id) { one_hop_neighbors[n_id] = true; }
        // unsigned int get_one_hop_neighbor_num () { return one_hop_neighbors.size(); }

        //static void Level();
        class SDN_controller_generator;
        friend class SDN_controller_generator;
        // SDN_switch is derived from node_generator to generate a node
        class SDN_controller_generator : public node_generator{
                static SDN_controller_generator sample;
                // this constructor is only for sample to register this node type
                SDN_controller_generator() { /*cout << "SDN_switch registered" << endl;*/ register_node_type(&sample); }
            protected:
                virtual node * generate(unsigned int _id){ /*cout << "SDN_switch generated" << endl;*/ return new SDN_controller(_id); }
            public:
                virtual string type() { return "SDN_controller";}
                ~SDN_controller_generator(){}
        };
};
SDN_controller::SDN_controller_generator SDN_controller::SDN_controller_generator::sample;



class mycomp {
    bool reverse;

    public:
        mycomp(const bool& revparam = false) { reverse=revparam ; }
        bool operator() (const event* lhs, const event* rhs) const;
};

class event {
        event(event*&){} // this constructor cannot be directly called by users
        static priority_queue < event*, vector < event* >, mycomp > events;
        static unsigned int cur_time; // timer
        static unsigned int end_time;

        // get the next event
        static event * get_next_event() ;
        static void add_event (event *e) { events.push(e); }
        static hash<string> event_seq;

    protected:
        unsigned int trigger_time;

        event(){} // it should not be used
        event(unsigned int _trigger_time): trigger_time(_trigger_time) {}

    public:
        virtual void trigger()=0;
        virtual ~event(){}

        virtual unsigned int event_priority() const = 0;
        unsigned int get_hash_value(string string_for_hash) const {
            unsigned int priority = event_seq (string_for_hash);
            return priority;
        }

        static void flush_events (); // only for debug

        GET(getTriggerTime,unsigned int,trigger_time);

        static void start_simulate( unsigned int _end_time ); // the function is used to start the simulation

        static unsigned int getCurTime() { return cur_time ; }
        static void getCurTime(unsigned int _cur_time) { cur_time = _cur_time; }
        // static unsigned int getEndTime() { return end_time ; }
        // static void getEndTime(unsigned int _end_time) { end_time = _end_time; }

        virtual void print () const = 0; // the function is used to print the event information

        class event_generator{
                // lock the copy constructor
                event_generator(event_generator &){}
                // store all possible types of event
                static map<string,event_generator*> prototypes;
            protected:
                // allow derived class to use it
                event_generator() {}
                // after you create a new event type, please register the factory of this event type by this function
                void register_event_type(event_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your event
                virtual event* generate(unsigned int _trigger_time, void * data) = 0;
            public:
                // you have to implement your own type() to return your event type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of event derived
        	    static event * generate (string type, unsigned int _trigger_time, void * data) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            		    event * e = prototypes[type]->generate(_trigger_time, data);
            		    add_event(e);
            		    return e; // generate it!!
            		}
            		std::cerr << "no such event type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered event types: " << endl;
            	    for (map<string,event::event_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~event_generator(){}
        };
};
map<string,event::event_generator*> event::event_generator::prototypes;
priority_queue < event*, vector< event* >, mycomp > event::events;
hash<string> event::event_seq;

unsigned int event::cur_time = 0;
unsigned int event::end_time = 0;

void event::flush_events()
{
    cout << "**flush begin" << endl;
    while ( ! events.empty() ) {
        cout << setw(11) << events.top()->trigger_time << ": " << setw(11) << events.top()->event_priority() << endl;
        delete events.top();
        events.pop();
    }
    cout << "**flush end" << endl;
}
event * event::get_next_event() {
    if(events.empty())
        return nullptr;
    event * e = events.top();
    events.pop();
    // cout << events.size() << " events remains" << endl;
    return e;
}
void event::start_simulate(unsigned int _end_time) {
    if (_end_time<0) {
        cerr << "you should give a possitive value of _end_time" << endl;
        return;
    }
    end_time = _end_time;
    event *e;
    e = event::get_next_event ();
    while ( e != nullptr && e->trigger_time <= end_time ) {
        if ( cur_time <= e->trigger_time )
            cur_time = e->trigger_time;
        else {
            cerr << "cur_time = " << cur_time << ", event trigger_time = " << e->trigger_time << endl;
            break;
        }

        // cout << "event trigger_time = " << e->trigger_time << endl;
        e->print(); // for log
        // cout << " event begin" << endl;
        e->trigger();
        // cout << " event end" << endl;
        delete e;
        e = event::get_next_event ();
    }
    // cout << "no more event" << endl;
}

bool mycomp::operator() (const event* lhs, const event* rhs) const {
    // cout << lhs->getTriggerTime() << ", " << rhs->getTriggerTime() << endl;
    // cout << lhs->type() << ", " << rhs->type() << endl;
    unsigned int lhs_pri = lhs->event_priority();
    unsigned int rhs_pri = rhs->event_priority();
    // cout << "lhs hash = " << lhs_pri << endl;
    // cout << "rhs hash = " << rhs_pri << endl;

    if (reverse)
        return ((lhs->getTriggerTime()) == (rhs->getTriggerTime())) ? (lhs_pri < rhs_pri): ((lhs->getTriggerTime()) < (rhs->getTriggerTime()));
    else
        return ((lhs->getTriggerTime()) == (rhs->getTriggerTime())) ? (lhs_pri > rhs_pri): ((lhs->getTriggerTime()) > (rhs->getTriggerTime()));
}

class recv_event: public event {
    public:
        class recv_data; // forward declaration

    private:
        recv_event(recv_event&) {} // this constructor cannot be used
        recv_event() {} // we don't allow users to new a recv_event by themselv
        unsigned int senderID; // the sender
        unsigned int receiverID; // the receiver; the packet will be given to the receiver
        packet *pkt; // the packet

    protected:
        // this constructor cannot be directly called by users; only by generator
        recv_event(unsigned int _trigger_time, void *data): event(_trigger_time), senderID(BROCAST_ID), receiverID(BROCAST_ID), pkt(nullptr){
            recv_data * data_ptr = (recv_data*) data;
            senderID = data_ptr->s_id;
            receiverID = data_ptr->r_id; // the packet will be given to the receiver
            pkt = data_ptr->_pkt;
        }

    public:
        virtual ~recv_event(){}
        // recv_event will trigger the recv function
        virtual void trigger();

        unsigned int event_priority() const;

        class recv_event_generator;
        friend class recv_event_generator;
        // recv_event is derived from event_generator to generate a event
        class recv_event_generator : public event_generator{
                static recv_event_generator sample;
                // this constructor is only for sample to register this event type
                recv_event_generator() { /*cout << "recv_event registered" << endl;*/ register_event_type(&sample); }
            protected:
                virtual event * generate(unsigned int _trigger_time, void *data){
                    // cout << "recv_event generated" << endl;
                    return new recv_event(_trigger_time, data);
                }

            public:
                virtual string type() { return "recv_event";}
                ~recv_event_generator(){}
        };
        // this class is used to initialize the recv_event
        class recv_data{
            public:
                unsigned int s_id;
                unsigned int r_id;
                packet *_pkt;
        };

        void print () const;
};
recv_event::recv_event_generator recv_event::recv_event_generator::sample;

void recv_event::trigger() {
    if (pkt == nullptr) {
        cerr << "recv_event error: no pkt!" << endl;
        return ;
    }
    else if (node::id_to_node(receiverID) == nullptr){
        cerr << "recv_event error: no node " << receiverID << "!" << endl;
        delete pkt; return ;
    }
    node::id_to_node(receiverID)->recv(pkt);
}
unsigned int recv_event::event_priority() const {
    string string_for_hash;
    string_for_hash = to_string(getTriggerTime()) + to_string(senderID) + to_string (receiverID) + to_string (pkt->getPacketID());
    return get_hash_value(string_for_hash);
}
// the recv_event::print() function is used for log file
void recv_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime()
         << "   recID"       << setw(11) << receiverID
         << "   pktID"       << setw(11) << pkt->getPacketID()
         << "   srcID"       << setw(11) << pkt->getHeader()->getSrcID()
         << "   dstID"       << setw(11) << pkt->getHeader()->getDstID()
         << "   preID"       << setw(11) << pkt->getHeader()->getPreID()
         << "   nexID"       << setw(11) << pkt->getHeader()->getNexID()
         << "   "            << pkt->type()
         << endl;
    // cout << pkt->type()
    //      << "   time "       << setw(11) << event::getCurTime()
    //      << "   recID "      << setw(11) << receiverID
    //      << "   pktID"       << setw(11) << pkt->getPacketID()
    //      << "   srcID "      << setw(11) << pkt->getHeader()->getSrcID()
    //      << "   dstID"       << setw(11) << pkt->getHeader()->getDstID()
    //      << "   preID"       << setw(11) << pkt->getHeader()->getPreID()
    //      << "   nexID"       << setw(11) << pkt->getHeader()->getNexID()
    //      << endl;
}

class send_event: public event {
    public:
        class send_data; // forward declaration

    private:
        send_event (send_event &){}
        send_event (){} // we don't allow users to new a recv_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int senderID; // the sender
        unsigned int receiverID; // the receiver
        packet *pkt; // the packet

    protected:
        send_event (unsigned int _trigger_time, void *data): event(_trigger_time), senderID(BROCAST_ID), receiverID(BROCAST_ID), pkt(nullptr){
            send_data * data_ptr = (send_data*) data;
            senderID = data_ptr->s_id;
            receiverID = data_ptr->r_id;
            pkt = data_ptr->_pkt;
        }

    public:
        virtual ~send_event(){}
        // send_event will trigger the send function
        virtual void trigger();

        unsigned int event_priority() const;

        class send_event_generator;
        friend class send_event_generator;
        // send_event is derived from event_generator to generate a event
        class send_event_generator : public event_generator{
                static send_event_generator sample;
                // this constructor is only for sample to register this event type
                send_event_generator() { /*cout << "send_event registered" << endl;*/ register_event_type(&sample); }
            protected:
                virtual event * generate(unsigned int _trigger_time, void *data){
                    // cout << "send_event generated" << endl;
                    return new send_event(_trigger_time, data);
                }

            public:
                virtual string type() { return "send_event";}
                ~send_event_generator(){}
        };
        // this class is used to initialize the send_event
        class send_data{
            public:
                unsigned int s_id;
                unsigned int r_id;
                packet *_pkt;
                unsigned int t;
        };

        void print () const;
};
send_event::send_event_generator send_event::send_event_generator::sample;

void send_event::trigger() {
    if (pkt == nullptr) {
        cerr << "send_event error: no pkt!" << endl;
        return ;
    }
    else if (node::id_to_node(senderID) == nullptr){
        cerr << "send_event error: no node " << senderID << "!" << endl;
        delete pkt; return ;
    }
    node::id_to_node(senderID)->send(pkt);
}
unsigned int send_event::event_priority() const {
    string string_for_hash;
    string_for_hash = to_string(getTriggerTime()) + to_string(senderID) + to_string (receiverID) + to_string (pkt->getPacketID());
    return get_hash_value(string_for_hash);
}
// the send_event::print() function is used for log file
void send_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime()
         << "   senID"       << setw(11) << senderID
         << "   pktID"       << setw(11) << pkt->getPacketID()
         << "   srcID"       << setw(11) << pkt->getHeader()->getSrcID()
         << "   dstID"       << setw(11) << pkt->getHeader()->getDstID()
         << "   preID"       << setw(11) << pkt->getHeader()->getPreID()
         << "   nexID"       << setw(11) << pkt->getHeader()->getNexID()
         << "   "            << pkt->type()
         //<< "   msg"         << setw(11) << dynamic_cast<SDN_data_payload*>(pkt->getPayload())->getMsg()
         << endl;
    // cout << pkt->type()
    //      << "   time "       << setw(11) << event::getCurTime()
    //      << "   senID "      << setw(11) << senderID
    //      << "   pktID"       << setw(11) << pkt->getPacketID()
    //      << "   srcID "      << setw(11) << pkt->getHeader()->getSrcID()
    //      << "   dstID"       << setw(11) << pkt->getHeader()->getDstID()
    //      << "   preID"       << setw(11) << pkt->getHeader()->getPreID()
    //      << "   nexID"       << setw(11) << pkt->getHeader()->getNexID()
    //      << endl;
}

////////////////////////////////////////////////////////////////////////////////

class SDN_data_pkt_gen_event: public event {
    public:
        class gen_data; // forward declaration

    private:
        SDN_data_pkt_gen_event (SDN_data_pkt_gen_event &){}
        SDN_data_pkt_gen_event (){} // we don't allow users to new a recv_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int src; // the src
        unsigned int dst; // the dst
        // packet *pkt; // the packet
        string msg;

    protected:
        SDN_data_pkt_gen_event (unsigned int _trigger_time, void *data): event(_trigger_time), src(BROCAST_ID), dst(BROCAST_ID){
            pkt_gen_data * data_ptr = (pkt_gen_data*) data;
            src = data_ptr->src_id;
            dst = data_ptr->dst_id;
            // pkt = data_ptr->_pkt;
            msg = data_ptr->msg;
        }

    public:
        virtual ~SDN_data_pkt_gen_event(){}
        // SDN_data_pkt_gen_event will trigger the packet gen function
        virtual void trigger();

        unsigned int event_priority() const;

        class SDN_data_pkt_gen_event_generator;
        friend class SDN_data_pkt_gen_event_generator;
        // SDN_data_pkt_gen_event_generator is derived from event_generator to generate an event
        class SDN_data_pkt_gen_event_generator : public event_generator{
                static SDN_data_pkt_gen_event_generator sample;
                // this constructor is only for sample to register this event type
                SDN_data_pkt_gen_event_generator() { /*cout << "send_event registered" << endl;*/ register_event_type(&sample); }
            protected:
                virtual event * generate(unsigned int _trigger_time, void *data){
                    // cout << "send_event generated" << endl;
                    return new SDN_data_pkt_gen_event(_trigger_time, data);
                }

            public:
                virtual string type() { return "SDN_data_pkt_gen_event";}
                ~SDN_data_pkt_gen_event_generator(){}
        };
        // this class is used to initialize the SDN_data_pkt_gen_event
        class pkt_gen_data{
            public:
                unsigned int src_id;
                unsigned int dst_id;
                string msg;
                // packet *_pkt;
        };

        void print () const;
};
SDN_data_pkt_gen_event::SDN_data_pkt_gen_event_generator SDN_data_pkt_gen_event::SDN_data_pkt_gen_event_generator::sample;

void SDN_data_pkt_gen_event::trigger() {
    if (node::id_to_node(src) == nullptr){
        cerr << "SDN_data_pkt_gen_event error: no node " << src << "!" << endl;
        return ;
    }
    else if ( dst != BROCAST_ID && node::id_to_node(dst) == nullptr ) {
        cerr << "SDN_data_pkt_gen_event error: no node " << dst << "!" << endl;
        return;
    }

    SDN_data_packet *pkt = dynamic_cast<SDN_data_packet*> ( packet::packet_generator::generate("SDN_data_packet") );
    if (pkt == nullptr) {
        cerr << "packet type is incorrect" << endl; return;
    }
    SDN_data_header *hdr = dynamic_cast<SDN_data_header*> ( pkt->getHeader() );
    SDN_data_payload *pld = dynamic_cast<SDN_data_payload*> ( pkt->getPayload() );

    if (hdr == nullptr) {
        cerr << "header type is incorrect" << endl; return ;
    }
    if (pld == nullptr) {
        cerr << "payload type is incorrect" << endl; return ;
    }

    hdr->setSrcID(src);
    hdr->setDstID(dst);
    hdr->setPreID(src); // this column is not important when the packet is first received by the src (i.e., just generated)
    hdr->setNexID(src); // this column is not important when the packet is first received by the src (i.e., just generated)

    pld->setMsg(msg);

    recv_event::recv_data e_data;
    e_data.s_id = src;
    e_data.r_id = src; // to make the packet start from the src
    e_data._pkt = pkt;

    recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event", trigger_time, (void *)&e_data) );

}
unsigned int SDN_data_pkt_gen_event::event_priority() const {
    string string_for_hash;
    string_for_hash = to_string(getTriggerTime()) + to_string(src) + to_string (dst) ; //to_string (pkt->getPacketID());
    return get_hash_value(string_for_hash);
}
// the SDN_data_pkt_gen_event::print() function is used for log file
void SDN_data_pkt_gen_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime()
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
         << "   srcID"       << setw(11) << src
         << "   dstID"       << setw(11) << dst
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
         << "   SDN_data_packet generating"
         << endl;
}

class SDN_ctrl_pkt_gen_event: public event {
    public:
        class gen_data; // forward declaration

    private:
        SDN_ctrl_pkt_gen_event (SDN_ctrl_pkt_gen_event &){}
        SDN_ctrl_pkt_gen_event (){} // we don't allow users to new a recv_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int src; // the src
        unsigned int dst; // the dst
        unsigned int mat;
        unsigned int act;
        // packet *pkt; // the packet
        string msg;

    protected:
        SDN_ctrl_pkt_gen_event (unsigned int _trigger_time, void *data): event(_trigger_time), src(BROCAST_ID), dst(BROCAST_ID){
            pkt_gen_data * data_ptr = (pkt_gen_data*) data;
            src = data_ptr->src_id;
            dst = data_ptr->dst_id;
            // pkt = data_ptr->_pkt;
            mat = data_ptr->mat_id;
            act = data_ptr->act_id;
            msg = data_ptr->msg;
        }

    public:
        virtual ~SDN_ctrl_pkt_gen_event(){}
        // SDN_ctrl_pkt_gen_event will trigger the packet gen function
        virtual void trigger();

        unsigned int event_priority() const;

        class SDN_ctrl_pkt_gen_event_generator;
        friend class SDN_ctrl_pkt_gen_event_generator;
        // SDN_ctrl_pkt_gen_event_generator is derived from event_generator to generate an event
        class SDN_ctrl_pkt_gen_event_generator : public event_generator{
                static SDN_ctrl_pkt_gen_event_generator sample;
                // this constructor is only for sample to register this event type
                SDN_ctrl_pkt_gen_event_generator() { /*cout << "send_event registered" << endl;*/ register_event_type(&sample); }
            protected:
                virtual event * generate(unsigned int _trigger_time, void *data){
                    // cout << "send_event generated" << endl;
                    return new SDN_ctrl_pkt_gen_event(_trigger_time, data);
                }

            public:
                virtual string type() { return "SDN_ctrl_pkt_gen_event";}
                ~SDN_ctrl_pkt_gen_event_generator(){}
        };
        // this class is used to initialize the SDN_ctrl_pkt_gen_event
        class pkt_gen_data{
            public:
                unsigned int src_id; // the controller
                unsigned int dst_id; // the node that should update its rule
                unsigned int mat_id; // the target of the rule
                unsigned int act_id; // the next hop toward the target recorded in the rule
                string msg;
                // packet *_pkt;
        };

        void print () const;
};
SDN_ctrl_pkt_gen_event::SDN_ctrl_pkt_gen_event_generator SDN_ctrl_pkt_gen_event::SDN_ctrl_pkt_gen_event_generator::sample;

void SDN_ctrl_pkt_gen_event::trigger() {
    if ( dst == BROCAST_ID || node::id_to_node(dst) == nullptr ) {
        cerr << "SDN_ctrl_pkt_gen_event error: no node " << dst << "!" << endl;
        return;
    }

    SDN_ctrl_packet *pkt = dynamic_cast<SDN_ctrl_packet*> ( packet::packet_generator::generate("SDN_ctrl_packet") );
    if (pkt == nullptr) {
        cerr << "packet type is incorrect" << endl; return;
    }
    SDN_ctrl_header *hdr = dynamic_cast<SDN_ctrl_header*> ( pkt->getHeader() );
    SDN_ctrl_payload *pld = dynamic_cast<SDN_ctrl_payload*> ( pkt->getPayload() );

    if (hdr == nullptr) {
        cerr << "header type is incorrect" << endl; return ;
    }
    if (pld == nullptr) {
        cerr << "payload type is incorrect" << endl; return ;
    }

    hdr->setSrcID(src);
    hdr->setDstID(dst);
    hdr->setPreID(src);
    hdr->setNexID(src); // in hw3, you should set NexID to src

    pld->setMsg(msg);
    pld->setMatID(mat);
    pld->setActID(act);

    recv_event::recv_data e_data;
    e_data.s_id = src;
    e_data.r_id = src; // in hw3, you should set r_id it src
    e_data._pkt = pkt;

    recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event",trigger_time, (void *)&e_data) );
}
unsigned int SDN_ctrl_pkt_gen_event::event_priority() const {
    string string_for_hash;
    string_for_hash = to_string(getTriggerTime()) + to_string(src) + to_string(dst) + to_string(mat) + to_string(act); //to_string (pkt->getPacketID());
    return get_hash_value(string_for_hash);
}
// the SDN_ctrl_pkt_gen_event::print() function is used for log file
void SDN_ctrl_pkt_gen_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime()
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
         << "   srcID"       << setw(11) << src
         << "   dstID"       << setw(11) << dst
         << "   matID"       << setw(11) << mat
         << "   actID"       << setw(11) << act
         << "   SDN_ctrl_packet generating"
         << endl;
}


class SDN_stat_pkt_gen_event: public event {
    public:
        class gen_data; // forward declaration

    private:
        SDN_stat_pkt_gen_event (SDN_stat_pkt_gen_event &){}
        SDN_stat_pkt_gen_event (){} // we don't allow users to new a recv_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int src; // the src
        unsigned int dst; // the dst
        vector <pair<unsigned int, unsigned int>> info;
        // packet *pkt; // the packet
        string msg;

    protected:
        SDN_stat_pkt_gen_event (unsigned int _trigger_time, void *data): event(_trigger_time), src(BROCAST_ID), dst(BROCAST_ID){
            pkt_gen_data * data_ptr = (pkt_gen_data*) data;
            src = data_ptr->src_id; // set src to dst_id
            dst = data_ptr->dst_id; // set dst to src_id
            info = data_ptr->info;
            msg = data_ptr->msg;
        }

    public:
        virtual ~SDN_stat_pkt_gen_event(){}
        // SDN_ctrl_pkt_gen_event will trigger the packet gen function
        virtual void trigger();

        unsigned int event_priority() const;

        class SDN_stat_pkt_gen_event_generator;
        friend class SDN_stat_pkt_gen_event_generator;
        // SDN_ctrl_pkt_gen_event_generator is derived from event_generator to generate an event
        class SDN_stat_pkt_gen_event_generator : public event_generator{
                static SDN_stat_pkt_gen_event_generator sample;
                // this constructor is only for sample to register this event type
                SDN_stat_pkt_gen_event_generator() { /*cout << "send_event registered" << endl;*/ register_event_type(&sample); }
            protected:
                virtual event * generate(unsigned int _trigger_time, void *data){
                    // cout << "send_event generated" << endl;
                    return new SDN_stat_pkt_gen_event(_trigger_time, data);
                }

            public:
                virtual string type() { return "SDN_stat_pkt_gen_event";}
                ~SDN_stat_pkt_gen_event_generator(){}
        };
        // this class is used to initialize the SDN_stat_pkt_gen_event
        class pkt_gen_data{
            public:
                unsigned int src_id; // the controller
                unsigned int dst_id; // the node that should update its rule
                vector <pair <unsigned int, unsigned int> > info; // stat_info
                string msg;
                // packet *_pkt;
        };

        void print () const;
};
SDN_stat_pkt_gen_event::SDN_stat_pkt_gen_event_generator SDN_stat_pkt_gen_event::SDN_stat_pkt_gen_event_generator::sample;

void SDN_stat_pkt_gen_event::trigger() {
    if ( dst == BROCAST_ID || node::id_to_node(dst) == nullptr ) {
        cerr << "SDN_stat_pkt_gen_event error: no node " << dst << "!" << endl;
        return;
    }

    SDN_stat_packet *pkt = dynamic_cast<SDN_stat_packet*> ( packet::packet_generator::generate("SDN_stat_packet") );
    if (pkt == nullptr) {
        cerr << "packet type is incorrect" << endl; return;
    }
    SDN_stat_header *hdr = dynamic_cast<SDN_stat_header*> ( pkt->getHeader() );
    SDN_stat_payload *pld = dynamic_cast<SDN_stat_payload*> ( pkt->getPayload() );

    if (hdr == nullptr) {
        cerr << "header type is incorrect" << endl; return ;
    }
    if (pld == nullptr) {
        cerr << "payload type is incorrect" << endl; return ;
    }

    hdr->setSrcID(src);
    hdr->setDstID(dst);
    hdr->setPreID(src);
    hdr->setNexID(src);

    pld->setMsg(msg);
    pld->setInfo(info);


    recv_event::recv_data e_data;
    e_data.s_id = src;
    e_data.r_id = src;
    e_data._pkt = pkt;

    recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event",trigger_time, (void *)&e_data) );
}

unsigned int SDN_stat_pkt_gen_event::event_priority() const {
    string string_for_hash;
    string t ="";
    for(int i = 0; i < info.size(); i++){
        unsigned int t1 = info[i].first, t2 = info[i].second;
        t = t+ to_string(t1) + to_string(t2);
    }
    string_for_hash = to_string(getTriggerTime()) + to_string(src) + to_string(dst) +t ;
    return get_hash_value(string_for_hash);
}

// the SDN_stat_pkt_gen_event::print() function is used for log file
void SDN_stat_pkt_gen_event::print () const {
    cout << "time "          << setw(11) << event::getCurTime()
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
         << "   srcID"       << setw(11) << src
         << "   dstID"       << setw(11) << dst
         << "        "       << setw(11) << " "
         << "        "       << setw(11) << " "
         << "   SDN_stat_packet generating"
         << endl;
}

////////////////////////////////////////////////////////////////////////////////

class link {
        // all links created in the program
        static map< pair<unsigned int,unsigned int>, link*> id_id_link_table;

        unsigned int id1; // from
        unsigned int id2; // to

    protected:
        link(link&){} // this constructor should not be used
        link(){} // this constructor should not be used
        link(unsigned int _id1, unsigned int _id2): id1(_id1), id2(_id2) { id_id_link_table[pair<unsigned int,unsigned int>(id1,id2)] = this; }

    public:
        virtual ~link() {
            id_id_link_table.erase (pair<unsigned int,unsigned int>(id1,id2)); // erase the link
        }

        static link * id_id_to_link (unsigned int _id1, unsigned int _id2) {
            return ((id_id_link_table.find(pair<unsigned int,unsigned int>(_id1,_id2))!=id_id_link_table.end()) ? id_id_link_table[pair<unsigned,unsigned>(_id1,_id2)]: nullptr) ;
        }

        virtual double getLatency() = 0; // you must implement your own latency

        static void del_link (unsigned int _id1, unsigned int _id2) {
            pair<unsigned int, unsigned int> temp;
            if (id_id_link_table.find(temp)!=id_id_link_table.end())
                id_id_link_table.erase(temp);
        }

        static unsigned int getLinkNum () { return id_id_link_table.size(); }

        class link_generator {
                // lock the copy constructor
                link_generator(link_generator &){}
                // store all possible types of link
                static map<string,link_generator*> prototypes;
            protected:
                // allow derived class to use it
                link_generator() {}
                // after you create a new link type, please register the factory of this link type by this function
                void register_link_type(link_generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your link
                virtual link* generate(unsigned int _id1, unsigned int _id2) = 0;
            public:
                // you have to implement your own type() to return your link type
        	    virtual string type() = 0;
        	    // this function is used to generate any type of link derived
        	    static link * generate (string type, unsigned int _id1, unsigned int _id2) {
        	        if(id_id_link_table.find(pair<unsigned int,unsigned int>(_id1,_id2))!=id_id_link_table.end()){
        	            std::cerr << "duplicate link id" << std::endl; // link id is duplicated
        	            return nullptr;
        	        }
        	        else if ( BROCAST_ID == _id1 || BROCAST_ID == _id2 ) {
        	            cerr << "BROCAST_ID cannot be used" << endl;
        	            return nullptr;
        	        }
            		else if (prototypes.find(type) != prototypes.end()){ // if this type derived exists
            		    link * created_link = prototypes[type]->generate(_id1,_id2);
            			return created_link; // generate it!!
            		}
            		std::cerr << "no such link type" << std::endl; // otherwise
            		return nullptr;
            	}
            	static void print () {
            	    cout << "registered link types: " << endl;
            	    for (map<string,link::link_generator*>::iterator it = prototypes.begin(); it != prototypes.end(); it ++)
            	        cout << it->second->type() << endl;
            	}
            	virtual ~link_generator(){};
        };
};
map<string,link::link_generator*> link::link_generator::prototypes;
map<pair<unsigned int,unsigned int>, link*> link::id_id_link_table;

void node::add_phy_neighbor (unsigned int _id, string link_type){
    if (id == _id) return; // if the two nodes are the same...
    if (id_node_table.find(_id)==id_node_table.end()) return; // if this node does not exist
    if (phy_neighbors.find(_id)!=phy_neighbors.end()) return; // if this neighbor has been added
    phy_neighbors[_id] = true;

    link::link_generator::generate(link_type,id,_id);
}
void node::del_phy_neighbor (unsigned int _id){
    phy_neighbors.erase(_id);

}


class simple_link: public link {
    protected:
        simple_link() {} // it should not be used outside the class
        simple_link(simple_link&) {} // it should not be used
        simple_link(unsigned int _id1, unsigned int _id2): link (_id1,_id2){} // this constructor cannot be directly called by users

    public:
        virtual ~simple_link() {}
        virtual double getLatency() { return ONE_HOP_DELAY; } // you can implement your own latency

        class simple_link_generator;
        friend class simple_link_generator;
        // simple_link is derived from link_generator to generate a link
        class simple_link_generator : public link_generator {
                static simple_link_generator sample;
                // this constructor is only for sample to register this link type
                simple_link_generator() { /*cout << "simple_link registered" << endl;*/ register_link_type(&sample); }
            protected:
                virtual link * generate(unsigned int _id1, unsigned int _id2)
                { /*cout << "simple_link generated" << endl;*/ return new simple_link(_id1,_id2); }
            public:
                virtual string type() { return "simple_link"; }
                ~simple_link_generator(){}
        };
};

simple_link::simple_link_generator simple_link::simple_link_generator::sample;


// the data_packet_event function is used to add an initial event
void data_packet_event (unsigned int src, unsigned int dst, unsigned int t = 0, string msg = "default"){
    if ( node::id_to_node(src) == nullptr || (dst != BROCAST_ID && node::id_to_node(dst) == nullptr) ) {
        cerr << "src or dst is incorrect" << endl; return ;
        return;
    }

    SDN_data_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = dst;
    e_data.msg = msg;

    // recv_event *e = dynamic_cast<recv_event*> ( event::event_generator::generate("recv_event",t, (void *)&e_data) );
    SDN_data_pkt_gen_event *e = dynamic_cast<SDN_data_pkt_gen_event*> ( event::event_generator::generate("SDN_data_pkt_gen_event",t, (void *)&e_data) );
    if (e == nullptr) cerr << "event type is incorrect" << endl;
}

// the ctrl_packet_event function is used to add an initial event
void ctrl_packet_event (unsigned int con_id, unsigned int id, unsigned int mat, unsigned int act, unsigned int t = event::getCurTime(), string msg = "default"){
// void ctrl_packet_event (unsigned int dst, unsigned int t = 0, string msg = "default"){
    if ( id == BROCAST_ID || node::id_to_node(id) == nullptr ) {
        cerr << "id is incorrect" << endl; return;
    }

    unsigned int src = con_id;
    SDN_ctrl_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = id;
    e_data.mat_id = mat;
    e_data.act_id = act;
    e_data.msg = msg;

    SDN_ctrl_pkt_gen_event *e = dynamic_cast<SDN_ctrl_pkt_gen_event*> ( event::event_generator::generate("SDN_ctrl_pkt_gen_event",t, (void *)&e_data) );
    if (e == nullptr) cerr << "event type is incorrect" << endl;
}
// send_handler function is used to transmit packet p based on the information in the header
// Note that the packet p will not be discard after send_handler ()

void stat_packet_event (unsigned int con_id, unsigned int id, vector<pair<unsigned int,unsigned int>> info, unsigned int t = event::getCurTime(), string msg = "default"){
// void ctrl_packet_event (unsigned int dst, unsigned int t = 0, string msg = "default"){
    if ( id == BROCAST_ID || node::id_to_node(id) == nullptr ) {
        cerr << "id is incorrect" << endl; return;
    }

    unsigned int src = con_id;
    SDN_stat_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = id;
    e_data.dst_id = src;
    e_data.info = info;
    e_data.msg = msg;

    SDN_stat_pkt_gen_event *e = dynamic_cast<SDN_stat_pkt_gen_event*> ( event::event_generator::generate("SDN_stat_pkt_gen_event",t, (void *)&e_data) );
    if (e == nullptr) cerr << "event type is incorrect" << endl;
}

void node::send_handler(packet *p){
    packet *_p = packet::packet_generator::replicate(p);
    send_event::send_data e_data;
    e_data.s_id = _p->getHeader()->getPreID();
    e_data.r_id = _p->getHeader()->getNexID();
    e_data._pkt = _p;
    send_event *e = dynamic_cast<send_event*> (event::event_generator::generate("send_event",event::getCurTime(), (void *)&e_data) );
    if (e == nullptr) cerr << "event type is incorrect" << endl;
}

void node::send(packet *p){ // this function is called by event; not for the user
    if (p == nullptr) return;

    unsigned int _nexID = p->getHeader()->getNexID();
    for ( map<unsigned int,bool>::iterator it = phy_neighbors.begin(); it != phy_neighbors.end(); it ++) {
        unsigned int nb_id = it->first; // neighbor id

        if (nb_id != _nexID && BROCAST_ID != _nexID) continue; // this neighbor will not receive the packet

        unsigned int trigger_time = event::getCurTime() + link::id_id_to_link(id, nb_id)->getLatency() ; // we simply assume that the delay is fixed
        // cout << "node " << id << " send to node " <<  nb_id << endl;
        recv_event::recv_data e_data;
        e_data.s_id = id;    // set the sender   (i.e., preID)
        e_data.r_id = nb_id; // set the receiver (i.e., nexID)

        packet *p2 = packet::packet_generator::replicate(p);
        e_data._pkt = p2;

        recv_event *e = dynamic_cast<recv_event*> (event::event_generator::generate("recv_event", trigger_time, (void*) &e_data)); // send the packet to the neighbor
        if (e == nullptr) cerr << "event type is incorrect" << endl;
    }
    packet::discard(p);
}

void shortestPath( unsigned int nodes, unsigned int dst, int judge, int parent[]){ // ��ؼ��I��U�I���̵u�Z��

    unsigned int d[nodes];// �����I��ؼ��I��weight
    int visit[nodes]; // visit�аO�O�_���X�L;parent�O����next node
    for( int i = 0; i < nodes; i++ ){// �N�C�I��ؼ��I���Z����l�Ʀ�INT_MAX
        d[i] = MAX; // �N�C�I��ؼ��I���Z����l�Ʀ�INT_MAX
        visit[i] = -1; // ��l��-1
        parent[i] = -1; // ��l��-1
    } // for

    d[dst] = 0;
    // �N�ؼ��I��ؼ��I���Z���]��0�A�ñq���I�}�l����neighbor���U�h

    for(int i = 0; i < nodes;i++){ // ���j��ΨӱN�C�I�]�L�@��
        unsigned int min= MAX; // ���Nmin�]��INT_MAX
        int a = -1;
        for(int j = 0; j< nodes; j++){
        //�M��|�����X�L���I�A�B���I�Z����ؼ��I����MAX�B���̤p��
            if( visit[j] != 1 && d[j] < min ){
                a = j; // a�ΨӦs��Z���ؼ��I�̤p���I
                min = d[j];
            } // if
        } // for

        if( a == -1 )    break;
        if( min == MAX )    break;
        visit[a] = 1;

        if(judge == 0){
            // judge�ΨӧP�_�ثe�O�n�@old weight�]judge == 0�^���̵u���|�٬Onew weight�]judge == 1�^
            const map<unsigned int,bool> &nblist = node::id_to_node(a)->getPhyNeighbors();

            map<unsigned int,bool>::const_iterator it = nblist.begin();

            for( int k = 0 ; it != nblist.end() ; k++,++it){
                // �A�q�W����쪺�I�h��s��F�~��ۤv���̵u�Z���A�ñN�ؼ��I(dst)�H�Ψ�U��node(a)�s�Jmap(tmp)
                // 1. �p�G��F�~�w���X�άO��s�᪺�Z���j�󤧫e���N����s
                // 2. �p�G��s�ᤧ�Z��������e�����N�����parent�A
                //    �ÿ��node���p����parent�M���sparent[]

                unsigned int tmp = it->first;
                if(it->first == nodes) continue;
                auto t = node::id_to_node(a)->oldW.find(tmp);
                unsigned int weight = t->second;
                if( visit[tmp] != 1 && d[a] + weight < d[tmp] ){
                    d[tmp] = d[a] + weight;
                    parent[tmp] = a;
                    node::id_to_node(tmp)->oldm[dst]= a;
                } // if
                else if( visit[tmp] != 1 && d[a] + weight == d[tmp] && a < parent[tmp] ){
                    parent[tmp] = a;
                    node::id_to_node(tmp)->oldm[dst]= a;
                } // else if

            } // for
        } // if
        else if( judge == 1 ){
            const map<unsigned int,bool> &nblist = node::id_to_node(a)->getPhyNeighbors();
            map<unsigned int,bool>::const_iterator it = nblist.begin();
            for( int k = 0 ;  it != nblist.end(); k++, ++it){
                unsigned int tmp = it->first;
                if(it->first == nodes) continue;
                auto t = node::id_to_node(a)->newW.find(tmp);
                unsigned int weight = t->second;
                if( visit[tmp] != 1 && d[a] + weight < d[tmp] ){
                    d[tmp] = d[a] + weight;
                    parent[tmp] = a;
                    node::id_to_node(tmp)->newm[dst]= a;
                }
                else if( visit[tmp] != 1 && d[a] + weight == d[tmp] && a < parent[tmp] ){
                    parent[tmp] = a;
                    node::id_to_node(tmp)->newm[dst]= a;
                } // else if
            } // for
        } // else if
    } // for
}

void update(unsigned int nodes, int parent[], unsigned int dst){

    int r[nodes] = {0}; // store index node's round
    vector<int> pre; // last round nodes
    vector<int> tmp; // tmp
    int n = 0; // count num of nodes are already set
    pre.push_back(dst); // let pre first is destination

    while(n < nodes-1 ){
        for(unsigned int id = 0; id < nodes;id++){
            if(find(pre.begin(), pre.end(), parent[id])!=pre.end()){
                // find pre has its next id or not
                if(node::id_to_node(id)->oldm[dst] != node::id_to_node(id)->newm[dst]){
                    //if pre has its next id andits routing is updated(next id)
                    updateTable[r[parent[id]]].push_back(make_pair(id, dst)); // update updateTable
                    r[id] = r[node::id_to_node(id)->newm[dst]]+1;//update id's round
                } // if
                tmp.push_back(id); // push the found id to tmp
                n++;
            } // if
        } // for

        pre = tmp; // update pre
        tmp.clear(); // clear pre
    } // while

    for(int j : r ){ //set totalRound
        if( (j-1) > totalRound ) totalRound = j-1;
    } // for
}

void SDN_controller::recv_handler (packet *p){

    if (p == nullptr) return ;

    if (p->type() == "SDN_ctrl_packet" && p->getHeader()->getDstID() == getNodeID() ) { // recieve from node
        SDN_ctrl_packet *p3 = nullptr;
        p3 = dynamic_cast<SDN_ctrl_packet*> (p);
        SDN_ctrl_payload *l3 = nullptr;
        l3 = dynamic_cast<SDN_ctrl_payload*> (p3->getPayload());
        string msg = l3->getMsg();
        if(msg == "install") return;
        for(long unsigned int i = 0; i < updateTable[round].size();i++){
            // record ctrl_packet send from node
            pair<int, int> tmp = updateTable[round][i];
            if(tmp.first == p3->getHeader()->getPreID() && tmp.second == l3->getMatID()){
                updateTable[round][i].first = -1;
                updateTable[round][i].second = -1;
                break;
            } // if
        } // for

        long unsigned int k = 0; //Record the num of SDN ctrl packets that have been sent from the node in this round
        for(long unsigned int i = 0; i < updateTable[round].size();i++){
            pair<int, int> tmp = updateTable[round][i];
            if(tmp.first == -1) k++;
        } // for
        if(k == updateTable[round].size()){
            // if k == num of this round need to update, then round+1 to next round
            // if round is equal to totalround => return
            //round++;
            if(round == totalRound)   return;
            round++;
            for(long unsigned int i = 0; i < updateTable[round].size(); i++){
                // send next round ctrl_packet
                map<unsigned int ,unsigned int>::iterator it = node::id_to_node(updateTable[round][i].first)->newm.begin();
                for(; it!= node::id_to_node(updateTable[round][i].first)->newm.end(); ++it){
                    if(it->first == updateTable[round][i].second){
                        ctrl_packet_event(getNodeNum()-1, updateTable[round][i].first, it->first, it->second );
                    } // if
                } // for
            } // for
        } // if
    } // if
    else if(p->type() == "SDN_ctrl_packet" ){ // send to node
        SDN_ctrl_packet *p3 = nullptr;
        p3 = dynamic_cast<SDN_ctrl_packet*> (p);
        p3->getHeader()->setNexID(p->getHeader()->getDstID()); // set the NextID to node ID
        send_handler(p3);
    } // else
    else if(p->type() == "SDN_stat_packet"){

        if( visit.size() < nodes) { // set visit vector to sizes of nodes
            visit.resize(nodes);
            fill(visit.begin(), visit.end(), 0);  // initialize
        }

        SDN_stat_packet *p3 = nullptr;
        p3 = dynamic_cast<SDN_stat_packet*> (p);
        SDN_stat_payload *l3 = nullptr;
        l3 = dynamic_cast<SDN_stat_payload*> (p3->getPayload());
        string msg = l3->getMsg();

        int i = p3->getHeader()->getSrcID(); // id
        visit[i] = 1; // record send from SDN_Switch
        int k = 0; // used to count num of nodes have already received
        for(int i : visit){
            if( i == 0 ) k =1;
        }

        if( k == 0 ) { // received all nodes
            int parent[nodes];
            if( msg == "install" ){
                for(int dst:dstID) shortestPath(nodes, dst, 0, parent);
                for (unsigned int id = 0; id < node::getNodeNum()-1; id ++){
                    // inorder generate ctrl_packet_event(install)
                    map<unsigned int ,unsigned int>::iterator it = node::id_to_node(id)->oldm.begin();
                    for(; it!= node::id_to_node(id)->oldm.end(); ++it){
                        ctrl_packet_event(con_id, id, it->first, it->second, event::getCurTime(),"install");
                        // msg:"install" to let me judge it is installation or update(in SDN_controller's recv_handler)
                    } // for
                } // for
                // init the visit vector
                visit.clear();
                visit.resize(nodes);
                fill(visit.begin(), visit.end(), 0);
            }
            else if( msg == "update"){
                for(int dst:dstID) {
                    shortestPath(nodes, dst, 1,parent);
                    update(nodes, parent,dst); // update order
                } // for
                updateTable.resize(totalRound+1); // set updateTable size to correct size
                for(long unsigned int i = 0; i < updateTable[0].size(); i++){
                    // generate first round ctrl_packet_event(update)
                    map<unsigned int ,unsigned int>::iterator it = node::id_to_node(updateTable[0][i].first)->newm.begin();
                    for(; it!= node::id_to_node(updateTable[0][i].first)->newm.end(); ++it){
                        if(it->first == updateTable[0][i].second){ // dst
                            ctrl_packet_event(con_id, updateTable[0][i].first, it->first, it->second);
                        }
                    }
                }
            }
        }
    }
}


void SDN_switch::recv_handler (packet *p){

    if (p == nullptr) return ;

    if (p->type() == "SDN_data_packet" ) { // the switch receives a packet from the other switch
        SDN_data_packet * p2 = nullptr;
        p2 = dynamic_cast<SDN_data_packet*> (p);
        auto it = route.find(p->getHeader()->getDstID()); // find the DstID from the cur_node's routing table

        if( it == route.end() ) return; // if didn't find it => do nothing
        else if( it != route.end() && getNodeID() != p->getHeader()->getDstID() ){
            // if find and cur_node's ID is different to DstID
            // set the packet of PreID to cur_Node's ID
            //                   NexID to routing table's NextID
            //                   DstID to the recv_packet's DstID
            p2->getHeader()->setPreID ( getNodeID() );
            p2->getHeader()->setNexID ( it->second );
            p2->getHeader()->setDstID ( p->getHeader()->getDstID() );
            send_handler (p2);
        } // else if
    } // if
    else if (p->type() == "SDN_ctrl_packet") { // the switch receives a packet from the controller
        SDN_ctrl_packet *p3 = nullptr;
        p3 = dynamic_cast<SDN_ctrl_packet*> (p);
        SDN_ctrl_payload *l3 = nullptr;
        l3 = dynamic_cast<SDN_ctrl_payload*> (p3->getPayload());
        unsigned int mat = l3->getMatID();
        unsigned int act = l3->getActID();
        route[mat]= act;
        unsigned int src = p->getHeader()->getSrcID();
        unsigned int nex = p->getHeader()->getNexID();
        unsigned int pre = p->getHeader()->getPreID();
        p3->getHeader()->setNexID(pre);
        p3->getHeader()->setDstID(src);
        p3->getHeader()->setPreID(nex);
        send_handler(p3);
    } // else if
    else if( p->type() == "SDN_stat_packet") {
        SDN_stat_packet *p3 = nullptr;
        p3 = dynamic_cast<SDN_stat_packet*> (p);
        p3->getHeader()->setNexID( p->getHeader()->getDstID());
        send_handler(p3);
    }

}

int main()
{
    // header::header_generator::print(); // print all registered headers
    // payload::payload_generator::print(); // print all registered payloads
    // packet::packet_generator::print(); // print all registered packets
    // node::node_generator::print(); // print all registered nodes
    // event::event_generator::print(); // print all registered events
    // link::link_generator::print(); // print all registered links

    // read the input and generate switch nodes
    unsigned int dsts, links;
    cout << "#Nodes    #Dsts    #Links"<<endl;
    cin >> nodes >> dsts >> links;
    for (unsigned int id = 0; id < nodes; id ++){
        node::node_generator::generate("SDN_switch",id);
    } // for

    unsigned int insTime, updTime, simDuration;
    cout << "InsTime   UpdTime    SimDuration"<<endl;
    cin >> insTime >> updTime >> simDuration;

    unsigned int t;
    cout << "DstID"<<endl;
    for( unsigned int i = 0 ; i < dsts ; i++ ) { // cin the dstID
        cin >> t;
        dstID.push_back(t);
    } // for

    con_id = node::getNodeNum();
    node::node_generator::generate("SDN_controller",con_id);
    // set switches' neighbors
    cout << "LinkID    Node1    Node2    oldW    newW\n";
    for( unsigned int i = 0 ; i < links ; i++ ){
        unsigned int n1, n2, ow, nw;
        cin >> t >> n1 >> n2 >> ow >> nw;
        node::id_to_node(n1)->add_phy_neighbor(n2);
        node::id_to_node(n1)->oldW[n2] = ow;
        node::id_to_node(n1)->newW[n2] = nw;
        node::id_to_node(n2)->add_phy_neighbor(n1);
        node::id_to_node(n2)->oldW[n1] = ow;
        node::id_to_node(n2)->newW[n1] = nw;
    } // for

    for( unsigned int id = 0; id < nodes; id++){
        node::id_to_node(id)->add_phy_neighbor(con_id);
        node::id_to_node(con_id)->add_phy_neighbor(id);
    }

    // generate all initial events that you want to simulate in the networks
    unsigned int src = 0, dst = 0;
    // read the input and use data_packet_event to add an initial event
    cout << "Time    SrcID    DstID\n";
    while(cin >> t){
        cin >> src >> dst;
        data_packet_event(src, dst, t);
    } // while

    updateTable.resize(nodes*2);
    // set updateTable size to double of nodes(temporary)
    // row's index : round, column : pair(id,dst)
    cout<<endl;
        cout  << setw(11)<< "time "          << setw(18)
         << "   recID"       << setw(11) << " "
         << "   pktID"       << setw(11) << " "
         << "   srcID"       << setw(20)
         << "   dstID"       << setw(18)
         << "   preID"       << setw(11) << " "
         << "   nxtID"       << setw(15) << " "
         << "packet type"
         << endl;
    cout<<"---------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    for (unsigned int id = 0; id < node::getNodeNum()-1; id ++){
            const map<unsigned int,bool> &nblist = node::id_to_node(id)->getPhyNeighbors();
            vector< pair<unsigned int, unsigned int> > info; // store curnode's neiID and old weight
            for(auto it = nblist.begin(); it != nblist.end()&& it->first != con_id ;++it){
                auto w = node::id_to_node(id)->oldW.find(it->first);
                info.push_back(make_pair(w->first, w->second));
            }
            stat_packet_event (con_id, id, info, insTime, "install"); // generate stat_packet_event of installation

    } // for

    for (unsigned int id = 0; id < node::getNodeNum()-1; id ++){
            const map<unsigned int,bool> &nblist = node::id_to_node(id)->getPhyNeighbors();
            vector< pair<unsigned int, unsigned int> > info;// store curnode's neiID and new weight
            for(auto it = nblist.begin(); it != nblist.end()&& it->first != con_id ;++it){
                auto w = node::id_to_node(id)->newW.find(it->first);
                info.push_back(make_pair(w->first, w->second));
            }
            stat_packet_event (con_id, id, info, updTime,"update"); // generate stat_packet_event of update

    } // for
    // start simulation!!
    event::start_simulate(simDuration);
    // event::flush_events() ;
    // cout << packet::getLivePacketNum() << endl;
    return 0;
}
