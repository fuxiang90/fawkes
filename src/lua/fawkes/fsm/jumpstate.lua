
------------------------------------------------------------------------
--  jumpstate.lua - FSM Jump state to build Hybrid State Machines (HSM)
--
--  Created: Thu Dec 04 10:40:54 2008
--  Copyright  2008-2009  Tim Niemueller [www.niemueller.de]
--
------------------------------------------------------------------------

--  This program is free software; you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation; either version 2 of the License, or
--  (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU Library General Public License for more details.
--
--  Read the full text in the LICENSE.GPL file in the doc directory.

require("fawkes.modinit")

--- This module provides a state to create a Hybrid State Machine (HSM)
-- @author Tim Niemueller
module(..., fawkes.modinit.module_init)
local fsmmod = require("fawkes.fsm")
assert(fsmmod, "fsmmod is nil")
local State = fsmmod.State
assert(State, "State for JumpState is nil")


--- @class JumpState
-- Hybrid State Machine jump state for FSM.  This class provides a
-- generic state that makes a Finite State Machine a Hybrid State
-- Machine (iff only jump states are used). Jump states are states
-- that do not return a follow state in loop(), but rather set a
-- number of predefined transitions. Each transition has a jump
-- condition and the target state. On a call to do_loop() all jump
-- conditions are evaluated. If one matches this transition is
-- executed. If multiple could match then only the very first
-- transition is executed. The order is determined by the order the
-- transitions where added.
-- @author Tim Niemueller
JumpState = { comment           = "",
              clear_transitions = State.clear_transitions,
	      get_transition    = State.get_transition,
	      get_transitions   = State.get_transitions,
	      last_transition   = State.last_transition,
	      init              = State.init,
	      exit              = State.exit,
	      loop              = State.loop,
	      reset             = State.reset,
	      do_exit           = State.do_exit}

--- Create new jump state.
-- @param o table with initializations for the object.
-- @return Initialized FSM state
function JumpState:new(o)
   assert(o, "JumpState requires a table as argument")
   assert(o.name, "JumpState requires a name")
   assert(o.fsm, "JumpState " .. o.name .. " requires a FSM")
   assert(not getmetatable(o), "Meta table already set for object")
   setmetatable(o, self)
   self.__index = self

   o.dotattr = o.dotattr or {}
   o.transitions = o.transitions or {}
   o.preconditions = {}
   o.loops = o.loops or {}
   o.inits = o.inits or {}

   o:setup_timeout()

   assert(type(o.transitions) == "table", "Transitions for " .. o.name .. " not a table")

   return o
end

--- Execute init routines.
-- This executes the init method. This may contain some general code
-- executed for every state for derived states. 
-- @param ... Any parameters, passed to the init() function.
function JumpState:do_init(...)
   local rv = { self:try_transitions(self.preconditions) }
   if next(rv) then return unpack(rv) end
   self:init(...)
   for _,i in ipairs(self.inits) do
      i(self)
   end
   return self:try_transitions()
end


--- Execute jump state loop.
-- This will execute the loop and afterwards check if any jump condition
-- of the registered transitions holds. Iff one holds the checks are immediately
-- abort and the transition is executed.
-- @return new state if jump condition holds or false otherwise
function JumpState:do_loop()
   self:loop()
   for _,l in ipairs(self.loops) do
      l(self)
   end

   return self:try_transitions()
end


--- Add transition as precondition.
-- The given transition must already exist in the list of transitions of this
-- state. Then the transition is added to the list of preconditions. That is it
-- is checked just before init() would be run. If any of the jump conditions of
-- the precondition transition fires the transition is executed immediately and
-- the state is never run (init() and loop() are not called.
function JumpState:add_precondition(transition)
   local ok = false
   for _,t in ipairs(self.transitions) do
      if t == transition then
	 ok = true
	 break
      end
   end
   assert(ok, "JumpState/" .. self.name .. ":add_precondition: transition has not been added via add_transition before")

   table.insert(self.preconditions, transition)

   return transition
end

--- Add transition.
-- Adds a transition for this state. Jump conditions are executing after loop()
-- has run to check if a transition should happen. The jump condition is a
-- function whose first return value must be boolean. Iff the return value is
-- true then the condition holds/fires and the transition is executed. No further
-- jump conditions will be tried in that case. All (optional) following return
-- values are passed verbatim to the init() function of the state the transition
-- points to.
-- @param state state to switch to if jumpcond holds
-- @param jumpcond jump condition function, see description above.
-- @param description a string representation of the jump condition, can
-- be a plain copy of the code as string or a verbal description, used for
-- debugging and graph generation
function JumpState:add_transition(state, jumpcond, description)
   assert(state, self.name .. ": Follow state is nil while adding '" .. tostring(description) .. "'")
   assert(state.name or type(state) == "string", self.name .. ": Follow state does not have a valid name while adding '" .. tostring(description) .. "'")
   assert(jumpcond, self.name .. ": Jump condition is nil while adding '" .. tostring(description) .. "'")
   --printf("%s: When '%s' -> %s (%s)", self.name, description, state.name, tostring(self.transitions))
   local jc
   if type(jumpcond) == "function" then
      jc = jumpcond
      description = description or ""
   elseif type(jumpcond) == "string" then
      jc = assert(loadstring("return " .. jumpcond),
		  self.name .. ": compiling jump condition '" .. jumpcond ..
		     "' failed")
      --local fe = getfenv(jc)
      local fe = { string=string, math=math, table=table,
		   os={time=os.time, date=os.date, clock=os.clock, difftime=os.difftime},
		   next=next, rawequal=rawequal, type=type,
		   state=self, self=self, vars=self.fsm.vars }
      if self.closure then
	 for k,v in pairs(self.closure) do fe[k] = v end
      end
      setfenv(jc, fe)
      description = description or jumpcond
   elseif type(jumpcond) == "boolean" then
      assert(jumpcond == true, self.name .. ": adding a jump condition for "..
	     "false does not make any sense, it would never fire")
      jc = JumpState.jumpcond_true
      description = description or "Unconditional"
   else
      error(self.name .. ": type of jump condition must be function, string or boolean")
   end

   local transition = {state       = state,
		       jumpcond    = jc,
		       description = description}
   table.insert(self.transitions, transition)
   return transition
end


--- Add a transition which is also a precondition.
-- This adds a transition with the passed data. Additionally, the transition
-- is made a precondition.
-- @param state state to switch to if jumpcond holds
-- @param jumpcond jump condition function, see description above.
-- @param description a string representation of the jump condition, can
-- be a plain copy of the code as string or a verbal description, used for
-- debugging and graph generation
function JumpState:add_precond_trans(state, jumpcond, description)
   local t = self:add_transition(state, jumpcond, description)
   self:add_precondition(t)
end


--- Prepare the state.
-- This method is called once and only once in the FSMs and states lifetime.
-- It is used for example to resolve forward declaration of states (when a string
-- with the states name was given instead of the state object). If that fails an
-- error is thrown.
function JumpState:prepare()
   assert(self.preconditions, self.name .. ": preconditions table is nil, will cause flaky results")
   for _,t in ipairs(self.transitions) do
      if type(t.state) == "string" then
	 local name = t.state
	 t.state = self.fsm.states[name]
	 assert(t.state and t.state.name, "JumpState.prepare: failed to resolve "..
		"forward declaration of state " .. name .. ", no such state in FSM")
      end
   end
end

--- Try all transitions and return a follow state if applicable.
-- This tries for all added transitions if the jump condition fires. If it does
-- the follow state is returned with any parameters the jump condition might have
-- supplied.
-- @return follow state or nil to stay in current state as first argument, possibly
-- any number of additional arguments that should be passed to the follow state
function JumpState:try_transitions(transtable)
   local transtable = transtable or self.transitions
   --print("Trying conditions for " .. self.name)
   for _,t in ipairs(transtable) do
      local rv = { t.jumpcond(self) }
      local jcfires = rv[1]
      table.remove(rv, 1)
      if jcfires then
	 if self.fsm and self.fsm.debug then
	    print("Jump condition '" .. tostring(t.description) .. "' FIRES, returning " .. t.state.name)
	 end
	 self.last_trans = t
	 return t.state, unpack(rv)
      end
   end

   -- Remain in current state, no jump condition fired
   return nil
end


--- Jump condition that returns always true.
-- This is a convenience method that avoid having to write such a function every
-- time it is needed (for example when using and explicit start state that only
-- transitions to another state or when just sending a BlackBoard message and then
-- advancing to a wait state).
-- @return true
function JumpState:jumpcond_true()
   return true
end

--- Fires on timeout.
-- @return true if the time ran out
function JumpState:jumpcond_timeout()
   return os.difftime(os.time(), self.timeout_start) >= self.timeout_time
end

--- Initializes timeout value.
function JumpState:init_timeout()
   self.timeout_start = os.time()
end


--- Setup timeout.
-- If the timeout field is set, a timeout transition is added. The timeout
-- field must be a table. Either it is an array with two values, the first
-- being the time in seconds, the second being the state where to go to on
-- timeout. Or it can be a table with a time and a to field representing the
-- timeout in seconds and state to go to on timeout respectively.
function JumpState:setup_timeout()
   if self.timeout then
      local timeout_time = self.timeout[1]
      local timeout_to   = self.timeout[2]
      if self.timeout.time then
	 timeout_time = self.timeout.time
	 timeout_to   = self.timeout.to
      end
      assert(timeout_time, "No timeout value given")
      assert(type(timeout_time) == "number", "Timeout value must be a number")
      assert(timeout_to, "No timeout target state given")
      self.timeout_time = timeout_time
      self.timeout_to   = timeout_to
	 
      table.insert(self.inits, self.init_timeout)
      self.timeout_transition = self:add_transition(self.timeout_to, self.jumpcond_timeout, "Timeout (" .. self.timeout_time .. " sec)")
      self.timeout_transition.dotattr = { style = "dashed" }
   end
end




--- Checks a number of interfaces for no writer.
-- This jump condition can be used to check a number of interfaces for a
-- non-existent writer. For this to work you need to set the nowriter_interfaces
-- member variable of the state to an array of interfaces to check.
-- @return true if any of the interfaces in the nowriter_interfaces table does not
-- have a writer, false if no interfaces given or all interfaces have a writer
function JumpState:jumpcond_nowriter()
   if self.nowriter_interfaces and type(self.nowriter_interfaces) == "table" then
      for _,i in ipairs(self.nowriter_interfaces) do
	 if not i:has_writer() then
	    return true
	 end
      end
   else
      printf("nowriter fail")
   end
   return false
end
