#Helper
from libcpp.vector  cimport vector as cpp_vector

cdef cppclass _Ca:
    inline _Ca():pass

    cpp_vector[int] conditions
    cpp_vector[int] actions


cdef void _c_add_condition(_Ca &ca, int condition_id) nogil 
cdef void _c_add_action   (_Ca &ca, int action_id)nogil
          
     
      
cdef cppclass CTca:

    object conditions   #owns conditions (vector of python objects)
    object actions      #owns actions    (vector of python objects)
    
    cpp_vector[ cpp_vector[int] ]   trigger_link #trigger_id => vector<link_id>
    cpp_vector[_Ca]                 link_ca      #link_id => _Ca
    
  

cdef CTca* c_new()         nogil
cdef void  c_delete(CTca*) nogil

cdef int c_new_link     (CTca &tca)nogil
cdef int c_new_trigger  (CTca &tca)nogil
cdef int c_new_condition(CTca &tca, object condition)
cdef int c_new_action   (CTca &tca, object action)
     
cdef void c_link_trigger     (CTca &tca, int link_id, int trigger_id)  nogil    
cdef void c_link_condition   (CTca &tca, int link_id, int condition_id)nogil
cdef void c_link_action      (CTca &tca, int link_id, int action_id)   nogil

cdef void c_fire_trigger     (CTca &tca, int trigger_id, object message) 
      
cdef bint _c_check_conditions(CTca &tca, int link_id, object message)
cdef void _c_execute_actions (CTca &tca, int link_id, object message)




