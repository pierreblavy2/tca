/*Example

#include <tca/Tca.hpp>
#include <iostream>

//--- simple version ---
struct Action_print : tca::Action_t<void>{
	explicit Action_print(const std::string &m):msg(m){}
	void execute()override final{
		std::cout << msg;
	}
	std::string msg;
};


struct Condition_true : tca::Condition_t<void>{
	bool check()override final{return true;}
};

struct Condition_false : tca::Condition_t<void>{
	bool check()override final{return false;}
};


//--- template version ---


struct Condition_print_t : tca::Condition_t<std::string>{
	bool check(const std::string &msg)override final{
		return msg !="don't print me";
	}
};

struct Action_print_t : tca::Action_t<std::string>{
	void execute(const std::string &msg)override final{
		std::cout << msg;
	}
};


int main(){

	//--- simple version dont't pass messages ---
	tca::Tca tca;

	auto b1 = tca.new_block();
	auto c_true  =tca.new_condition_n<Condition_true>("cond_true");
	auto a_print1=tca.new_action_u   <Action_print>  ("OK1\n");
	auto trigger=tca.new_trigger();


	tca.add(b1,trigger);
	tca.add(b1,c_true);
	tca.add(b1,c_true);
	tca.add(b1,a_print1);


	auto b2 = tca.new_block();
	auto c_false =tca.new_condition_u<Condition_false>();
	auto a_print2=tca.new_action_u   <Action_print>  ("SHOULD NOT BE FIRED\n");
	tca.add(b2,trigger);
	tca.add(b2,c_false);
	tca.add(b2,a_print2);

	tca.fire(trigger);

	//--- template version : pass messages ---
	//--may throw error of bad dynamic cast ---
	auto b3      = tca.new_block();
	auto trigger3= tca.new_trigger();
	auto c3_1    = tca.new_condition_u<Condition_print_t>();
	auto c3_2    = tca.new_condition_u<Condition_true>(); //note that conditions that doesn't take a message works
	auto a3      = tca.new_action_u<Action_print_t>();
	tca.add(b3,trigger3);
	tca.add(b3,c3_1);
	tca.add(b3,c3_2);
	tca.add(b3,a3);

	tca.fire<std::string>(trigger3,"Template version OK\n");

	try{
	tca.fire(trigger3);
	}catch(tca::Tca_exception_message_mismatch &e){
		std::cout << "Missmatch message ok with error message : "<< e.what() << "\n";
	}

}

 */

#ifndef LIB_TCA_TCA_HPP_
#define LIB_TCA_TCA_HPP_

#include <UniqueId.hpp>

#include<mutex>
#include<string>

#include<memory>
#include<cassert>

#include<unordered_map>
#include<vector>


//---Exceptions---
#include <mk_exception/begin.hpp>
namespace tca{
	MK_EXCEPTION_BASE(Tca_exception);

	MK_EXCEPTION(Tca_exception_missing_trigger  ,Tca_exception);
	MK_EXCEPTION(Tca_exception_missing_condition,Tca_exception);
	MK_EXCEPTION(Tca_exception_missing_action   ,Tca_exception);

	MK_EXCEPTION(Tca_exception_message_mismatch   ,Tca_exception);


}//end namespace tca
#include <mk_exception/end.hpp>


//--- IDS ---
namespace tca{
	UNIQUE_ID(Trigger  ,size_t)
	UNIQUE_ID(Condition,size_t)
	UNIQUE_ID(Action   ,size_t)
	UNIQUE_ID(Block    ,size_t)
}


//--- Tca base class ---
namespace tca{


	struct Condition_abstract{
		virtual ~Condition_abstract(){}
	};
	typedef std::unique_ptr<Condition_abstract> Condition_ptr;

	template<typename Message_tt>
	struct Condition_t : Condition_abstract{
		typedef Message_tt Message_t;
		virtual ~Condition_t(){}
		virtual bool check(const Message_t &m)=0;
	};

	template<>
	struct Condition_t<void> : Condition_abstract{
		typedef void Message_t;
		virtual ~Condition_t(){}
		virtual bool check()=0;
		template<typename T> bool check(const T&){return check();}
	};




	struct Action_abstract{
		virtual ~Action_abstract(){}
	};
	typedef std::unique_ptr<Action_abstract> Action_ptr;

