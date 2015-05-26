#include "cache.h"

int cache::load(config& conf){
	m_memc = memcached_create(NULL);
	int port = conf["cache"]["port"].to_int();
	LOG_IF(INFO, g_debug_flag)<<"cache server port="<<port;
	elem_group& server_desc = conf["cache"]["server"];
	if(server_desc.size() == 0){
		LOG(ERROR)<<"no cache server in [cache]server";
		return -1;
	}

	memcached_return rc;
	memcached_server_st *servers = NULL;
	for(int i = 0; i < server_desc.size(); i++){
		LOG_IF(INFO, g_debug_flag)<<"add server conf="<<server_desc[i].to_cstr();
		servers = memcached_server_list_append(servers, server_desc[i].to_cstr(), port, &rc);
	}
	rc = memcached_server_push(m_memc, servers);
	if(rc != MEMCACHED_SUCCESS){
		LOG(ERROR)<<"push server failed "<<rc<<"("<<memcached_strerror(0, rc)<<")";
		return rc;
	}
	memcached_server_free(servers);
	return 0;
}

int cache::del(cache_key* key){
	time_t expiration = 0;
	std::string key_str = key->gen_key(); 
	memcached_return rc=memcached_delete(m_memc,key_str.c_str(), key_str.length(), expiration);
	if(rc==MEMCACHED_SUCCESS){
		LOG_IF(INFO, g_debug_flag)<<"del cache key("<<key_str.c_str()<<") success";
		return MEMCACHED_SUCCESS;
	}else if(rc == MEMCACHED_NOTFOUND){
		LOG_IF(INFO, g_debug_flag)<<"cache key("<<key_str.c_str()<<") not found";
		return MEMCACHED_SUCCESS;
	}
	LOG(ERROR)<<"del cache key("<<key_str.c_str()<<") failed "<<rc<<"("<<memcached_strerror(0, rc)<<")";
	return rc;
}

int cache_key_factory::load(config& conf){
	m_conf = &conf;
	elem_group& db = conf["cache"]["db"];
	for(size_t i = 0; i < db.size(); i++){
		LOG_IF(INFO, g_debug_flag)<<"load cache config for db "<<db[i].to_string()<<"...";
		config db_key_map(("conf/"+db[i].to_string()+".conf").c_str());
		for(config::iterator it = db_key_map.begin(); 
				it != db_key_map.end();
				it++){
			std::string  table = it->first;
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
		}
	}

	return 0;
}

cache_key* cache_key_factory::table2key(std::string& db, std::string& table, mysql::Row_of_fields& fields){
	std::string idx = db+table;
	if(m_key_map.find(idx) == m_key_map.end()){
		LOG(ERROR)<<"unsupported key : db="<<db<<" table="<<table;
		return NULL;
	}

	cache_key& ck = m_key_map[idx];
	std::vector<std::string>& desc = m_table_desc[idx];

	mysql::Converter converter;
	ck.clear();
	for(int i = 0; i < desc.size(); i++){
		std::string str;
		converter.to(str, fields[i]);
		ck.add(desc[i].c_str(), str.c_str());
	}

	return &ck;
}
