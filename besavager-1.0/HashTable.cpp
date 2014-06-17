// BeSavager - Savager Workroom Development Toolkit for BeOS
// (C) Copyright by Anthony Lee <anthonylee@eLong.com> 2002
//
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
//
//
// Hash Table yanked from GLIB


#include "HashTable.h"
#include "Strings.h"

#define HASH_TABLE_MIN_SIZE 11
#define HASH_TABLE_MAX_SIZE 13845163

SHashNode::SHashNode()
	: key(NULL), value(NULL), next(NULL)
{
}


SHashNode::SHashNode(void *key, void *value)
{
	SHashNode::key = key;
	SHashNode::value = value;
	next = NULL;
}


SHashNode::~SHashNode()
{
}


SHashTable::SHashTable()
{
	size = HASH_TABLE_MIN_SIZE;
	nnodes = 0;
	nodes = new SHashNode*[size];

	for(int32 i = 0; i < size; i++) nodes[i] = NULL;
}


void
SHashTable::destroy_node(SHashNode *hash_node, bool destroy)
{
	if(!hash_node) return;

	if(destroy)
	{
		if(hash_node->key) KeyDestroy(hash_node->key);
		if(hash_node->value) ValueDestroy(hash_node->value);
	}

	hash_node->key = NULL;
	hash_node->value = NULL;
}

void
SHashTable::destroy_nodes(SHashNode *has_node)
{
	if(has_node == NULL) return;

	SHashNode *node = has_node;

	while(node->next)
	{
		if(node->key) KeyDestroy(node->key);
		if(node->value) ValueDestroy(node->value);

		node->key = NULL;
		node->value = NULL;

		node = node->next;
	}

	if(node->key) KeyDestroy(node->key);
	if(node->value) ValueDestroy(node->value);

	node->key = NULL;
	node->value = NULL;
}


void
SHashTable::MakeEmpty(bool remove_style)
{
	if(remove_style) for(int32 i = 0; i < size; i++) destroy_nodes(nodes[i]);
	delete[] nodes;

	size = HASH_TABLE_MIN_SIZE;
	nnodes = 0;
	nodes = new SHashNode*[size];
	
	for(int32 i = 0; i < size; i++) nodes[i] = NULL;
}


SHashTable::~SHashTable()
{
	for(int32 i = 0; i < size; i++) destroy_nodes(nodes[i]);
	delete[] nodes;
}


uint32
SHashTable::Hash(const void* key)
{
	return((uint32)key);
}


bool
SHashTable::KeyEqual(const void* a, const void* b)
{
	return a == b;
}

void
SHashTable::KeyDestroy(void *key)
{
}


void
SHashTable::ValueDestroy(void *value)
{
}


SHashNode**
SHashTable::lookup_node(const void* key)
{
	SHashNode **node;

	node = &nodes[Hash(key) % size];

	while(*node && KeyEqual((*node)->key, key) == false) node = &(*node)->next;

	return node;
}


void*
SHashTable::Lookup(const void* key)
{
	if(key == NULL) return NULL;

	SHashNode* node = *lookup_node(key);
	return node ? node->value : NULL;
}


bool
SHashTable::Lookup(const void* lookup_key, void** orig_key, void** value)
{
	SHashNode* node;
  
	node = *lookup_node(lookup_key);
  
	if(node)
	{
		if(orig_key) *orig_key = node->key;
		if(value) *value = node->value;
		return true;
	}
	else
		return false;
}


static const uint32 hash_primes[] =
{
  11,
  19,
  37,
  73,
  109,
  163,
  251,
  367,
  557,
  823,
  1237,
  1861,
  2777,
  4177,
  6247,
  9371,
  14057,
  21089,
  31627,
  47431,
  71143,
  106721,
  160073,
  240101,
  360163,
  540217,
  810343,
  1215497,
  1823231,
  2734867,
  4102283,
  6153409,
  9230113,
  13845163,
};

static const uint32 hash_nprimes = sizeof(hash_primes) / sizeof(hash_primes[0]);

inline uint32 hash_spaced_primes_closest(uint32 num)
{
	uint32 i;

	for(i = 0; i < hash_nprimes; i++)
		if(hash_primes[i] > num) return hash_primes[i];

	return hash_primes[hash_nprimes - 1];
}


#undef  CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))


void
SHashTable::sizechanged()
{
	SHashNode **new_nodes;
	SHashNode *node;
	SHashNode *next;

	uint32 hash_val;
	int32 new_size;
	int32 i;

	new_size = hash_spaced_primes_closest(nnodes);
	new_size = CLAMP(new_size, HASH_TABLE_MIN_SIZE, HASH_TABLE_MAX_SIZE);
 
	new_nodes = new SHashNode*[new_size];
	for(i = 0; i < new_size; i++) new_nodes[i] = NULL;
  
	for(i = 0; i < size; i++)
		for(node = nodes[i]; node; node = next)
		{
			next = node->next;

			hash_val = Hash(node->key) % new_size;

			node->next = new_nodes[hash_val];
			new_nodes[hash_val] = node;
		}
  
	delete[] nodes;
	nodes = new_nodes;
	size = new_size;
}


