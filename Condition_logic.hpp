/*
 * Condition_logic.hpp
 *
 *  Created on: 2 nov. 2015
 *      Author: pierre
 */

#ifndef LIB_TCA_CONDITION_LOGIC_HPP_
#define LIB_TCA_CONDITION_LOGIC_HPP_

#include "Tca.hpp"

namespace tca{
namespace logic{

	struct Condition_true : tca::Condition_t<void>{virtual bool check()override final{return true;}};
	struct Condition_false: tca::Condition_t<void>{virtual bool check()override final{return false;}};

	struct Condition_not  : tca::Condition_t<void>{
		Condition_not(tca::Condition_t<void> &a_):a(a_){}
		tca::Condition_t<void> &a;
		virtual bool check()override final{return !a.check();}
	};


	//todo patch to handle conditions with message
	struct Condition_and  : tca::Condition_t<void>{
		Condition_and(Condition_t<void> &a_, Condition_t<void> &b_):a(a_),b(b_){}
		tca::Condition_t<void> &a;
		tca::Condition_t<void> &b;
		virtual bool check()override final{return a.check() and b.check();}
	};

	struct Condition_or  : tca::Condition_t<void>{
		Condition_or(Condition_t<void> &a_, Condition_t<void> &b_):a(a_),b(b_){}
		tca::Condition_t<void> &a;
		tca::Condition_t<void> &b;
		virtual bool check()override final{return a.check() or b.check();}
	};

}}


#endif /* LIB_TCA_CONDITION_LOGIC_HPP_ */
