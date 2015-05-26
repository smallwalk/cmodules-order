#include "cache.h"

int cache_key_factory::load(config& conf){
	m_conf = &conf;
	elem_group& db = conf["solr"]["db"];
	for(size_t i = 0; i < db.size(); i++){
		LOG_IF(INFO, g_debug_flag)<<"load cache config for db "<<db[i].to_string()<<"...";
		config db_key_map(("conf/"+db[i].to_string()+".conf").c_str());
		for (config::iterator it = db_key_map.begin(); it != db_key_map.end(); it++) {
			std::string  table = it->first;
			cache_key ck;

			elem_group& eg = db_key_map[table.c_str()]["field"];
			for (size_t j = 0; j < eg.size(); j++) {
				ck.fields_order.push_back(eg[j].to_string());
			}

			elem_group& col = db_key_map[table.c_str()]["column"];
			std::vector<std::string> desc;
			for (size_t j = 0; j < col.size(); j++) {
				desc.push_back(col[j].to_string());
			}

			std::string key = db[i].to_string() + table;
			m_key_map[key] = ck;
			m_table_desc[key] = desc;
		}
	}

	return 0;
}

cache_key* cache_key_factory::table2key(std::string& db, std::string& table, mysql::Row_of_fields& fields) {
	std::string idx = db + table;
	if (m_key_map.find(idx) == m_key_map.end()) {
		LOG(ERROR) << "unsupported key : db=" << db << " table=" << table;
		return NULL;
	}

	cache_key& ck = m_key_map[idx];
	std::vector<std::string>& desc = m_table_desc[idx];

	mysql::Converter converter;
	ck.clear();
	for (int i = 0; i < desc.size(); i++) {
		std::string str;
		converter.to(str, fields[i]);
		ck.add(desc[i].c_str(), str.c_str());
	}

	return &ck;
}