void
SHashTable::Insert(void* key, void* value)
{
	SHashNode **node;
  
	node = lookup_node(key);
  
	if(*node)
    {
		KeyDestroy(key);
		ValueDestroy((*node)->value);
		(*node)->value = value;
	}
	else
	{
		*node = new SHashNode(key, value);
		nnodes++;

		if((size >= 3 * nnodes && size > HASH_TABLE_MIN_SIZE) ||
		   (3 * size <= nnodes && size < HASH_TABLE_MAX_SIZE))
			sizechanged();
	}
}


void
SHashTable::Replace(void *key, void* value)
{
	SHashNode **node;
  
	node = lookup_node(key);
  
	if(*node)
	{
		KeyDestroy((*node)->key);
		ValueDestroy((*node)->value);
		(*node)->key = key;
		(*node)->value = value;
	}
	else
	{
		*node = new SHashNode(key, value);
		nnodes++;

		if((size >= 3 * nnodes && size > HASH_TABLE_MIN_SIZE) ||
		   (3 * size <= nnodes && size < HASH_TABLE_MAX_SIZE))
			sizechanged();
	}
}


bool
SHashTable::Remove(const void* key)
{
	SHashNode **node, *dest;
  
	node = lookup_node(key);

	if(*node)
	{
		dest = *node;
		(*node) = dest->next;
		destroy_node(dest, true);
		nnodes--;

		if((size >= 3 * nnodes && size > HASH_TABLE_MIN_SIZE) ||
		   (3 * size <= nnodes && size < HASH_TABLE_MAX_SIZE))
			sizechanged();

		return true;
	}

	return false;
}


bool
SHashTable::Steal(const void* key)
{
	SHashNode **node, *dest;
  
	node = lookup_node(key);

	if(*node)
	{
		dest = *node;
		(*node) = dest->next;
		destroy_node(dest, false);
		nnodes--;

		if((size >= 3 * nnodes && size > HASH_TABLE_MIN_SIZE) ||
		   (3 * size <= nnodes && size < HASH_TABLE_MAX_SIZE))
			sizechanged();

		return true;
	}

	return false;
}


void
SHashTable::DoForeach(SHashTableForeachFunction func, void *user_data)
{
	SHashNode *node;
	int32 i;
  
	if(func == NULL) return;
  
	for(i = 0; i < size; i++)
		for(node = nodes[i]; node; node = node->next)
			(*func)(node->key, node->value, user_data);
}


uint32
SHashTable::CountNodes() const
{
	return nnodes;
}


int32
SHashTable::foreach_remove_or_steal(SHashTableForeachFunction func,
									void *user_data,
									bool remove_style)
{
	SHashNode *node, *prev;
	int32 i;
	int32 deleted = 0;

	for(i = 0; i < size; i++)
	{
restart:

		prev = NULL;
      
		for(node = nodes[i]; node; prev = node, node = node->next)
		{
			if((*func)(node->key, node->value, user_data))
			{
				deleted += 1;
	      
				nnodes -= 1;
	      
				if(prev)
				{
					prev->next = node->next;
					destroy_node(node, remove_style);
					node = prev;
				}
				else
				{
					nodes[i] = node->next;
					destroy_node(node, remove_style);
					goto restart;
				}
			}
		}
	}
  
	if((size >= 3 * nnodes && size > HASH_TABLE_MIN_SIZE) ||
	   (3 * size <= nnodes && size < HASH_TABLE_MAX_SIZE))
		sizechanged();
	
	return deleted;
}


int32
SHashTable::DoForeachRemove(SHashTableForeachFunction func, void *user_data)
{
	if(func == NULL) return 0;
	
	return foreach_remove_or_steal(func, user_data, true);
}


int32
SHashTable::DoForeachSteal(SHashTableForeachFunction func, void *user_data)
{
	if(func == NULL) return 0;
	
	return foreach_remove_or_steal(func, user_data, false);
}


SStringHash::SStringHash()
	: SHashTable()
{
}


SStringHash::~SStringHash()
{
	MakeEmpty();
}


uint32
SStringHash::Hash(const void* key)
{
	const char *p = (char*)key;
	uint32 h = *p;

	while(*p)
		h = (h << 7) ^ (h >> 24) ^ *p++;

	return h;
}


bool
SStringHash::KeyEqual(const void* a, const void* b)
{
	if(!a && !b) return true;
	if(!a || !b) return false;

	const char *string1 = (char*)a;
	const char *string2 = (char*)b;

	return(strcasecmp(string1, string2) == 0 && strlen(string1) == strlen(string2));
}


void
SStringHash::KeyDestroy(void* key)
{
	char *string = (char*)key;
	delete[] string;
}


void
SStringHash::ValueDestroy(void* value)
{
	char *string = (char*)value;
	delete[] string;
}


void
SStringHash::InsertString(const char* key, const char* value)
{
	if(key == NULL || value == NULL) return;
	char *key_string = Sstrdup(key);
	char *value_string = Sstrdup(value);

	Insert((void*)key_string, (void*)value_string);
}


