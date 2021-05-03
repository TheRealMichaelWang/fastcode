#pragma once

#ifndef COLLECTION_H
#define COLLECTION_H

#include "references.h"
#include "garbage.h"
#include "value.h"

class collection {
public:
	unsigned long size;
	collection(unsigned long size, garbage_collector* gc);
	collection(collection* a, collection* b, reference_apartment* parent_reference);
	~collection();
	inline void set_reference(unsigned long index, reference_apartment* reference) {
		this->inner_collection[index]->remove_reference(parent_reference);
		reference->add_reference(parent_reference);
		this->inner_collection[index] = reference;
	}

	inline reference_apartment* get_reference(unsigned long index) {
		return this->inner_collection[index];
	}

	inline void set_value(unsigned long index, value* value) {
		get_reference(index)->set_value(value);
	}

	inline value* get_value(unsigned long index) {
		return get_reference(index)->value;
	}
	
	inline reference_apartment** get_children() {
		return this->inner_collection;
	}

	collection* clone(reference_apartment* parent_reference);

	inline reference_apartment* get_parent() {
		return this->parent_reference;
	}

	int hash();

private:
	reference_apartment* parent_reference;
	reference_apartment** inner_collection;
	collection(unsigned long size, reference_apartment* parent_reference);
};

#endif // !COLLECTION_H