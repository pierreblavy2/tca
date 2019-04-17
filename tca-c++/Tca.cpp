#include <tca/Tca.hpp>


//triggers

tca::Trigger_id tca::Tca::new_trigger(const std::string & name){
	std::lock_guard<std::mutex> l(mutex_trigger);
	assert(name_trigger.find(name)==name_trigger.end());
	Trigger_id r { vect_trigger.size() };
	vect_trigger.emplace_back();
	name_trigger[name]=r;
	return r;
}

tca::Trigger_id tca::Tca::new_trigger(){
	std::lock_guard<std::mutex> l(mutex_trigger);
	Trigger_id r { vect_trigger.size() };
	vect_trigger.emplace_back();
	return r;
}


tca::Trigger_id tca::Tca::get_trigger_id  (const std::string &name)const{
	return name_trigger.at(name);
}


//conditions

tca::Condition_id tca::Tca::move_condition(const std::string & name, Condition_ptr &&cond){
	std::lock_guard<std::mutex> l(mutex_condition);
	assert(name_condition.find(name)==name_condition.end());
	Condition_id r{vect_condition.size()};
	name_condition[name]=r;
	vect_condition.emplace_back(std::move(cond));
	return r;
}

tca::Condition_id tca::Tca::move_condition(Condition_ptr &&condition){
	std::lock_guard<std::mutex> l(mutex_condition);
	Condition_id r{vect_condition.size()};
	vect_condition.emplace_back(std::move(condition));
	return r;
}


//actions

tca::Action_id tca::Tca::move_action(const std::string & name, Action_ptr &&action){
	std::lock_guard<std::mutex> l(mutex_action);
	assert(name_action.find(name)==name_action.end());
	Action_id r{vect_action.size()};
	name_action[name]=r;
	vect_action.emplace_back(std::move(action));
	return r;
}

tca::Action_id tca::Tca::move_action(Action_ptr &&action){
	std::lock_guard<std::mutex> l(mutex_action);
	Action_id r{vect_action.size()};
	vect_action.emplace_back(std::move(action));
	return r;
}


//blocks
tca::Block_id tca::Tca::new_block(){
	std::lock_guard<std::mutex> l(mutex_block);
	Block_id index{ vect_block.size() };
	vect_block.emplace_back();
	return index;
}

//be carefll don't call multiple times
void tca::Tca::add(const Block_id &b, const Trigger_id &x){
	std::lock_guard<std::mutex> l(mutex_block);
	vect_trigger[x.get()].emplace_back(b);
}


void tca::Tca::add(const Block_id &b, const Condition_id &x){
	std::lock_guard<std::mutex> l(mutex_block); //todo lock at a finer grain?
	vect_block[b].conditions.emplace_back(x);
}

void tca::Tca::add(const Block_id &b, const Action_id &x){
	std::lock_guard<std::mutex> l(mutex_block); //todo lock at a finer grain?
	vect_block[b].actions.emplace_back(x);
}





#undef MK_ID_GENERATOR

