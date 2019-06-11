#ifndef	HASH_DICT_H
#define	HASH_DICT_H

#include	<string>
#include        <string.h>
#include	<fstream>
#include        <iostream>
#include        <assert.h>
#include        "util.h"
#include        "log.h"

using namespace	std;

const uint64 DEFAULT_DICT_SIZE = 12281;

const uint64 PRIMES[] = {
                53,        193,        769,       3079,       6151,       12281,
             24593,      49157,      98317,     196613,     393241,      786433,
            920219,    1226959,    1572869,    1829417,    2181271,     2439247,
           2908361,    3385997,    3877817,    4336439,    5170427,     5781961,
           6291469,    6893911,    7709287,    9191891,   12582917,    16341163,
          21788233,   29050993,   38734667,   50331653,   34189843,    39888179,
          46536209,   54292267,   63340987,   73897841,   86214169,   100583213,
         117347101,  136904951,  159722471,  186342883,  217400039,   253633381,
         295905619,  345223237,  402760451,  469887193,  548201729,   639568711,
         746163527,  870524119, 1015611473, 1184880097, 1382360123,  1612753489,
        1881545741, 2195136743, 2560992893, 2987825063, 3485795909,  4066761893,
        4744555559, 5535314819, 6457867337, 7534178579, 8789875037, 10254854273
};

template <class DataType>
class HashDict {
public:
	typedef struct {
		uint64          key;
		DataType          value;
		char            empty;
	} DICT_ITEM;
private:
	DICT_ITEM	*dict;
	uint64		dict_size;
	uint64		free_buckets;

	void ResizeHashDict();

public:
	HashDict();
	~HashDict();

	void Initial(uint64 ds = DEFAULT_DICT_SIZE);
	void Initial(ifstream &ifs);
	bool Dump(ofstream &ofs);
	void Free();
	void Clear();

	uint64 GetSize(){
		return dict_size-free_buckets;
	}
	void AddHashDict(HashDict*);
	void DelHashDict(HashDict *delDict);
	void UpdateHashDict(HashDict*);

	void ResizeToHashDict(HashDict*, int dict_num);

	bool AddItem(const uint64 key, DataType value);
	bool AddItem(const char *key, DataType value);
	bool AddItem(const string &key, DataType value);
	bool GetItem(const uint64 key, DataType &value);
	bool GetItem(const char *key, DataType &value);
	bool GetItem(const string &key, DataType &value);
	void ModifyItem(const uint64 key, DataType value);
	void ModifyItem(const char *key, DataType value);
	void ModifyItem(const string &key, DataType value);
};


template <class DataType>
HashDict<DataType>::HashDict()
{
	dict = NULL;
	dict_size = 0;
	free_buckets = 0;
}

template <class DataType>
HashDict<DataType>::~HashDict()
{
	Free();
}

template <class DataType>
void HashDict<DataType>::Free()
{
	if (dict != NULL) {
		delete[] dict;
		dict = NULL;
		dict_size = 0;
		free_buckets = 0;
	}
}

template <class DataType>
void HashDict<DataType>::Clear()
{
	assert(dict != NULL);
	free_buckets = dict_size;
	for (uint64 i = 0; i < dict_size; i ++) {
		dict[i].empty = 1;
	}
}

template <class DataType>
void HashDict<DataType>::Initial(uint64 ds)
{
	assert(dict == NULL);
	dict_size = ds;
	for (size_t i = 0; i < sizeof(PRIMES)/sizeof(uint64); i ++) {
		if (PRIMES[i] > ds) {
			dict_size = PRIMES[i];
			break;
		}
	}
	//log(LOG_NOTICE, "initail Hash dict size: %ld\n", dict_size);

	assert(dict_size > 0);

	dict = new DICT_ITEM[dict_size];
	assert(dict != NULL);

	free_buckets = dict_size;

	for (uint64 i = 0; i < dict_size; i ++) {
		dict[i].empty = 1;
	}
}

template <class DataType>
void HashDict<DataType>::Initial(ifstream &ifs)
{
	assert(dict == NULL);

	ifs.read((char *)&dict_size, sizeof(dict_size));
	assert(ifs && dict_size > 0);

	ifs.read((char *)&free_buckets, sizeof(free_buckets));
	assert(ifs && free_buckets >= 0);

	dict = new DICT_ITEM[dict_size];
	assert(dict != NULL);
        unsigned long all_size = sizeof(DICT_ITEM) * dict_size;
        unsigned long off_set = 0;
        unsigned long len = 9999999;
        unsigned long size = 0;
        while (off_set < all_size) {
            unsigned long tmp = all_size - off_set;
            size = tmp > len ? len : tmp;
            ifs.read((char *)dict + off_set, size);
            if (!ifs) {
	        log(LOG_NOTICE, "###b\n");
                break;
            }
            off_set += size;
        }
//	ifs.read((char *)dict, sizeof(DICT_ITEM) * dict_size);
	assert(ifs);
}

