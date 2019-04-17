# TCA
TCA is a simple formalism that allows to model decisions rules as triggers, conditions and actions.

When something happens (i.e., a trigger if fired) a set of condition is checked ;  
if all conditions are true then a set of actions is performed.

This formalism allows to express decisions rules in a way that's 
clearer than nested if blocks and that can easily be implemented in many languages. 

#Implementation
The current implementation allows to generate the TCA at runtime and to execute it quickly.

TCA is implemented in *C++* and in *python*. The two implementations are independant.

The code is published under the GPL 3.0 license (see https://www.gnu.org/licenses/gpl-3.0.txt). 
Copyright : Pierre Blavy, INRA.
