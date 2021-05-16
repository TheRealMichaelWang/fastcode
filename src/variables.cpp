#include "errors.h"
#include "hash.h"
#include "variables.h"

namespace fastcode {
	namespace runtime {
		variable_manager::variable_bucket::variable_bucket(unsigned long id_hash, reference_apartment* apartment, variable_bucket* next_bucket) {
			this->id_hash = id_hash;
			this->apartment = apartment;
			this->next_bucket = next_bucket;
		}

		variable_manager::variable_manager(class garbage_collector* garbage_collector) {
			this->size = 0;
			this->garbage_collector = garbage_collector;
			for (size_t i = 0; i < VARIABLE_HASH_BUCKET_SIZE; i++)
			{
				hash_buckets[i] = nullptr;
			}
		}

		variable_manager::~variable_manager() {
			for (size_t i = 0; i < VARIABLE_HASH_BUCKET_SIZE; i++)
			{
				if (hash_buckets[i] != nullptr) {
					variable_bucket* current = hash_buckets[i];
					while (current != nullptr)
					{
						variable_bucket* to_delete = current;
						to_delete->apartment->remove_reference();
						current = current->next_bucket;
						delete to_delete;
					}
				}
			}
		}

		reference_apartment* variable_manager::declare_var(unsigned long id_hash, reference_apartment* reference) {
			variable_bucket* bucket = hash_buckets[id_hash % VARIABLE_HASH_BUCKET_SIZE];
			variable_bucket* parent = nullptr;
			while (bucket != nullptr) {
				if (bucket->id_hash == id_hash)
					throw ERROR_VARIABLE_ALREADY_DEFINED;
				parent = bucket;
				bucket = bucket->next_bucket;
			}
			reference->add_reference();
			if (parent == nullptr)
				hash_buckets[id_hash % VARIABLE_HASH_BUCKET_SIZE] = new variable_bucket(id_hash, reference);
			else
				parent = new variable_bucket(id_hash, reference);
			return reference;
		}

		void variable_manager::remove_var(unsigned long id_hash) {
			variable_bucket* bucket = hash_buckets[id_hash % VARIABLE_HASH_BUCKET_SIZE];
			variable_bucket* parent = nullptr;
			while (bucket != nullptr)
			{
				if (bucket->id_hash == id_hash) {
					break;
				}
				parent = bucket;
				bucket = bucket->next_bucket;
			}
			if (bucket == nullptr)
				throw ERROR_VARIABLE_NOT_DEFINED;
			if (parent == nullptr)
				hash_buckets[id_hash % VARIABLE_HASH_BUCKET_SIZE] = bucket->next_bucket;
			else
				parent->next_bucket = bucket->next_bucket;
			bucket->apartment->remove_reference();
			delete bucket;
		}

		bool variable_manager::has_var(unsigned long id_hash)
		{
			variable_bucket* bucket = hash_buckets[id_hash % VARIABLE_HASH_BUCKET_SIZE];
			while (bucket != nullptr) {
				if (bucket->id_hash == id_hash)
					return true;
				bucket = bucket->next_bucket;
			}
			return false;
		}

		void variable_manager::set_var_reference(unsigned long id_hash, reference_apartment* reference) {
			variable_bucket* bucket = hash_buckets[id_hash % VARIABLE_HASH_BUCKET_SIZE];
			while (bucket != nullptr) {
				if (bucket->id_hash == id_hash) {
					bucket->apartment->remove_reference();
					reference->add_reference();
					bucket->apartment = reference;
					return;
				}
				bucket = bucket->next_bucket;
			}
			throw ERROR_VARIABLE_NOT_DEFINED;
		}

		reference_apartment* variable_manager::get_var_reference(unsigned long id_hash) {
			variable_bucket* bucket = hash_buckets[id_hash % VARIABLE_HASH_BUCKET_SIZE];
			while (bucket != nullptr) {
				if (bucket->id_hash == id_hash) {
					return bucket->apartment;
				}
				bucket = bucket->next_bucket;
			}
			throw ERROR_VARIABLE_NOT_DEFINED;
		}
	}
}