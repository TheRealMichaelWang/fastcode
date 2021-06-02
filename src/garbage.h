#pragma once

#ifndef GARBAGE_H
#define GARBAGE_H

#include <stack>
#include "references.h"

namespace fastcode {
	namespace runtime {
		class garbage_collector {
		private:
			unsigned int size;
			reference_apartment* head;
			reference_apartment* tail;
			std::stack<reference_apartment*> sweep_frames;

		public:
			garbage_collector();
			~garbage_collector();

			//initializes a new garbage collection frame
			inline void new_frame() {
				if (tail != nullptr)
					this->sweep_frames.push(tail);
			}

			//creates a new variable apartment within the garbage collector
			reference_apartment* new_apartment(value* initial_value);

			//de-allocates unused variable appartments within the current garbage collection frame
			unsigned int sweep(bool pop_frame);
		};
	}
}

#endif // !GARBAGE_H