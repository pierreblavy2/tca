#Helper class that stores Tca_condition_id and Tca_action_id


#--- _Ca ---
cdef void _c_add_condition(_Ca &ca, int condition_id)nogil:
    ca.conditions.push_back(condition_id);
    
cdef void _c_add_action   (_Ca &ca, int action_id)nogil:
    ca.actions.push_back(action_id);      
          
  
 
 
#--- CTca ---  
cdef CTca* c_new()nogil:
    cdef CTca* tca = new CTca();
    
    with gil:
        tca[0].conditions = [];
        tca[0].actions = [];
    
    return tca;



cdef void c_delete(CTca* tca)nogil:
    del tca;
         
         

cdef int c_new_link(CTca &tca)nogil:
    cdef int r = tca.link_ca.size()
    with gil : tca.link_ca.push_back( _Ca() )
    return r;
       
cdef int c_new_trigger  (CTca &tca)nogil:
    cdef int r = tca.trigger_link.size();
    tca.trigger_link.push_back( cpp_vector[int]()  );
    return r
    
    
cdef int c_new_condition(CTca &tca, object condition):
    cdef int r = len(tca.conditions)
    tca.conditions.append(condition)
    return r
   
   
cdef int c_new_action(CTca &tca, action):
    cdef int r = len(tca.actions)
    tca.actions.append(action)
    return r
    
        
cdef void c_link_trigger(CTca &tca,int link_id, int trigger_id)nogil:
    tca.trigger_link[trigger_id].push_back(link_id);
    
    
cdef void c_link_condition(CTca &tca,int link_id, int condition_id)nogil:
    _c_add_condition( tca.link_ca[link_id],condition_id);
  
    
cdef void c_link_action(CTca &tca,int link_id, int action_id)nogil:
    _c_add_action(tca.link_ca[link_id],action_id);
  
        
cdef void c_fire_trigger(CTca &tca, int trigger_id, object message):
    for link_id in tca.trigger_link[trigger_id]:
        ok = _c_check_conditions(tca,link_id,message);
        if(ok):
            _c_execute_actions(tca,link_id,message);
        #endif
    #end for
  

cdef bint _c_check_conditions(CTca &tca, int link_id, object message):
        for condition_id in tca.link_ca[link_id].conditions :
            if(not tca.conditions[condition_id].test(message) ):return False;
        return True;
        
cdef void _c_execute_actions(CTca &tca, int link_id, object message):
        for action_id in tca.link_ca[link_id].actions :
            tca.actions[action_id].execute(message)
            
 
 
 
#--- TCA ---
#simple wrapper around C functions

cdef class Tca:
    cdef CTca * tca;
    
    def __init__(self):
        self.tca=c_new()
    
    def __del__(self):
        c_delete(self.tca) 
    
    def new_link      (self) :          return c_new_link     (self.tca[0])
    def new_trigger   (self) :          return c_new_trigger  (self.tca[0])
    def new_condition (self,condition): return c_new_condition(self.tca[0], condition)
    def new_action    (self,action):    return c_new_action   (self.tca[0], action)
         
    def link_trigger   (self,link_id,trigger_id):   c_link_trigger   (self.tca[0], link_id, trigger_id)    
    def link_condition (self,link_id,condition_id): c_link_condition (self.tca[0], link_id, condition_id)
    def link_action    (self,link_id,action_id):    c_link_action    (self.tca[0], link_id, action_id)
    
    def fire_trigger   (self,trigger_id,message):   c_fire_trigger   (self.tca[0], trigger_id, message) 
    

            
#User must derive from Tca_action and Tca_condition to write his own classes
#Don't forget to use weak references to models in conditions and actions, as you don't want to perform them on dead models.

class Tca_action:
    #def execute(self,message):raise NotImplementedError("Please implement Tca_action::execute(self,message)");
    def execute(self,message):pass

class Tca_condition:
    #def test    (self,message):raise NotImplementedError("Please implement Tca_condition::test(self,message)->boolean");
    def test    (self,message):pass


    

#--- some classical actions ---
class Tca_action_nothing(Tca_action):
    def execute(self,message):pass

class Tca_action_print(Tca_action):
    def __init__(self,prefix='',postfix=''):
        Tca_action.__init__(self);
        self.prefix=prefix;
        self.postfix=postfix;
          
    def execute(self,message):
        print('{0}{1}{2}'.format(self.prefix,message,self.postfix));



#--- trivial conditions (use them to write tests) ---
class Tca_condition_true(Tca_condition):
    def test    (self,message):return True;

class Tca_condition_false(Tca_condition):
    def test    (self,message):return False;
    
class Tca_condition_debug(Tca_condition):
    def __init__(self,value,prefix='',postfix=''):
        Tca_condition.__init__(self);
        self.value=value;
        self.prefix=prefix;
        self.postfix=postfix;
        
    def test    (self,message):
        print('{0}{1}{2}'.format(self.prefix,self.value,self.postfix));
        return self.value;
 

#WARNING : UNSECURE CODE CAN BE INJECTED IN EXPRESSION !!!       
class Tca_condition_eval(Tca_condition):
    def __init__(self,expression):
        self.expression=expression;
       
    def test    (self,message):
        print( "evaluating:{}".format(self.expression) );
        return eval(self.expression);    