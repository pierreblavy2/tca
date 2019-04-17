#include <tca/Tca.hpp>
#include <iostream>


//Example 1 : When trigger is fired, and condition is true, print "hello"

//--- Action ---
struct Action_print : tca::Action_t<void>{
	explicit Action_print(const std::string &m):msg(m){}
	void execute()override final{
		std::cout << msg;
	}
	std::string msg;
};

//--- Condition ---
struct Condition_simple : tca::Condition_t<void>{
    explicit Condition_simple(bool b):value(b){}
    bool value;
	bool check()override final{return value;}
};


void example1(){

	//--- simple version dont't pass messages ---
	tca::Tca tca;
	
	//triggers
    tca::Trigger_id trigger=tca.new_trigger();
    
    //conditions _n = named,   _u = unamed
	tca::Condition_id c_true  =tca.new_condition_n<Condition_simple>("condition_true", true);
    // or : tca::Condition_id c_true = tca.new_condition_u<Condition_simple>(true);


    //actions
    tca::Action_id a_hello=tca.new_action_n   <Action_print>  ("action_hello", "hello\n");
    //or : tca::Action_id a_hello=tca.new_action_u   <Action_print>  ("hello\n"); 
    	
    //link trigger to c_true and a_print
	auto b1 = tca.new_block();
	tca.add(b1,trigger);
	tca.add(b1,c_true);
	tca.add(b1,a_hello);

	tca.fire(trigger);
}













//Example 2 : When trigger is fired, a message is sent to the actions and the conditions.
//The Condition_msg check_message returns true if the message is_ok member is true
//The action Action_msg print the message txt member


struct Message{
  bool is_ok;
  std::string txt;
};

//--- Condition ---
struct Condition_msg : tca::Condition_t<Message>{
    explicit Condition_msg(){}
	bool check(const Message &m)override final{return m.is_ok;}
};


//--- Action ---
struct Action_msg : tca::Action_t<Message>{
	explicit Action_msg(){}
	void execute(const Message &m)override final{
		std::cout << m.txt;
	}
};


void example2(){

	tca::Tca tca;
	
	//triggers
	tca::Trigger_id trigger = tca.new_trigger();
	
	//conditions
	tca::Condition_id c_msg  =tca.new_condition_u<Condition_msg>();
	
	//actions
	tca::Action_id a_msg  =tca.new_action_u<Condition_msg>();
	
	
	//link trigger to c_msg and a_msg
	auto b1 = tca.new_block();
	tca.add(b1,trigger);
	tca.add(b1,c_msg);
	tca.add(b1,a_msg);
	
	//fire a trigger, that will forward the message
	//NOTE messages types are dynamically checked, be carefull to pass consistent messages at run time
	//as the compiler doesn't check message consistency for you.
	
	Message msg;
	msg.is_ok = true;
	msg.txt   = "hello, I'm a OK message";
	tca.fire<Message>(trigger, msg);
	


}