	template<typename Message_tt>
	struct Action_t : Action_abstract{
		typedef Message_tt Message_t;
		virtual void execute(const Message_t &m)=0;
		virtual ~Action_t(){}
	};

	template<>
	struct Action_t<void> : Action_abstract{
		virtual void execute()=0;
		template<typename T> void execute(const T&){execute();}
		virtual ~Action_t(){}
	};






}//end namespace tca



//--- Tca stuff ---
namespace tca{
	struct Tca{

		Tca()=default;
		template<typename X> explicit Tca(const X&)=delete;

		//conditions and actions
		struct Block_ca{
			std::vector<Condition_id> conditions;
			std::vector<Action_id>    actions;

			template<
			  template<typename, typename...>class Condition_cont,
			  template<typename, typename...>class Action_cont
			>
			Block_ca(const Condition_cont<Condition_id>&c, const Action_cont<Action_id>&a);
			Block_ca(){}

			void add(const Condition_id &c){conditions.emplace_back(c);}
			void add(const Action_id    &a){actions   .emplace_back(a);}

		};


		//triggers
		Trigger_id new_trigger(const std::string & name);
		Trigger_id new_trigger();

		Trigger_id   get_trigger_id  (const std::string &name)const;
		bool exists_trigger  (const std::string &name)const{std::lock_guard<std::mutex> l (mutex_trigger); return name_trigger  .count(name)!=0;}
		bool exists_trigger  (const Trigger_id  &id)  const{std::lock_guard<std::mutex> l (mutex_trigger); return id.get() <vect_trigger.size();}


		template<typename Message_t>
		void fire(const Trigger_id &c, const Message_t &m);

		void fire(const Trigger_id &c);

		//conditions
		Condition_id move_condition(const std::string & name, Condition_ptr &&cond);
		Condition_id move_condition(                          Condition_ptr &&condition);

		template<typename T, typename... Args> Condition_id new_condition_u(Args...a);
		template<typename T, typename... Args> Condition_id new_condition_n(const std::string & name, Args... a);

		Condition_id get_condition_id(const std::string &name)const;
		bool exists_condition(const std::string &name)const{std::lock_guard<std::mutex> l (mutex_condition); return name_condition.count(name)!=0;}
		bool exists_condition(const Condition_id  &id)const{std::lock_guard<std::mutex> l (mutex_condition); return id.get() <vect_condition.size();}



		//actions
		Action_id move_action(const std::string & name, Action_ptr &&action);
		Action_id move_action(Action_ptr &&action);

		template<typename T, typename... Args> Action_id new_action_u(Args... a);
		template<typename T, typename... Args> Action_id new_action_n(const std::string & name, Args... a);

		Action_id get_action_id(const std::string &name)const;
		bool exists_action(const std::string &name)const{std::lock_guard<std::mutex> l (mutex_action); return name_action   .count(name)!=0;}
		bool exists_action(const Action_id &id)    const{std::lock_guard<std::mutex> l (mutex_action); return id.get() <vect_action.size();}



		//blocks (or links)

		template<
		  template<typename, typename...>class Trigger_cont,
		  template<typename, typename...>class Condition_cont,
		  template<typename, typename...>class Action_cont
		>
		Block_id new_block(
				const Trigger_cont  <Trigger_id>   & trigger,
				const Condition_cont<Condition_id> & conditions,
				const Action_cont   <Action_id>    & actions
		);

		Block_id new_block();


		void add(const Block_id &b, const Trigger_id &x);
		void add(const Block_id &b, const Condition_id &x);
		void add(const Block_id &b, const Action_id &x);


		Condition_abstract & get_condition(const Condition_id &x){return *vect_condition[x.get()];}

		template<typename Message_tt>
		Condition_t<Message_tt> & get_condition_t(const Condition_id &x){
			return dynamic_cast<Condition_t<Message_tt>&>(get_condition(x));
		}


		Action_abstract    & get_action(const Action_id &x)   {return *vect_action   [x.get()];}

		template<typename Message_tt>
		Action_t<Message_tt> & get_action_t(const Action_id &x){
			return dynamic_cast<Action_t<Message_tt>&>(get_action(x));
		}


		//throw errors if message type mismatch
		template<typename Message_t>
		void check_block(const Block_ca &b , const Message_t &m);
		void check_block(const Block_ca &b);

	private:
		template<typename Message_t>
		void fire_block( const Block_ca &b , const Message_t &m);
		void fire_block( const Block_ca &b );

