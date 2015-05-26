#include "hunter-include.h"
#include "conf.h"
#include "cache_key.h"

class cache_key_factory{
	public:
		int load(config& conf);

	private:
		std::map<std::string, cache_key> m_key_map;
		std::map< std::string, std::vector<std::string> > m_table_desc;
		config*							 m_conf;
};
