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

#ifndef __SAVAGER_HASH_TABLE_H__
#define __SAVAGER_HASH_TABLE_H__

#include <string.h>

#include <SupportDefs.h>
#include <GraphicsDefs.h>
#include <String.h>

class SHashTable;

class SHashNode {
public:
	SHashNode();
	SHashNode(void *key, void *value);
	~SHashNode();

private:
	friend class SHashTable;

	void* key;
	void* value;
	SHashNode *next;
};

typedef bool (*SHashTableForeachFunction) (void *key, void *value, void *user_data);

class SHashTable {
public:
	SHashTable();
	virtual ~SHashTable();

	void*	Lookup(const void* key);
	bool	Lookup(const void* lookup_key, void** orig_key, void** value);

	// Insert(),Replace():
	//		the memory for key and value must be malloced by yourself, just link to the node!
	//		the key and value can be NULL, so use it carefully by yourself
	//		or probably you can't find anything by pass NULL to the Lookup()
	void	Insert(void* key, void* value);
	void	Replace(void* key, void* value);

	bool	Remove(const void* key); // destroy the key and value too
	bool	Steal(const void* key); // same as Remove(), but don't destroy the key and value

	// MakeEmpty(): delete all nodes of the hash
	//	remove_style - true: like Remove();		false: like Steal();
	void	MakeEmpty(bool remove_style = true);

	uint32	CountNodes() const;

	void	DoForeach(SHashTableForeachFunction func, void *user_data = NULL);

	// if the func return true, the data in the current node will be free automatic
	// Return Value: the number of key/value pairs removed.
	int32	DoForeachRemove(SHashTableForeachFunction func, void *user_data = NULL);
	int32	DoForeachSteal(SHashTableForeachFunction func, void *user_data = NULL);


	//
	// virtual functions
	//

	// Hash(): generate the hash value from the key
	//		   here just the direct hash function
	virtual uint32 Hash(const void* key);

	// KeyEqual(): whether the key "a" equal to key "b"
	//			   here just the direct equal function
	virtual bool KeyEqual(const void* a, const void* b);


	// KeyDestroy(): destroy key data callback function
	//				 here just a cleanup function
	virtual void KeyDestroy(void* key);

	
	// ValueDestroy(): destroy key data callback function
	//				   here just a cleanup function
	virtual void ValueDestroy(void* value);


private:
	int32 size;
	int32 nnodes;
	SHashNode **nodes;

	void destroy_node(SHashNode *hash_node, bool destroy);
	void destroy_nodes(SHashNode *has_node);

	SHashNode** lookup_node(const void* key);

	void sizechanged();

	int32 foreach_remove_or_steal(SHashTableForeachFunction func, void *user_data, bool remove_style);
};


// String->String Hash Table
//
//		SStringHash::~SStringHash()
//		{
//			MakeEmpty(); /* make sure to empty the nodes by your custom destroy function */
//		}
//
//
class SStringHash : public SHashTable {
public:
	SStringHash();
	virtual ~SStringHash();

	virtual uint32 Hash(const void* key);
	virtual bool KeyEqual(const void* a, const void* b);
	virtual void KeyDestroy(void* key);
	virtual void ValueDestroy(void* value);

	void	InsertString(const char* key, const char* value);
	void	InsertString(const BString key, const BString value);
	void	InsertString(const char* key, const BString value);
	void	InsertString(const BString key, const char* value);

	void	ReplaceString(const char* key, const char* value);
	void	ReplaceString(const BString key, const BString value);
	void	ReplaceString(const char* key, const BString value);
	void	ReplaceString(const BString key, const char* value);
};


inline void
SStringHash::InsertString(const BString key, const BString value)
{
	InsertString(key.String(), value.String());
}


inline void
SStringHash::InsertString(const char* key, const BString value)
{
	InsertString(key, value.String());
}


inline void
SStringHash::InsertString(const BString key, const char* value)
{
	InsertString(key.String(), value);
}


inline void
SStringHash::ReplaceString(const BString key, const BString value)
{
	ReplaceString(key.String(), value.String());
}


inline void
SStringHash::ReplaceString(const char* key, const BString value)
{
	ReplaceString(key, value.String());
}


inline void
SStringHash::ReplaceString(const BString key, const char* value)
{
	ReplaceString(key.String(), value);
}


// String->Int32 Hash Table
class SStringInt32Hash : public SHashTable {
public:
	SStringInt32Hash();
	virtual ~SStringInt32Hash();

	virtual uint32 Hash(const void* key);
	virtual bool KeyEqual(const void* a, const void* b);
	virtual void KeyDestroy(void* key);
	virtual void ValueDestroy(void* value);

	void	InsertInt32(const char* key, int32 value);
	void	InsertInt32(const BString key, int32 value);

