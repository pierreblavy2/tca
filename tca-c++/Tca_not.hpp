//Helper to generate NOT conditions only once


#ifndef LIB_TCA_TCA_NOT_HPP_
#define LIB_TCA_TCA_NOT_HPP_

#include "Tca.hpp"


namespace tca{

template<typename Message_tt>
struct Condition_not : Condition_t<Message_tt>{
	typedef Message_tt Message_t;

	explicit Condition_not(Tca & t, const Condition_id c):tca(t),condition_id(c){}
	Tca & tca;
	Condition_id condition_id;

	bool check(const Message_t &m)override final{
		return ! tca.get_condition_t<Message_tt>(condition_id).check(m);
	}
};


template<>
struct Condition_not<void> : Condition_t<void>{
	explicit Condition_not(Tca & t, const Condition_id c):tca(t),condition_id(c){}
	Tca & tca;
	Condition_id condition_id;

	bool check()override final{
		return ! tca.get_condition_t<void>(condition_id).check();
	}
};



struct Tca_not{
	explicit Tca_not (Tca &t):tca(t){}

	template<typename Message_t>
	Condition_id get_not(const Condition_id &id){
		//exists return it
		auto f = not_map.find(id);
		if(f!=not_map.end()){return f->second;}

		//else create Condition
		auto cond = std::make_unique<Condition_not<Message_t>>(tca,id);
		Condition_id condition_not = tca.move_condition(std::move(cond));

		not_map[id]=condition_not;
		not_map[condition_not]=id;
		return condition_not;

	}

	std::map<Condition_id,Condition_id> not_map;
	Tca & tca;

};
}






#endif /* LIB_TCA_TCA_NOT_HPP_ */
