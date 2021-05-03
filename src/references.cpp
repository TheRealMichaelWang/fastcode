#include "errors.h"
#include "references.h"

reference_apartment::reference_apartment(class value* value, reference_apartment* next_apartmen)
{
	this->value = value;
	this->next_apartment = next_apartment;
	this->references = 0;
}

reference_apartment::~reference_apartment() {
	delete value;
}

void reference_apartment::add_reference() {
	this->references++;
	unsigned int children_count = 0;
	reference_apartment** children = get_children(&children_count);
	if (children != nullptr) {
		for (size_t i = 0; i < children_count; i++)
		{
			children[i]->add_reference();
		}
	}
}

void reference_apartment::add_reference(reference_apartment* parent) {
	this->references += parent->references;
	unsigned int children_count = 0;
	reference_apartment** children = get_children(&children_count);
	if (children != nullptr) {
		for (size_t i = 0; i < children_count; i++)
		{
			children[i]->add_reference(parent);
		}
	}
}

void reference_apartment::remove_reference() {
	if (this->references == 0)
		throw ERROR_CANNOT_DEREFERENCE;
	this->references--;
	unsigned int children_count = 0;
	reference_apartment** children = get_children(&children_count);
	if (children != nullptr) {
		for (size_t i = 0; i < children_count; i++)
		{
			children[i]->remove_reference();
		}
	}
}

void reference_apartment::remove_reference(reference_apartment* parent) {
	if (this->references == 0)
		throw ERROR_CANNOT_DEREFERENCE;
	this->references -= parent->references;
	unsigned int children_count = 0;
	reference_apartment** children = get_children(&children_count);
	if (children != nullptr) {
		for (size_t i = 0; i < children_count; i++)
		{
			children[i]->remove_reference(parent);
		}
	}
}

void reference_apartment::set_value(class value* value) {
	unsigned int children_count = 0;
	reference_apartment** children = get_children(&children_count);
	if (children != nullptr) {
		for (size_t i = 0; i < children_count; i++)
		{
			if (this->references > 0)
				children[i]->remove_reference(this);
		}
	}
	delete this->value;
	this->value = value;
	children = get_children(&children_count);
	if (children != nullptr) {
		for (size_t i = 0; i < children_count; i++)
		{
			if (this->references > 0)
				children[i]->add_reference(this);
		}
	}
}