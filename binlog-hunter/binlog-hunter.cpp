/*
Copyright (c) 2013, Oracle and/or its affiliates. All rights
reserved.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of
the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
02110-1301  USA
*/

#include "glog/logging.h"
using namespace std;
/*
  Here is a basic system using the event loop to fetch context events
  and store them in an associative array.
 */
using mysql::Binary_log;
using mysql::system::create_transport;
using mysql::system::get_event_type_str;
using mysql::User_var_event;

#define GID "gid"
bool g_debug_flag=false;
std::string g_binlogfile = "";
unsigned long g_offset = 4;

////////////////////
//class for table map event
typedef std::pair< long int, mysql::Table_map_event* > table_map_elem;
typedef std::map< long int, mysql::Table_map_event* > table_index_map;

class Table_Index : public mysql::Content_handler, public table_index_map
{
	public:
		mysql::Binary_log_event* process_event(mysql::Table_map_event* event){
			if(find(event->table_id) == end())
				insert(table_map_elem(event->table_id, event));
			return NULL;
		}

		~Table_Index(){
			for( table_index_map::iterator it = begin();
					it != end();
					it++){
				if(it->second){
					delete it->second;
					it->second = NULL;
				}
			}
		}
};

////////////////////
//class for incident event
class Incident_handler : public mysql::Content_handler
{
	public:
		mysql::Binary_log_event* process_event(mysql::Incident_event* incident){
			LOG(INFO)<<"Event type: "
					 << mysql::system::get_event_type_str(incident->get_event_type())
					 << " length: " << incident->header()->event_length
				     << " next pos: " << incident->header()->next_position;
			LOG(INFO)<< "type= "
					 << (unsigned)incident->type
					 << " message= "
					 << incident->message;
				    /* Consume the event */
			delete incident;
			return NULL;
		}
};

///////////////////
//class for rotate event
class rotate_handler : public mysql::Content_handler
{
	public:
		mysql::Binary_log_event *process_event(mysql::Rotate_event *ev){
			LOG(INFO)<<"change binlog position to "<<ev->binlog_file<<":"<<ev->binlog_pos;
			g_binlogfile = ev->binlog_file;
			g_offset = ev->binlog_pos;
			return ev;
		}
};

////////////////////
//class for xid event
class xid_handler : public mysql::Content_handler
{
	public:
		mysql::Binary_log_event *process_event(mysql::Xid *ev){
			g_offset = ev->header()->next_position;
			return ev;
		}
};

////////////////////
//class for row event
class row_handler : public mysql::Content_handler
{
	public:
		row_handler(Table_Index* ti) : m_ti(ti),  m_conf(NULL) {}   

		int init(config* conf){
			m_conf = conf;
			if(0 != m_cache_key.load(*conf)){
				return -2;
			}
			
			return 0;
		}

		mysql::Binary_log_event* process_event(mysql::Row_event* event){
			int table_id = event->table_id;
			table_index_map::iterator tit = m_ti->find(table_id);
			if(tit == m_ti->end()){
				LOG(ERROR)<< "binlog["<<g_binlogfile<<"] offset["<<g_offset
						  << "] tableid["<<table_id
						  << "] table info was not registered by any preceding table map event.";
				return event;
			}

			mysql::Row_event_set rows(event, tit->second);
			string db_name = tit->second->db_name;
			string table_name = tit->second->table_name;

			//hack
			if (table_name != "bat" && table_name != "brd_goods_sku_info") {
				return event;
			}

			if(event->get_event_type() != mysql::WRITE_ROWS_EVENT &&
					event->get_event_type() != mysql::WRITE_ROWS_EVENT_V1 &&
					event->get_event_type() != mysql::UPDATE_ROWS_EVENT &&
					event->get_event_type() != mysql::UPDATE_ROWS_EVENT_V1 &&
					event->get_event_type() != mysql::DELETE_ROWS_EVENT &&
					event->get_event_type() != mysql::DELETE_ROWS_EVENT_V1){
				LOG(ERROR)<<"unsupported row event "
					<<"binlog["<<g_binlogfile<<"] offset["<<g_offset<<"] tableid["<<table_id
					<<"] event["<<event->get_event_type()<<"]";
				return event;
			}
			
			LOG_IF(INFO, g_debug_flag)<<"process a row event "
				<<"binlog["<<g_binlogfile<<"] offset["<<g_offset<<"] tableid["<<table_id<<"] event["<<event->get_event_type()<<"]";

			try{
				mysql::Row_event_set::iterator it = rows.begin();
				do{
					mysql::Row_of_fields fields = *it;
					long int timestamp = event->header()->timestamp;

					/*
					cache_key* ck = m_cache_key.table2key(db_name, table_name, fields);
					if(ck){
						int ret = 0;
						for(int i = 0; i < 3; i++){
							if( (ret = m_cache.del(ck)) == 0 )
								break;
						}
						if(0 == ret)
							LOG(INFO)<<"errno[0] time["<<timestamp<<"] db["<<db_name<<"] table["<<table_name
								<<"] binlog["<<g_binlogfile<<"] offset["<<g_offset<<"] event["<<event->get_event_type()
								<<"] del cache key["<<ck->gen_key()<<"]";
						else
							LOG(INFO)<<"errno["<<ret<<"] time["<<timestamp<<"] db["<<db_name<<"] table["<<table_name
								<<"] binlog["<<g_binlogfile<<"] offset["<<g_offset<<"] event["<<event->get_event_type()
								<<"] del cache key["<<ck->gen_key()<<"]";
					}
					*/
				}while(++it != rows.end());
			}catch(const std::logic_error& le){
				LOG(ERROR)<< "MySQL Data Type error: " << le.what();
			}

			delete event;
			return NULL;
		}