template <class DataType>
bool HashDict<DataType>::Dump(ofstream &ofs)
{
	assert(dict != NULL);

	ofs.write((char *)&dict_size, sizeof(dict_size));
	if (!ofs) {
		return false;
	}

	ofs.write((char *)&free_buckets, sizeof(free_buckets));
	if (!ofs) {
		return false;
	}

        unsigned long all_size = sizeof(DICT_ITEM) * dict_size;
        unsigned long off_set = 0;
        unsigned long len = 9999999;
        unsigned long size = 0;
        while (off_set < all_size) {
            unsigned long tmp = all_size - off_set;
            size = tmp > len ? len : tmp;
            ofs.write((char *)dict + off_set, size);
            if (!ofs) {
	        log(LOG_NOTICE, "###a\n");
                return false;
            }
            off_set += size;
        }
   /*
        ofs.write((char *)dict, sizeof(DICT_ITEM)*dict_size);
	if (!ofs) {
		return false;
	}
   */

	log(LOG_NOTICE, "Hash dict dump over.\n");
	log(LOG_NOTICE, "Hash dict size: %ld\n", dict_size);
	log(LOG_NOTICE, "Number of free buckets: %ld\n", free_buckets);
	return true;
}

template <class DataType>
void HashDict<DataType>::ResizeHashDict()
{
	assert(dict != NULL);

	uint64 new_size = 0;
	for (size_t i = 0; i < sizeof(PRIMES)/sizeof(uint64); i ++) {
		if (PRIMES[i] > dict_size) {
			new_size = PRIMES[i];
			break;
		}
	}
	assert(new_size > 0);

	free_buckets += (new_size - dict_size);
	DICT_ITEM *new_dict = new DICT_ITEM[new_size];
	assert(new_dict != NULL);

	for (uint64 i = 0; i < new_size; i ++) {
		new_dict[i].empty = 1;
	} // for

	for (uint64 i = 0; i < dict_size; i ++) {
		if (dict[i].empty) {
			continue;
		}
		uint64 bucket = dict[i].key % new_size;
		while (true) {
			if (new_dict[bucket].empty) {
				break;
			}
			bucket = (bucket + 1) % new_size;
		} // while

		memcpy(&new_dict[bucket], &dict[i], sizeof(DICT_ITEM));
	} // for

	//log(LOG_NOTICE, "Hash dict resize from %ld to %ld.\n", dict_size, new_size);

	delete []dict;
	dict = new_dict;
	dict_size = new_size;
}


template <class DataType>
void HashDict<DataType>::ResizeToHashDict(HashDict* hashDict, int dict_num){
	for (uint64 i = 0; i < dict_size; i ++) {
		if (dict[i].empty) {
			continue;
		}
		int bucket = dict[i].key % dict_num;
		hashDict[bucket].AddItem(dict[i].key,dict[i].value);
	}
}

template <class DataType>
void HashDict<DataType>::AddHashDict(HashDict* hashDict){
	if(NULL == hashDict){
		return ;
	}
	log(LOG_NOTICE, "start add Hash dict size : %ld, item_num %ld.\n", dict_size, (dict_size-free_buckets));
	for (uint64 i = 0; i < hashDict->dict_size ; i ++) {
		if (hashDict->dict[i].empty) {
			continue;
		}
		uint64 bucket = hashDict->dict[i].key % dict_size;
		while (true) {
			if (dict[bucket].empty || (dict[bucket].key == hashDict->dict[i].key)) {
				break;
			}
			bucket = (bucket + 1) % dict_size;
		} // while
		if (dict[bucket].empty) {
			dict[bucket].key = hashDict->dict[i].key;
			dict[bucket].value = hashDict->dict[i].value;
			dict[bucket].empty = 0;
			if (-- free_buckets < dict_size * 0.25) {
				ResizeHashDict();
			}
		}else{
			dict[bucket].key = hashDict->dict[i].key;
			dict[bucket].value += hashDict->dict[i].value;
			dict[bucket].empty = 0;
		}
	}
	log(LOG_NOTICE, "end add Hash dict size : %ld, item_num %ld.\n", dict_size, (dict_size-free_buckets));
}