	void	ReplaceInt32(const char* key, int32 value);
	void	ReplaceInt32(const BString key, int32 value);
};


inline void
SStringInt32Hash::InsertInt32(const BString key, int32 value)
{
	InsertInt32(key.String(), value);
}


inline void
SStringInt32Hash::ReplaceInt32(const BString key, int32 value)
{
	ReplaceInt32(key.String(), value);
}


// String->Int64 Hash Table
class SStringInt64Hash : public SHashTable {
public:
	SStringInt64Hash();
	virtual ~SStringInt64Hash();

	virtual uint32 Hash(const void* key);
	virtual bool KeyEqual(const void* a, const void* b);
	virtual void KeyDestroy(void* key);
	virtual void ValueDestroy(void* value);

	void	InsertInt64(const char* key, int64 value);
	void	InsertInt64(const BString key, int64 value);

	void	ReplaceInt64(const char* key, int64 value);
	void	ReplaceInt64(const BString key, int64 value);
};


inline void
SStringInt64Hash::InsertInt64(const BString key, int64 value)
{
	InsertInt64(key.String(), value);
}


inline void
SStringInt64Hash::ReplaceInt64(const BString key, int64 value)
{
	ReplaceInt64(key.String(), value);
}


// String->Color Hash Table
class SStringColorHash : public SHashTable {
public:
	SStringColorHash();
	virtual ~SStringColorHash();

	virtual uint32 Hash(const void* key);
	virtual bool KeyEqual(const void* a, const void* b);
	virtual void KeyDestroy(void* key);
	virtual void ValueDestroy(void* value);
	
	typedef struct s_rgb_color_and_index {
		rgb_color color;
		int32 index;
	} s_rgb_color_and_index;

	// the Lookup() will return the "s_rgb_color_and_index*"
	//
	//		SStringColorHash *hash = new SStringColorHash();
	//		...
	//		rgb_color red_color = {255, 0, 0, 255};
	//		hash->InsertColor("red", red_color);
	//		...
	//		rgb_color *color = (rgb_color*)hash->Lookup((void*)"red"); /* return rgb_color struct {255, 0, 0, 255} */
	//	or
	//		SStringColorHash::s_rgb_color_and_index *color = (SStringColorHash::s_rgb_color_and_index*)hash->Lookup((void*)"red");

	void InsertColor(const char* key, rgb_color color, int32 index = -1);
	void InsertColor(const BString key, rgb_color color, int32 index = -1);

	void InsertColor(const char* key, uint8 red, uint8 green, uint8 blue, uint8 alpha = 255, int32 index = -1);
	void InsertColor(const BString key, uint8 red, uint green, uint8 blue, uint8 alpha = 255, int32 index = -1);

	void ReplaceColor(const char* key, rgb_color color, int32 index = -1);
	void ReplaceColor(const BString key, rgb_color color, int32 index = -1);

	void ReplaceColor(const char* key, uint8 red, uint8 green, uint8 blue, uint8 alpha = 255, int32 index = -1);
	void ReplaceColor(const BString key, uint8 red, uint green, uint8 blue, uint8 alpha = 255, int32 index = -1);
};


inline void
SStringColorHash::InsertColor(const BString key, rgb_color color, int32 index)
{
	InsertColor(key.String(), color, index);
}


inline void
SStringColorHash::ReplaceColor(const BString key, rgb_color color, int32 index)
{
	ReplaceColor(key.String(), color, index);
}


inline void
SStringColorHash::InsertColor(const char* key, uint8 red, uint8 green, uint8 blue, uint8 alpha, int32 index)
{
	rgb_color color;
	color.red = red;
	color.green = green;
	color.blue = blue;
	color.alpha = alpha;
	
	InsertColor(key, color, index);
}


inline void
SStringColorHash::InsertColor(const BString key, uint8 red, uint green, uint8 blue, uint8 alpha, int32 index)
{
	rgb_color color;
	color.red = red;
	color.green = green;
	color.blue = blue;
	color.alpha = alpha;
	
	InsertColor(key, color, index);
}


inline void
SStringColorHash::ReplaceColor(const char* key, uint8 red, uint8 green, uint8 blue, uint8 alpha, int32 index)
{
	rgb_color color;
	color.red = red;
	color.green = green;
	color.blue = blue;
	color.alpha = alpha;
	
	ReplaceColor(key, color, index);
}


inline void
SStringColorHash::ReplaceColor(const BString key, uint8 red, uint green, uint8 blue, uint8 alpha, int32 index)
{
	rgb_color color;
	color.red = red;
	color.green = green;
	color.blue = blue;
	color.alpha = alpha;
	
	ReplaceColor(key, color, index);
}



#endif /* __SAVAGER_HASH_TABLE_H__ */