		template< typename Message_tt>
		static bool condition_check(Condition_abstract &cond_abstract, const Message_tt &m);

		template< typename Message_tt>
		static void action_execute(Action_abstract &action_abstract, const Message_tt &m);


		template< typename Message_tt> static bool action_is_valid   (const Action_abstract &   );
		template< typename Message_tt> static bool condition_is_valid(const Condition_abstract &);

		template< typename Message_tt> bool action_is_valid   (Action_id    a){return action_is_valid<Message_tt>(get_action(a)   );}
		template< typename Message_tt> bool condition_is_valid(Condition_id c){return action_is_valid<Message_tt>(get_condition(c));}

		std::unordered_map<std::string,Trigger_id>   name_trigger;
		std::unordered_map<std::string,Condition_id> name_condition;
		std::unordered_map<std::string,Action_id>    name_action;

		std::vector< std::vector<Block_id> > vect_trigger; //Block_id is an index in vect_block
		std::vector<Condition_ptr>           vect_condition;
		std::vector<Action_ptr>              vect_action;

		std::vector<Block_ca> vect_block;

		mutable std::mutex mutex_trigger;
		mutable std::mutex mutex_condition;
		mutable std::mutex mutex_action;
		mutable std::mutex mutex_block;

	};





	template<typename Message_t>
	struct Action_broadcast: tca::Action_t<Message_t>{
		Action_broadcast(Tca& t):tca(t){}
		Tca &tca;
		void execute(const Message_t &msg)final override{
			for(const Trigger_id &tid : targets){tca.fire(tid,msg);}
		}
		void add(const Trigger_id &tid){targets.emplace_back(tid);}

		private:
		std::vector<tca::Trigger_id> targets;
	};



}//end namespace tca


//--- implementation ---


template<
  template<typename, typename...>class Condition_cont,
  template<typename, typename...>class Action_cont
>
tca::Tca::Block_ca::Block_ca(const Condition_cont<Condition_id>&c, const Action_cont<Action_id>&a){
	conditions.reserve(c.size());
	actions.reserve(a.size());
	conditions.insert(conditions.end(), c.begin(), c.end());
	actions   .insert(actions   .end(), a.begin(), a.end());
}



//trigger
template<typename Message_tt>
void tca::Tca::fire(const Trigger_id &c, const Message_tt &m){
	std::vector<Block_id> &blocks = vect_trigger[c];
	for(const Block_id &bid : blocks){
		Block_ca & block = vect_block[bid];
		fire_block<Message_tt>(block, m );
	}
}


inline void tca::Tca::fire(const Trigger_id &c){
	std::vector<Block_id> &blocks = vect_trigger[c];
	for(const Block_id &bid : blocks){
		Block_ca & block = vect_block[bid];
		fire_block(block);
	}
}

//conditions
template<typename T, typename... Args>
tca::Condition_id tca::Tca::new_condition_u(Args...a){
	Condition_ptr p = std::make_unique<T>(std::forward<Args>(a)...);
	return move_condition(std::move(p));
}

template<typename T, typename... Args>
tca::Condition_id tca::Tca::new_condition_n(const std::string & name, Args... a){
	Condition_ptr p = std::make_unique<T>(std::forward<Args>(a)...);
	return move_condition(name,std::move(p));
}



//actions
template<typename T, typename... Args>
tca::Action_id tca::Tca::new_action_u(Args... a){
	Action_ptr p = std::make_unique<T>(std::forward<Args>(a)...);
	return move_action(std::move(p));
}

template<typename T, typename... Args>
tca::Action_id tca::Tca::new_action_n(const std::string & name, Args... a){
	Action_ptr p = std::make_unique<T>(std::forward<Args>(a)...);
	return move_action(name,std::move(p));
}


//blocks
template<
  template<typename, typename...>class Trigger_cont,
  template<typename, typename...>class Condition_cont,
  template<typename, typename...>class Action_cont
>
tca::Block_id tca::Tca::new_block(
		const Trigger_cont  <Trigger_id>   & trigger,
		const Condition_cont<Condition_id> & conditions,
		const Action_cont   <Action_id>    & actions
){
	std::unique_lock<std::mutex> lock_block  (mutex_block  ,std::defer_lock);
	std::unique_lock<std::mutex> lock_trigger(mutex_trigger,std::defer_lock);
	std::lock(lock_block,lock_trigger);

	Block_id index{ vect_block.size() };
	vect_block.emplace_back(conditions,actions);
	for(const Trigger_id & id : trigger){
		vect_trigger[id.get()].emplace_back(index.get());
	}
	return index;
}






