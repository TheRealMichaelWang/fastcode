#pragma once

#ifndef REFERENCE_H
#define REFERENCE_H

#include <list>
#include "value.h"

namespace fastcode {
	namespace runtime {
		class reference_apartment {
		private:
			value* value;
			bool gc_collect_flag;

			//gets a list of immediate children
			std::list<reference_apartment*> get_children();
			
			//performs a garbage-collector trace
			void trace();

			friend class garbage_collector;
		public:
			reference_apartment(class value* value);
			~reference_apartment();
		
			inline const class value* get_value() {
				return this->value;
			}
		};
	}
}

#endif // !REFERENCE_H
