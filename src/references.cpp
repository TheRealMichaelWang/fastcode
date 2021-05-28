#include "errors.h"
#include "references.h"

namespace fastcode {
	namespace runtime {
		reference_apartment::reference_apartment(class value* value)
		{
			this->value = value;
			this->gc_collect_flag = false;
		}

		reference_apartment::~reference_apartment() {
			delete value;
		}

		void reference_apartment::trace() {
			this->gc_collect_flag = true;
			std::list<reference_apartment*> children = get_children();
			for (auto i = children.begin(); i != children.end(); i++)
				(*i)->trace();
		}
	}
}