		void print_row(const std::string& db, const std::string& table, long int timestamp, const mysql::Row_of_fields& fields){
			mysql::Converter converter;
			int col = 0;
			int field_index_cnt = 0;

			std::string row_id;
			LOG_IF(INFO, g_debug_flag)<<"timestamp: "<<timestamp<<" size: "<<fields.size()
				<<" db: "<<db<<" table: "<<table;

			std::string data = "";
			for(int i = 0; i < fields.size(); i++){
				std::string str;
				converter.to(str, fields[i]);
				data += "|"+str;
			}
			LOG_IF(INFO, g_debug_flag)<<data<<"|";
		}

	private:
		Table_Index* m_ti;
		Binary_log* m_bin;
		config* m_conf;
		cache_key_factory m_cache_key;
};
//

int load_position(std::string& file, unsigned long& offset){
	ifstream ifs(GID, ios::in);
    if(!ifs){
		file = "";
		offset = 4;
		return -1;
	}
	
	ifs>>file>>offset;
	return 0;	
}

int save_position(std::string& file, unsigned long offset){
	ofstream ofs(GID, ios::out);
    if(ofs){
		ofs<<file<<" "<<offset;
	}	
	return 0;
}


int main(int argc, char** argv) {
	google::InitGoogleLogging(argv[0]);
	google::SetLogDestination(google::INFO, "log/binlog-hunter.");
	google::SetLogDestination(google::ERROR, "log/binlog-hunter.wf.");
	
	config my_conf("conf/binlog-hunter.conf");
	int log_level = my_conf["log"]["debug"].to_int();
	if(log_level == 0)
		g_debug_flag = false;
	else
		g_debug_flag = true;

	std::string mysql_info = my_conf["mysql"]["url"].to_cstr();
	Binary_log binlog(create_transport(mysql_info.c_str()));
	if(0 != binlog.connect()){
		LOG(ERROR)<<"connect mysql failed, mysql url="<<mysql_info;
		return 0;	
	}
	LOG(INFO)<<"connect mysql success, mysql url="<<mysql_info;

	load_position(g_binlogfile, g_offset);
	LOG(INFO)<<"load binlog position info, file="<<g_binlogfile<<" offset="<<g_offset;
	if(g_binlogfile == "")
		binlog.set_position(g_offset);
	else
		binlog.set_position(g_binlogfile, g_offset);

	Table_Index ti_handler;
	Incident_handler inci_handler;
	rotate_handler rota_handler;
	xid_handler x_handler;
	row_handler rhandler(&ti_handler);
	if(0 != rhandler.init(&my_conf)){
		return -1;
	}

	binlog.content_handler_pipeline()->push_back(&ti_handler);
	binlog.content_handler_pipeline()->push_back(&inci_handler);
	binlog.content_handler_pipeline()->push_back(&rota_handler);
	binlog.content_handler_pipeline()->push_back(&x_handler);
	binlog.content_handler_pipeline()->push_back(&rhandler);

	while (true) {
		Binary_log_event *event;
		int result = binlog.wait_for_next_event(&event);
		if (result != ERR_OK){
			LOG(ERROR)<<"wait for next event error, ret="<<result;
			break;
		}
	
		save_position(g_binlogfile, g_offset);
		LOG_IF(INFO, g_debug_flag)<<"get event : "<<event->get_event_type();
  }
}
