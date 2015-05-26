#include "cache.h"

int cache_key_factory::load(config& conf){
	m_conf = &conf;
	elem_group& db = conf["solr"]["db"];
	for(size_t i = 0; i < db.size(); i++){
		LOG_IF(INFO, g_debug_flag)<<"load cache config for db "<<db[i].to_string()<<"...";
		config db_key_map(("conf/"+db[i].to_string()+".conf").c_str());
		for (config::iterator it = db_key_map.begin(); it != db_key_map.end(); it++) {
			std::string  table = it->first;
			/*
			cache_key ck;
			ck.prefix = db_key_map[table.c_str()]["prefix"].to_string();
			ck.suffix = db_key_map[table.c_str()]["suffix"].to_string();
			ck.split = db_key_map[table.c_str()]["split"].to_string();

			elem_group& eg = db_key_map[table.c_str()]["field"];
			for(size_t j = 0; j < eg.size(); j++){
				ck.fields_order.push_back(eg[j].to_string());
			}

			elem_group& col = db_key_map[table.c_str()]["column"];
			std::vector<std::string> desc;
			for(size_t j = 0; j < col.size(); j++){
				desc.push_back(col[j].to_string());
			}

			std::string key = db[i].to_string()+table; 
			m_key_map[key] = ck;
			m_table_desc[key] = desc;	
			LOG_IF(INFO, g_debug_flag)<<"load key-map config : "
									<<"db["<<db[i].to_string()<<"] "
									<<"table["<<table<<"] "
									<<"prefix["<<ck.prefix<<"] "
									<<"suffix["<<ck.suffix<<"] "
									<<"split["<<ck.split<<"] ";
			*/
		}
	}

	return 0;
}
