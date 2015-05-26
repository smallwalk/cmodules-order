#include "hunter-include.h"
#include "conf.h"
#include "cache_key.h"

class cache_key_factory{
	public:
		int load(config& conf);
		cache_key* table2key(std::string& db, std::string& table, mysql::Row_of_fields& fields);

	private:
		std::map<std::string, cache_key> m_key_map;
		std::map< std::string, std::vector<std::string> > m_table_desc;
		config*							 m_conf;
};