template< typename Message_tt>
bool  tca::Tca::condition_check(Condition_abstract &cond_abstract, const Message_tt &m){
	//Try to pass the message
	Condition_t<Message_tt>* cond_m = dynamic_cast<Condition_t<Message_tt>*>(&cond_abstract);
	if(cond_m!=nullptr){
		return cond_m->check(m);
	}else{
		//Try with void, throw std::bad_cast if fail
		Condition_t<void>* cond_v = dynamic_cast<Condition_t<void>*>(&cond_abstract);
		if(cond_v==nullptr){throw Tca_exception_message_mismatch("message mismatch for condition");}
		return cond_v->check();
	}
}

template< typename Message_tt>
void  tca::Tca::action_execute(Action_abstract &action_abstract, const Message_tt &m){
	//Try to pass the message
	Action_t<Message_tt>* act_m = dynamic_cast<Action_t<Message_tt>*>(&action_abstract);
	if(act_m!=nullptr){
		act_m->execute(m); return;
	}else{
		//Try with void, throw std::bad_cast if fail
		Action_t<void>* act_v = dynamic_cast<Action_t<void>*>(&action_abstract);
		if(act_v==nullptr){throw Tca_exception_message_mismatch("message mismatch for action");}
		act_v->execute(); return;
	}
}


template< typename Message_tt>
bool tca::Tca::condition_is_valid(const Condition_abstract &c){
	return dynamic_cast<const Condition_t<Message_tt>*>(&c) != nullptr
	   or  dynamic_cast<const Condition_t<void>      *>(&c) != nullptr ;
}

template< typename Message_tt>
bool tca::Tca::action_is_valid(const Action_abstract &a){
	return dynamic_cast<const Action_t<Message_tt>*>(&a) != nullptr
	   or  dynamic_cast<const Action_t<void>      *>(&a) != nullptr ;
}




template<typename Message_tt>
void tca::Tca::fire_block( const Block_ca &b , const Message_tt &m){


	//check conditions
	for(const Condition_id &cid : b.conditions){
		Condition_abstract &cond_a=get_condition(cid);
		if(Tca::condition_check(cond_a,m)==false){return;}
	}

	//if conditions are passed : fire the actions
	for(const Action_id &aid : b.actions){
		Action_abstract &a = get_action(aid);
		Tca::action_execute(a,m);
	}
}

inline void tca::Tca::fire_block( const Block_ca &b){
	//check all conditions once

	for(const Condition_id &cid : b.conditions){
		Condition_abstract & cond_abstract=get_condition(cid);
		Condition_t<void>* cond_v = dynamic_cast<Condition_t<void>*>(&cond_abstract);
		if(cond_v==nullptr){throw Tca_exception_message_mismatch("a message is expected in condition " + std::to_string(cid));}
		if(! cond_v->check()){return;}
	}

	//if conditions are passed : execute the actions
	for(const Action_id &aid : b.actions){
		Action_abstract & act_abstract=get_action(aid);
		Action_t<void>* act_v = dynamic_cast<Action_t<void>*>(&act_abstract);
		if(act_v==nullptr){throw Tca_exception_message_mismatch("a message is expected in action " + std::to_string(aid));}
		act_v->execute();
	}
}


template<typename Message_tt>
void tca::Tca::check_block( const Block_ca &b , const Message_tt &m){

	for(const Condition_id &cid : b.conditions){
		if(! condition_is_valid<Message_tt>(cid)){
			throw Tca_exception_message_mismatch("message mismatch for condition" + std::to_string(cid) );
		}
	}

	for(const Action_id &aid : b.actions){
		if(! action_is_valid<Message_tt>(aid)){
			throw Tca_exception_message_mismatch("message mismatch for action" + std::to_string(aid) );
		}
	}
}

inline void tca::Tca::check_block( const Block_ca &b){
	//check all conditions once
	for(const Condition_id &cid : b.conditions){
		get_condition_t<void>(cid);
	}

	//if conditions are passed : fire the actions
	for(const Action_id &aid : b.actions){
		get_action_t<void>(aid);
	}
}


#endif /* LIB_TCA_TCA_HPP_ */
