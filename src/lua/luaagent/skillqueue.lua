
----------------------------------------------------------------------------
--  skillqueue.lua - Skill queue for agents
--
--  Created: Fri Jan 02 16:31:14 2009
--  Copyright  2008-2009  Tim Niemueller [http://www.niemueller.de]
--
----------------------------------------------------------------------------

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

--- Skill Queue for agents.
-- @author Tim Niemueller
module(..., fawkes.modinit.module_init)


--- @class SkillQueue
-- The SkillQueue takes a number of skills. Upon execution a skill string is formed
-- and send to the skiller via the BlackBoard.
-- @author Tim Niemueller
SkillQueue = {}


--- Constructor.
-- @param o.name name of the SkillQueue
-- @param o.skills skills that will be re-added automatically on each execute or
-- reset.
function SkillQueue:new(o)
   assert(o, "SkillQueue requires a table as argument")
   assert(o.name, "SkillQueue requires a name")
   assert(not getmetatable(o), "Meta table already set for object")
   setmetatable(o, self)
   self.__index = self

   o.base_skills = o.skills or {}
   o.skills      = o:baseskills()
   o.skillstring = ""

   return o
end


--- Get base skills.
-- @return table of basic skills (copied)
function SkillQueue:baseskills()
   local rv = {}
   for _,s in ipairs(self.base_skills) do
      table.insert(rv, s)
   end
   return rv
end


--- Get status for currently executed skill.
-- @param skiller optional skiller interface, if not given or nil then
-- global interfaces.reading.skiller is tried.
-- @return S_INACTIVE is no skill was executed or the skill string currently
-- executed by the skill module does not match the expected string, the
-- status verbatim from the skiller otherwise
function SkillQueue:status(skiller)
   local skiller = skiller or interfaces.reading.skiller
   assert(skiller, "SkillQueue:status: Interface not set")

   if self.skillstring == "" then
      return S_INACTIVE
   elseif self.skillstring == skiller:skill_string() then
      return skiller:status()
   else
      return S_INACTIVE
   end
end

--- Add a skill.
-- @param skillname name of the skill to add
-- @param params skill parameters, an array with elements generated via fixp() and
-- fsmp() functions.
function SkillQueue:add_skill(skillname, params)
   table.insert(self.skills, {skillname, params})
end


--- Sets the parameters for all skills.
-- This method is almost only useful if the SkillQueue is used for a single skill.
-- @param args arguments to set. The table must contain key value pairs, with the
-- key being the skill name and the value being a table of parameters supplied as
-- a table with key to value mappings for each argument
function SkillQueue:set_args(args)
   for _,s in ipairs(self.skills) do
      if args[s[1]] then
	 s.args = args[s[1]]
      end
   end
end


--- Get skill string.
-- Transforms the queue into a skill string.
-- @return skill string.
function SkillQueue:skill_string()
   local rva = {}
   for _,s in ipairs(self.skills) do
      local params = ""
      if s.args or s[2] then -- has params
	 local subp = {}
	 for k,v in pairs(s.args or s[2]) do
	    if type(v) == "table" then
	       -- FSM variable
	       assert(self.fsm, "SkillQueue: FSM not set and fsmp parameter used")
	       for k2,v2 in ipairs(v) do
		  table.insert(subp, string.format("%s = %q", k2, self.fsm.vars[v2]))
	       end
	    elseif type(v) == "boolean" then
	       table.insert(subp, string.format("%s = %s", k, tostring(v)))
	    else
	       table.insert(subp, string.format("%s = %q", k, tostring(v)))
	    end

	    --[[
	    if p.fixed ~= nil then
	       table.insert(subp, string.format("%s = %q", p.name, p.fixed))
	    elseif p.fsmvar ~= nil then
	       assert(self.fsm, "SkillQueue: FSM not set and fsmp parameter used")
	       table.insert(subp, string.format("%s = %q", p.name, self.fsm.vars[p.fsmvar]))
	    else
	       error("SkillQueue: Unrecognized parameter, used fixp() and fsmp()")
	    end
	    --]]
	 end
	 params = table.concat(subp, ", ")
      end
      table.insert(rva, string.format("%s{%s}", s[1], params))
   end

   return table.concat(rva, "; ")
end


--- Execute skills.
-- Forms a skill string and send it to the skiller via the BlackBoard.
-- @param skiller skiller interface, if non is given the global variable
-- interfaces.reading.skiller is tried.
function SkillQueue:execute(skiller)
   local skiller = skiller or interfaces.reading.skiller
   assert(skiller, "SkillQueue:execute: Interface not set")

   self.skillstring = self:skill_string()
   local msg = skiller.ExecSkillContinuousMessage:new(self.skillstring)
   skiller:msgq_enqueue_copy(msg)
   self.skills = self:baseskills()
end


--- Stop skill execution.
-- Sends a StopExecMessage to the skiller.
-- @param skiller skiller interface, if non is given the global variable
-- interfaces.reading.skiller is tried.
function SkillQueue:stop(skiller)
   local skiller = skiller or interfaces.reading.skiller
   assert(skiller, "SkillQueue:execute: Interface not set")

   local msg = skiller.StopExecMessage:new()
   skiller:msgq_enqueue_copy(msg)  
end

--- Reset skill queue.
-- Resets the queue to the base skills.
function SkillQueue:reset()
   self.skills = self:baseskills()
   for _,s in ipairs(self.skills) do
      s.args = nil
   end
   self.skillstring = ""
end


function SkillQueue.oneshot(skills, skiller)
   local skiller = skiller or interfaces.reading.skiller
   local s = SkillQueue:new{name="oneshot", skills=skills}
   local skillstring = s:skill_string()
   local msg = skiller.ExecSkillMessage:new(skillstring)
   skiller:msgq_enqueue_copy(msg)
end
