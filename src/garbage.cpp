#include "garbage.h"

garbage_collector::garbage_collector() {
	this->size = 0;
	this->head = nullptr;
	this->sweep_frames = std::stack<reference_apartment*>();
}

garbage_collector::~garbage_collector() {
	while (size > 0)
	{
		sweep();
	}
}

reference_apartment* garbage_collector::new_apartment(value* initial_value) {
	if (size == 0) {
		size++;
		return (tail = (head = new reference_apartment(initial_value)));
	}
	else {
		size++;
		tail->next_apartment = new reference_apartment(initial_value);
		tail = tail->next_apartment;
		return tail;
	}
}

unsigned int garbage_collector::sweep() {
	unsigned int destroyed_values = 0;
	reference_apartment* current = sweep_frames.empty() ? head : sweep_frames.top()->next_apartment;
	reference_apartment* previous = sweep_frames.empty() ? nullptr : sweep_frames.top();
	while (current != nullptr)
	{
		if (current->can_delete()) {
			reference_apartment* to_delete = current;
			current = current->next_apartment;
			if (previous == nullptr)
				head = current;
			else
				previous->next_apartment = current;
			delete to_delete;
			size--;
			destroyed_values++;
		}
		else {
			previous = current;
			current = current->next_apartment;
		}
	}
	tail = previous;
	if (!sweep_frames.empty())
		sweep_frames.pop();
	return destroyed_values;
}