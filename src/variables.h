#pragma once

#ifndef VARIABLE_H
#define VARIABLE_H

#include "tokens.h"
#include "errors.h"
#include "value.h"
#include "references.h"
#include "garbage.h"

class variable_bucket {
public:
	unsigned long id_hash;
	reference_apartment* apartment;
	variable_bucket(unsigned long id_hash, reference_apartment* apartment, variable_bucket* next_bucket = nullptr);
	variable_bucket* next_bucket;
};

#define VARIABLE_HASH_BUCKET_SIZE 100

class variable_manager {
public:
	variable_manager(garbage_collector* garbage_collector);
	~variable_manager();

	//declares a variable with a value
	inline reference_apartment* declare_var(unsigned long id_hash, value* value) {
		return declare_var(id_hash, garbage_collector->new_apartment(value));
	}

	//declares a variable with a value
	inline reference_apartment* declare_var(identifier_token* identifier, value* value) {
		return declare_var(identifier->id_hash, value);
	}

	//declares a variable with a reference
	reference_apartment* declare_var(unsigned long id_hash, reference_apartment* reference);

	//declares a variable with a reference
	inline reference_apartment* declare_var(identifier_token* identifier, reference_apartment* reference) {
		return declare_var(identifier->id_hash, reference);
	}
	
	//removes a variable
	void remove_var(unsigned long id_hash);

	//removes a variable
	inline void remove_var(identifier_token* identifier) {
		remove_var(identifier->id_hash);
	}

	//checks whether a variable exists
	bool has_var(unsigned long id_hash);

	//checks whether a variable exists
	inline bool has_var(identifier_token* identifier) {
		return has_var(identifier->id_hash);
	}

	//sets a variable to a reference
	void set_var_reference(unsigned long id_hash, reference_apartment* reference);

	//sets a variable to a reference
	inline void set_var_reference(identifier_token* identifier, reference_apartment* reference) {
		set_var_reference(identifier->id_hash, reference);
	}

	//sets a variable to a value
	inline void set_var_value(unsigned long id_hash, value* value) {
		get_var_reference(id_hash)->set_value(value);
	}

	//sets a variable to a value
	inline void set_var_value(identifier_token* identifier, value* value) {
		get_var_reference(identifier->id_hash)->set_value(value);
	}

	//gets a variable's reference
	reference_apartment* get_var_reference(unsigned long id_hash);

	//gets a variable's reference
	inline reference_apartment* get_var_reference(identifier_token* identifier) {
		return get_var_reference(identifier->id_hash);
	}

	//gets a variable's value
	inline value* get_var_value(unsigned long id_hash) {
		return get_var_reference(id_hash)->value;
	}

	//gets a variable's value
	inline value* get_var_value(identifier_token* identifier) {
		return get_var_reference(identifier->id_hash)->value;
	}
private:
	unsigned int size;
	garbage_collector* garbage_collector;
	variable_bucket* hash_buckets[VARIABLE_HASH_BUCKET_SIZE];
};

#endif // !VARIABLE_H