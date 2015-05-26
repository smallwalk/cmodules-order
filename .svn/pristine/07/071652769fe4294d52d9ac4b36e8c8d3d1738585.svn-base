#include "hunter-include.h"
#include <vector>
#include <map>

class conf_elem{
	public:
		std::string key;
		std::string value;

	public:
		conf_elem(){}
		conf_elem(std::string& k, std::string& v)
			: key(k), value(v)	
		{}

		int to_int(){
			int ret = 0;
			sscanf(value.c_str(), "%d", &ret);
			return ret;
		};
		const char* to_cstr(){
			return value.c_str();
		};
		
		std::string& to_string(){
			return value;
		}
};

class elem_group : public  std::vector< conf_elem  >
{
	public:
		int to_int(){
			if(size() == 0)
				return 0;
			return (*this)[0].to_int();
		}

		const char* to_cstr(){
			if(size() == 0)
				return NULL;
		   return (*this)[0].to_cstr();	
		}

		std::string& to_string(){
			if(size() == 0)
				return m_null;
			return (*this)[0].to_string();
		}

	private:
		std::string m_null;
};

class conf_group{
	public:
		typedef std::map< std::string, elem_group >::iterator iterator;
		std::map< std::string, elem_group > m_group;

	public:
		elem_group& operator[](const char* name);
		iterator begin() { return m_group.begin(); }
		iterator end() { return m_group.end(); }
};

class config : public conf_group
{
	public:
		typedef std::map< std::string, conf_group >::iterator iterator;
		
		config(const char* file);
		conf_group& operator[](const char* name);
		iterator begin() { return m_conf.begin(); }
		iterator end() { return m_conf.end(); }

	private:
		std::string trim(std::string& org);
		bool is_group(std::string& line);
		bool is_comment(std::string& line);

	private:
		bool						m_init;
		std::string					m_file;	
		
		std::map< std::string, conf_group > m_conf;
};