void
SStringHash::ReplaceString(const char* key, const char* value)
{
	if(key == NULL || value == NULL) return;
	char *key_string = Sstrdup(key);
	char *value_string = Sstrdup(value);

	Replace((void*)key_string, (void*)value_string);
}


SStringInt32Hash::SStringInt32Hash()
	: SHashTable()
{
}


SStringInt32Hash::~SStringInt32Hash()
{
	MakeEmpty();
}


uint32
SStringInt32Hash::Hash(const void* key)
{
	const char *p = (char*)key;
	uint32 h = *p;

	while(*p)
		h = (h << 7) ^ (h >> 24) ^ *p++;

	return h;
}


bool
SStringInt32Hash::KeyEqual(const void* a, const void* b)
{
	if(!a && !b) return true;
	if(!a || !b) return false;

	const char *string1 = (char*)a;
	const char *string2 = (char*)b;

	return(strcasecmp(string1, string2) == 0 && strlen(string1) == strlen(string2));
}


void
SStringInt32Hash::KeyDestroy(void* key)
{
	char *string = (char*)key;
	delete[] string;
}


void
SStringInt32Hash::ValueDestroy(void* value)
{
	int32 *val = (int32*)value;
	delete val;
}


void
SStringInt32Hash::InsertInt32(const char* key, int32 value)
{
	if(key == NULL) return;
	char *key_string = Sstrdup(key);
	int32 *val = new int32;
	*val = value;

	Insert((void*)key_string, (void*)val);
}


void
SStringInt32Hash::ReplaceInt32(const char* key, int32 value)
{
	if(key == NULL) return;
	char *key_string = Sstrdup(key);
	int32 *val = new int32;
	*val = value;

	Replace((void*)key_string, (void*)val);
}


SStringInt64Hash::SStringInt64Hash()
	: SHashTable()
{
}


SStringInt64Hash::~SStringInt64Hash()
{
	MakeEmpty();
}


uint32
SStringInt64Hash::Hash(const void* key)
{
	const char *p = (char*)key;
	uint32 h = *p;

	while(*p)
		h = (h << 7) ^ (h >> 24) ^ *p++;

	return h;
}


bool
SStringInt64Hash::KeyEqual(const void* a, const void* b)
{
	if(!a && !b) return true;
	if(!a || !b) return false;

	const char *string1 = (char*)a;
	const char *string2 = (char*)b;

	return(strcasecmp(string1, string2) == 0 && strlen(string1) == strlen(string2));
}


void
SStringInt64Hash::KeyDestroy(void* key)
{
	char *string = (char*)key;
	delete[] string;
}


void
SStringInt64Hash::ValueDestroy(void* value)
{
	int64 *val = (int64*)value;
	delete val;
}


void
SStringInt64Hash::InsertInt64(const char* key, int64 value)
{
	if(key == NULL) return;
	char *key_string = Sstrdup(key);
	int64 *val = new int64;
	*val = value;

	Insert((void*)key_string, (void*)val);
}


void
SStringInt64Hash::ReplaceInt64(const char* key, int64 value)
{
	if(key == NULL) return;
	char *key_string = Sstrdup(key);
	int64 *val = new int64;
	*val = value;

	Replace((void*)key_string, (void*)val);
}


SStringColorHash::SStringColorHash()
	: SHashTable()
{
}


SStringColorHash::~SStringColorHash()
{
	MakeEmpty();
}


uint32
SStringColorHash::Hash(const void* key)
{
	const char *p = (char*)key;
	uint32 h = *p;

	while(*p)
		h = (h << 7) ^ (h >> 24) ^ *p++;

	return h;
}


bool
SStringColorHash::KeyEqual(const void* a, const void* b)
{
	if(!a && !b) return true;
	if(!a || !b) return false;

	const char *string1 = (char*)a;
	const char *string2 = (char*)b;

	return(strcasecmp(string1, string2) == 0 && strlen(string1) == strlen(string2));
}


void
SStringColorHash::KeyDestroy(void* key)
{
	char *string = (char*)key;
	delete[] string;
}


void
SStringColorHash::ValueDestroy(void* value)
{
	s_rgb_color_and_index *color = (s_rgb_color_and_index*)value;
	delete color;
}


void
SStringColorHash::InsertColor(const char* key, rgb_color color, int32 index)
{
	if(key == NULL) return;
	char *key_string = Sstrdup(key);

	s_rgb_color_and_index *cl = new s_rgb_color_and_index;
	cl->color.red = color.red;
	cl->color.green = color.green;
	cl->color.blue = color.blue;
	cl->color.alpha = color.alpha;
	cl->index = index;

	Insert((void*)key_string, (void*)cl);
}


void
SStringColorHash::ReplaceColor(const char* key, rgb_color color, int32 index)
{
	if(key == NULL) return;
	char *key_string = Sstrdup(key);
	
	s_rgb_color_and_index *cl = new s_rgb_color_and_index;
	cl->color.red = color.red;
	cl->color.green = color.green;
	cl->color.blue = color.blue;
	cl->color.alpha = color.alpha;
	cl->index = index;

	Replace((void*)key_string, (void*)cl);
}
