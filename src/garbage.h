#pragma once

#ifndef GARBAGE_H
#define GARBAGE_H

#include <list>
#include <stack>
#include "references.h"

namespace fastcode {
	namespace runtime {
		class garbage_collector {
		private:
			class garbage_frame {
			private:
				std::list<reference_apartment*> references;
				
				void reset_flags();

			public:
				garbage_frame();
				~garbage_frame();

				void collect(garbage_frame* prev_frame, garbage_frame* static_frame);

				inline void new_reference(reference_apartment* reference) {
					this->references.push_back(reference);
				}
			};

			std::stack<garbage_frame*> garbage_frames;
		public:
			garbage_collector();
			~garbage_collector();

			void new_frame();
			void collect_frame(bool pop);
		};
	}
}

#endif // !GARBAGE_H