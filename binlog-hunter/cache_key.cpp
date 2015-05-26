#include "cache_key.h"

int cache_key::add(const char* name, int value){
	if(NULL == name)
		return -1;

	char buf[32];
	sprintf(buf, "%d", value);
	buf[31] = 0;
	fields[name] = buf;
	return 0;
}

int cache_key::add(const char* name, const char* value){
	if(NULL == name || NULL == value)
		return -1;

	fields[name] = value;
	return 0;
}

std::string cache_key::gen_key(){
	std::string key = prefix != "" ? prefix+split : "";
	for(size_t i = 0; i < fields_order.size(); i++){
		if(i == 0)
			key += fields[fields_order[i]];
		else
			key += split + fields[fields_order[i]];
	}
	return suffix != "" ? key+split+suffix : key;
}
