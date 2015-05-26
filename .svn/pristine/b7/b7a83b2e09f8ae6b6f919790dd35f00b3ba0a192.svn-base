#include "hunter-include.h"
#include "libmemcached/memcached.hpp"
#include "conf.h"
#include "cache_key.h"

class cache
{
	public:
		cache():
			m_memc(NULL){}

		~cache(){
			if(m_memc)
				memcached_free(m_memc);
			m_memc = NULL;
		}

		int load(config& conf);
		int del(cache_key* key);

	private:
		memcached_st*		m_memc;
		memcached_return	m_rc;	
};

class cache_key_factory{
	public:
		int load(config& conf);
		cache_key* table2key(std::string& db, std::string& table, mysql::Row_of_fields& fields);

	private:
		std::map<std::string, cache_key> m_key_map;
		std::map< std::string, std::vector<std::string> > m_table_desc;
		config*							 m_conf;
};
