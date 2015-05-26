#include "hunter-include.h"

class cache_key
{
	public:
		std::vector<std::string> fields_order;
		std::map<std::string, std::string> fields;

	public:
		virtual int add(const char* name, int value);
		virtual int add(const char* name, const char* value);
		virtual std::string gen_key();
		void clear() { fields.clear(); }
};

//custom
