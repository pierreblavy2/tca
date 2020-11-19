
class _Ca:
    def __init__(self):
      self.conditions=[];  #Vector of ints
      self.actions   =[];  #Vector of ints
    
    def add_condition(self,condition_id):
      self.conditions.append(condition_id);  
    
    
    def add_action   (self,action_id):
        self.actions.append(action_id);  
    
    



class Tca:
    def __init__(self):
        self.trigger_link=[] #vector of vector of ints
        self.link_ca     =[] #vector of _Ca
        self.conditions = [];
        self.actions=[];
    
    def new_link(self):
        r = len(self.link_ca)
        self.link_ca.append( _Ca() )
        return r;
       
    def new_trigger  (self):
        r=len(self.trigger_link);
        self.trigger_link.append( [] );
        return r
        
    def new_condition(self, condition):
        r = len(self.conditions)
        self.conditions.append(condition)
        return r
        
    def new_action(self, action):
        r = len(self.actions)
        self.actions.append(action)
        return r
        
    def link_trigger(self, link_id,  trigger_id):
        self.trigger_link[trigger_id].append(link_id);
    
    def link_condition(self, link_id,  condition_id):
        self.link_ca[link_id].add_condition(condition_id);
  
    def link_action(self, link_id,  action_id):
        self.link_ca[link_id].add_action(action_id);
    
    def fire_trigger(self,trigger_id, message):
        for link_id in self.trigger_link[trigger_id]:
            ok = self.check_conditions(link_id,message);
            if(ok):
                self.execute_actions(link_id,message);
            #endif
        #end for
    
    def check_conditions(self, link_id, message):
        for condition_id in self.link_ca[link_id].conditions :
            if(not self.conditions[condition_id].test(message) ):return False;
        return True;
    
    def execute_actions(self,  link_id,  message):
        for action_id in self.link_ca[link_id].actions :
            self.actions[action_id].execute(message)
            
 

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