template <class DataType>
void HashDict<DataType>::UpdateHashDict(HashDict* hashDict){
	if(NULL == hashDict){
		return ;
	}
	for (uint64 i = 0; i < hashDict->dict_size ; i ++) {
		if (hashDict->dict[i].empty) {
			continue;
		}
		uint64 bucket = hashDict->dict[i].key % dict_size;
		while (true) {
			if (dict[bucket].empty || (dict[bucket].key == hashDict->dict[i].key)) {
				break;
			}
			bucket = (bucket + 1) % dict_size;
		} // while
		dict[bucket].key = hashDict->dict[i].key;
		dict[bucket].value = hashDict->dict[i].value;
		if (dict[bucket].empty) {
			dict[bucket].empty = 0;
			if (-- free_buckets < dict_size * 0.25) {
				ResizeHashDict();
			}
		}
	}
	log(LOG_NOTICE, "Hash dict size : %ld, item_num %ld.\n", dict_size, (dict_size-free_buckets));
}

template <class DataType>
void HashDict<DataType>::DelHashDict(HashDict *delDict){
	if(NULL == delDict){
		return ;
	}
	assert(dict != NULL);

	uint64 new_size = dict_size;
	assert(new_size != 0);

	DICT_ITEM *new_dict = new DICT_ITEM[new_size];
	assert(new_dict != NULL);

	for (uint64 i = 0; i < new_size; i ++) {
		new_dict[i].empty = 1;
	} // for

	log(LOG_NOTICE, "start del Hash dict size %ld , item size  %ld.\n", dict_size, ( dict_size - free_buckets));
	free_buckets = new_size;
	for (uint64 i = 0; i < dict_size; i ++) {
		DataType value;
		if (dict[i].empty) {
			continue;
		}
		uint64 bucket = dict[i].key % new_size;
		while (true) {
			if (new_dict[bucket].empty) {
				break;
			}																			                                     bucket = (bucket + 1) % new_size;
		}
		if(delDict->GetItem(dict[i].key,value)){
			if( dict[i].value > value){
				free_buckets--;
				new_dict[bucket].value = dict[i].value -value;
				new_dict[bucket].key = dict[i].key;
				new_dict[bucket].empty = 0;
			}
		}else{
			free_buckets--;
			new_dict[bucket].value = dict[i].value;
			new_dict[bucket].key = dict[i].key;
			new_dict[bucket].empty = 0;
			//memcpy(&new_dict[bucket], &dict[i], sizeof(DICT_ITEM));
		}
	} // for

	delete []dict;
	dict = new_dict;
	dict_size = new_size;
	log(LOG_NOTICE, "end del Hash dict size %ld , item size  %ld.\n", dict_size, ( dict_size - free_buckets));
}

template <class DataType>
bool HashDict<DataType>::AddItem(const char *key, DataType value)
{
	return AddItem(MurmurHash64A(key, strlen(key)), value);
}

template <class DataType>
bool HashDict<DataType>::AddItem(const string &key, DataType value)
{
	return AddItem(key.c_str(), value);
}

template <class DataType>
bool HashDict<DataType>::AddItem(const uint64 key, DataType value)
{
	assert(dict != NULL);

	uint64 bucket = key % dict_size;
	while (true) {
		if (dict[bucket].empty || (dict[bucket].key == key)) {
			break;
		}
		bucket = (bucket + 1) % dict_size;
	} // while
	dict[bucket].key = key;
	dict[bucket].value = value;

	if (dict[bucket].empty) {
		//dict[bucket].key = key;
		//dict[bucket].value = value;
		dict[bucket].empty = 0;

		if (-- free_buckets < dict_size * 0.25) {
			ResizeHashDict();
		}

		return true;
	} // if

	return false;
}

template <class DataType>
bool HashDict<DataType>::GetItem(const char *key, DataType &value)
{
	return GetItem(MurmurHash64A(key, strlen(key)), value);
}

template <class DataType>
bool HashDict<DataType>::GetItem(const string &key, DataType &value)
{
	return GetItem(key.c_str(), value);
}

template <class DataType>
bool HashDict<DataType>::GetItem(const uint64 key, DataType &value)
{
	assert(dict != NULL);

	uint64 bucket = key % dict_size;
	while (true) {
		if (dict[bucket].empty) {
			return false;
		}
		if (dict[bucket].key == key) {
			value = dict[bucket].value;
			return true;
		}
		bucket = (bucket + 1) % dict_size;
	} // while

	return false;
}

template <class DataType>
void HashDict<DataType>::ModifyItem(const char* key, DataType value)
{
	ModifyItem(MurmurHash64A(key, strlen(key)), value);
}

template <class DataType>
void HashDict<DataType>::ModifyItem(const string &key, DataType value)
{
	ModifyItem(key.c_str(), value);
}

template <class DataType>
void HashDict<DataType>::ModifyItem(const uint64 key, DataType value)
{
	assert(dict != NULL);

	uint64 bucket = key % dict_size;
	while (true) {
		if (dict[bucket].empty || (dict[bucket].key == key)) {
			break;
		}
		bucket = (bucket + 1) % dict_size;
	} // while

	assert(!dict[bucket].empty);
	dict[bucket].value = value;
}

#endif
