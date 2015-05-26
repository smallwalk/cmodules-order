#include "conf.h"

elem_group& conf_group::operator[](const char* name){
	return m_group[name];
}

config::config(const char* file){
	std::ifstream ifs(file, std::ios::in);
	if(!ifs){
		m_init = false;
		LOG(ERROR)<<"load config file error, file="<<file;
		return;
	}

	m_file = file;
	std::string line;
	std::string group = "root";
	while(getline(ifs, line)){
		//check
		line = trim(line);
		if(line == "" || is_comment(line))
			continue;
	
		if(is_group(line)){
			//new group
			group = line.substr(1, line.length()-2);	
		}else{
			int split = line.find('=');
			if(split == std::string::npos)
				continue;
			std::string key = line.substr(0, split);
			std::string value = line.substr(split+1, line.length()-split-1);
			m_conf[group][key.c_str()].push_back(conf_elem(key, value));
		}
	}

	LOG(INFO)<<"load config file success, file="<<file;
	m_init = true;
}

std::string config::trim(std::string& org){
	const char* res = org.c_str();
	char des[1024];
	int cnt = 0;
	for(int i = 0; i < org.length(); i++){
		if(res[i] >= 33 && res[i] <= 126)
			des[cnt++] = res[i];
		if(cnt == 1024)
			return "";
	}
	des[cnt] = 0;
	return des;
}

bool config::is_group(std::string& line){
	if(line.length() == 0)
		return false;

	return line[0] == '[' && line[line.length()-1] == ']' ? true : false;
}

bool config::is_comment(std::string& line){
	if(line.length() < 2)
		return false;

	return line[0] == '/' && line[1] == '/' ? true : false;
}

conf_group& config::operator[](const char* name){
	return m_conf[name];
}
