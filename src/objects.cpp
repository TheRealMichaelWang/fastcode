#include "hash.h"
#include "structure.h"
#include "collection.h"
#include "references.h"
#include "garbage.h"

namespace fastcode {
	namespace parsing {
		structure_prototype::structure_prototype(const char* identifier, const char* properties[], unsigned int property_count) : token(TOKEN_STRUCT_PROTO) {
			this->identifier = new identifier_token(identifier);
			this->property_count = property_count;
			for (unsigned int i = 0; i < property_count; i++) {
				identifier_token* prop = new identifier_token(identifier);
				this->property_indicies[prop->id_hash] = i;
				this->properties.push_back(prop);
			}
		}

		structure_prototype::structure_prototype(identifier_token* identifier, std::list<identifier_token*> properties) : token(TOKEN_STRUCT_PROTO) {
			this->identifier = identifier;
			this->property_count = properties.size();
			unsigned int index = 0;
			for (auto i = properties.begin(); i != properties.end(); ++i)
				this->property_indicies[(*i)->id_hash] = index++;
			this->properties = properties;
		}

		structure_prototype::~structure_prototype() {
			delete identifier;
		}
	}

	namespace runtime {
		structure::structure(parsing::structure_prototype* prototype, garbage_collector* gc) : structure(prototype, gc->new_apartment(new value(VALUE_TYPE_STRUCT, this))) {
			for (size_t i = 0; i < prototype->property_count; i++)
				this->properties[i] = gc->new_apartment(new value(VALUE_TYPE_NULL, nullptr));
		}

		structure::structure(parsing::structure_prototype* prototype, reference_apartment* parent_reference) {
			this->parent_reference = parent_reference;
			this->prototype = prototype;
			this->properties = new reference_apartment * [prototype->property_count];
		}

		structure::~structure() {
			delete[] this->properties;
		}

		void structure::set_reference(unsigned long id_hash, reference_apartment* reference) {
			unsigned int index = prototype->get_index(id_hash);
			this->properties[index]->remove_reference(parent_reference);
			reference->add_reference(parent_reference);
			this->properties[index] = reference;
		}

		structure* structure::clone(reference_apartment* parent_reference) {
			structure* kopy = new structure(this->prototype, parent_reference);
			for (size_t i = 0; i < this->prototype->property_count; i++)
			{
				kopy->properties[i] = this->properties[i];
				this->properties[i]->add_reference(parent_reference);
			}
			return kopy;
		}

		int structure::hash() {
			int hash = 2187; //magic num for structs
			for (size_t i = 0; i < this->prototype->property_count; i++)
			{
				hash = combine_hash(hash, this->properties[i]->value->hash());
			}
			return hash;
		}

		collection::collection(unsigned long size, garbage_collector* gc) : collection(size, gc->new_apartment(new value(VALUE_TYPE_COLLECTION, this))) {
			for (unsigned long i = 0; i < size; i++)
			{
				this->inner_collection[i] = gc->new_apartment(new value(VALUE_TYPE_NULL, nullptr));
			}
		}

		collection::collection(collection* a, collection* b, reference_apartment* parent_reference) : collection(a->size + b->size, parent_reference) {
			for (size_t i = 0; i < a->size; i++)
			{
				this->inner_collection[i] = a->inner_collection[i];
				this->inner_collection[i]->add_reference(parent_reference);
			}
			for (size_t i = 0; i < b->size; i++)
			{
				this->inner_collection[a->size + i] = b->inner_collection[i];
				b->inner_collection[i]->add_reference(parent_reference);
			}
		}

		collection::collection(unsigned long size, reference_apartment* parent_reference) {
			this->size = size;
			this->parent_reference = parent_reference;
			this->inner_collection = new reference_apartment * [size];
		}

		collection::~collection() {
			delete this->inner_collection;
		}

		collection* collection::clone(reference_apartment* new_parent_apptr) {
			collection* copy = new collection(this->size, new_parent_apptr);
			for (unsigned long i = 0; i < size; i++)
				copy->inner_collection[i] = this->inner_collection[i];
			return copy;
		}

		int collection::hash() {
			int hash = 66; //magic number for collection hahses
			for (size_t i = 0; i < this->size; i++)
				hash = combine_hash(hash, this->inner_collection[i]->value->hash());
			return hash;
		}

		//returns the inner refrence_apartment array. DO NOT DELETE 
		reference_apartment** reference_apartment::get_children(unsigned int* children_size) {
			if (value->type == VALUE_TYPE_COLLECTION) {
				collection* collection = (class collection*)value->ptr;
				*children_size = collection->size;
				return collection->get_children();
			}
			else if (value->type == VALUE_TYPE_STRUCT) {
				structure* structure = (class structure*)value->ptr;
				*children_size = structure->get_size();
				return structure->get_children();
			}
			return nullptr;
		}
	}

	value::~value() {
		switch (this->type)
		{
		case VALUE_TYPE_NULL:
			break;
		case VALUE_TYPE_CHAR:
			delete (char*)this->ptr;
			break;
		case VALUE_TYPE_NUMERICAL:
			delete (long double*)this->ptr;
			break;
		case VALUE_TYPE_COLLECTION:
			delete (runtime::collection*)this->ptr;
			break;
		case VALUE_TYPE_STRUCT:
			delete (runtime::structure*)this->ptr;
			break;
		case VALUE_TYPE_HANDLE:
			break;
		default:
			throw ERROR_INVALID_VALUE_TYPE;
		}
	}

	int value::hash() {
		switch (this->type)
		{
		case VALUE_TYPE_CHAR:
			return int(*(char*)this->ptr);
		case VALUE_TYPE_NUMERICAL:
			return int(*(long double*)this->ptr);
		case VALUE_TYPE_COLLECTION:
			return ((runtime::collection*)this->ptr)->hash();
		case VALUE_TYPE_STRUCT:
			return ((runtime::structure*)this->ptr)->hash();
		case VALUE_TYPE_HANDLE:
			return int(this->ptr);
		default:
			throw ERROR_INVALID_VALUE_TYPE;
		}
	}
}