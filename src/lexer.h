#pragma once

#ifndef LEXER_H
#define LEXER_H

#include <list>
#include <set>
#include <map>

#include "builtins.h"
#include "tokens.h"

#define GROUP_TYPE_STRUCT 0
#define GROUP_TYPE_FUNC 1
#define GROUP_TYPE_VAR 2

namespace fastcode {
	namespace parsing {
		class lexer {
		public:
			struct lexer_state {
			private:
				struct identifier_system
				{
					std::set<unsigned long> declerations;
					std::list<identifier_token*> references;
				};

				struct group {
				private:
					identifier_system id_systems[3];
					identifier_token* identifier;

					inline void proc_id(identifier_token* id) {
						std::list<char> chars;

						for (int i = 0; i < strlen(id->get_identifier()); i++)
							chars.push_back(id->get_identifier()[i]);

						group* current = this;
						while (current != nullptr)
						{
							chars.push_back('@');
							for (int i = 0; i < strlen(current->identifier->get_identifier()); i++)
								chars.push_back(current->identifier->get_identifier()[i]);
							current = current->parent;
						}

						char* new_buf = new char[chars.size() + 1];
						unsigned long in = 0;
						for (auto i = chars.begin(); i != chars.end(); ++i) {
							new_buf[in++] = *i;
						}
						new_buf[in] = 0;

						id->set_c_str(new_buf);
					}
				public:
					group* parent;

					group(struct identifier_token* identifier, group* parent = nullptr) {
						this->identifier = identifier;
						this->parent = parent;
					}

					inline void proc_decleration(identifier_token* id, unsigned char type) {
						id_systems[type].declerations.insert(id->id_hash);
						proc_id(id);
					}

					inline void proc_reference(identifier_token* id, unsigned char type) {
						id_systems[type].references.push_back(id);
					}

					inline void group_references(bool remall = false) {
						for (unsigned char type = 0; type < 3; type++)
						{
							std::list<std::list<identifier_token*>::iterator> to_remove;
							for (auto i = id_systems[type].references.begin(); i != id_systems[type].references.end(); ++i) {
								if (id_systems[type].declerations.count((*i)->id_hash)) {
									proc_id(*i);
									if (!remall)
										to_remove.push_back(i);
								}
							}
							if (remall)
								id_systems[type].references.clear();
							else
								for (auto i = to_remove.begin(); i != to_remove.end(); ++i) {
									id_systems[type].references.erase(*i);
								}
						}
					}

					~group() {
						group_references(true);
						delete identifier;
					}
				};

				group* top_group = nullptr;
			public:
				std::map<unsigned long, value_token*> constants;

				~lexer_state() {
					for (auto it = this->constants.begin(); it != this->constants.end(); ++it)
						delete (*it).second;
				}

				inline void declare_id(identifier_token* id, unsigned char type) {
					if (top_group != nullptr)
						top_group->proc_decleration(id, type);
				}

				inline void reference_id(identifier_token* id, unsigned char type) {
					if (top_group != nullptr)
						top_group->proc_reference(id, type);
				}

				inline group* current_group() {
					return this->top_group;
				}

				inline void new_group(identifier_token* identifier) {
					top_group = new group(identifier, top_group);
				}

				inline void pop_group() {
					group* to_delete = top_group;
					top_group = top_group->parent;
					delete to_delete;
				}

				inline void group_all_references() {
					group* current = top_group;
					while (current != nullptr) {
						current->group_references();
						current = current->parent;
					}
				}
			};

			lexer(const char* source, unsigned long source_length, lexer_state* lexer_state);
			~lexer();

			inline bool eos() {
				return last_tok == nullptr ? last_char == 0 : last_tok->type == 18 + MAX_TOKEN_LIMIT;
			}

			inline unsigned int get_pos() {
				return this->position;
			}

			std::list<token*> tokenize(bool interactive_mode);
		private:
			const char* source;
			unsigned long position;
			unsigned long source_length;
			char last_char;
			token* last_tok;

			lexer_state* lexer_state;

			//reads the next availible character.
			char read_char();

			//gets the next availible character without eating it
			inline char peek() {
				if (position == source_length) {
					return 0;
				}
				return source[position];
			}

			char read_data_char();

			//reads the next top-level token
			token* read_token();
			token* tokenize_statement(bool interactive_mode);
			std::list<token*> tokenize_body();
			variable_access_token* tokenize_var_access();
			variable_access_token* tokenize_var_access(identifier_token* identifier);
			token* tokenize_expression(unsigned char min = 0);
			token* tokenize_value();
		};
	}
}

#endif // !LEXER_H