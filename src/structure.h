#pragma once

#ifndef STRUCT_H
#define STRUCT_H

#include <list>
#include <map>
#include "tokens.h"
#include "references.h"
#include "garbage.h"

namespace fastcode {
	namespace parsing {
		class structure_prototype : public token {
		private:
			std::map<unsigned long, unsigned int> property_indicies;
			std::list<identifier_token*> properties;
		public:

			identifier_token* identifier;

			unsigned int property_count;

			structure_prototype(const char* identifier, const char* properties[], unsigned int property_count);
			structure_prototype(identifier_token* identifier, std::list<identifier_token*> properties);
			~structure_prototype();

			//gets the index of a property
			inline unsigned int get_index(unsigned long id_hash) {
				if (!this->property_indicies.count(id_hash))
					throw ERROR_PROPERTY_NOT_FOUND;
				return this->property_indicies[id_hash];
			}

			//gets the index of a property
			inline unsigned int get_index(identifier_token* identifier) {
				return get_index(identifier->id_hash);
			}

			inline std::list<identifier_token*> get_properties() {
				return this->properties;
			}

			void print();
		};
	}

	namespace runtime {
		class structure {
		private:
			reference_apartment* parent_reference;
			parsing::structure_prototype* prototype;
			reference_apartment** properties;
			structure(parsing::structure_prototype* prototype, reference_apartment* parent_reference);

		public:
			structure(parsing::structure_prototype* prototype, garbage_collector* gc);
			~structure();

			//sets the reference of a property
			void set_reference(unsigned long id_hash, reference_apartment* reference);

			structure* clone(reference_apartment* new_parent_appt);

			int hash();

			inline parsing::identifier_token* get_identifier() {
				return this->prototype->identifier;
			}

			//sets the reference of a property
			inline void set_reference(parsing::identifier_token* identifier, reference_apartment* reference) {
				set_reference(identifier->id_hash, reference);
			}

			//sets the value of a property
			inline void set_value(unsigned long id_hash, value* value) {
				this->properties[this->prototype->get_index(id_hash)]->set_value(value);
			}

			//sets the value of a property
			inline void set_value(parsing::identifier_token* identifier, value* value) {
				set_value(identifier->id_hash, value);
			}

			//gets the reference of a property
			inline reference_apartment* get_reference(unsigned long id_hash) {
				return this->properties[this->prototype->get_index(id_hash)];
			}

			//gets the reference of a property
			inline reference_apartment* get_reference(parsing::identifier_token* identifier) {
				return get_reference(identifier->id_hash);
			}

			//gets the value of a property
			inline value* get_value(unsigned long id_hash) {
				return get_reference(id_hash)->value;
			}

			//gets the value of a property
			inline value* get_value(parsing::identifier_token* identifier) {
				return get_value(identifier->id_hash);
			}

			inline unsigned int get_size() {
				return this->prototype->property_count;
			}

			inline reference_apartment** get_children() {
				return this->properties;
			}

			inline reference_apartment* get_parent_ref() {
				return this->parent_reference;
			}
		};
	}
}
#endif // !STRUCT_H