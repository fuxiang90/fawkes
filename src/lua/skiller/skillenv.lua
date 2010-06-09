
----------------------------------------------------------------------------
--  skillenv.lua - Skiller skill environment functions
--
--  Created: Fri Mar 14 22:04:43 2008
--  Copyright  2008-2009  Tim Niemueller [www.niemueller.de]
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
module(..., fawkes.modinit.register_all)
require("fawkes.logprint")
local skillstati = require("skiller.skillstati")
local shsmmod    = require("skiller.skillhsm")
local wsmod      = require("fawkes.fsm.waitstate")
local depinit    = require("fawkes.depinit")
local predlib    = require("fawkes.predlib")
local grapher    = require("fawkes.fsm.grapher")
local set        = require("fawkes.datastruct.set")

local skills        = {}
local active_skills = {}

local skill_space      = ""
local graphing_enabled = true

-- Print skill info.
-- @param skill_entry skill entry to print
function print_skill_info(skill_module)
   print("Skill: " .. skill_module.name)
   print("======================================================================")
   print(skill_module.documentation)
   print("======================================================================")
end

-- Get skill entry.
-- Looks for the requested skill.
-- @param skill name or (wrapped) function of the skill
-- @return skill entry if found, nil otherwise.
function get_skill_module(skill)
   if ( type(skill) == "string" ) then
      for _, s in ipairs(skills) do
	 if s.name == skill then
	    return s
	 end
      end
   elseif type(skill) == "table" then
      local mt = getmetatable(skill)
      for _, s in ipairs(skills) do
	 if s.wrapped_function == mt.__call then
	    return s
	 end
      end
   end

   return nil
end

-- Info about a skill.
-- @param skill skill to print the documentation string for. If nil a list of available
-- skills is printed.
function skill_info(skill)
   if skill == nil then
      print("Available skills:")
      for _, s in ipairs(skills) do
	 print(" %s", s.name)
      end
   else
      local m = get_skill_module(skill)
      if ( m ~= nil ) then
	 print_skill_info(m)
      else
	 print("The queried skill has not been registered")
      end
   end
end

-- template for the sandbox that a skill string is executed in.
local skill_env_template = {
   -- Skiller related stuff
   logger   = logger,
   config   = config,
   clock    = clock,

   -- Packages
   math     = math,
   os       = { date = os.date, getenv = os.getenv, time = os.time, difftime = os.difftime },
   string   = string,
   table    = table,

   -- For debugging only, will be removed eventually
   skills      = skills,

   -- Functions
   assert      = assert,
   error       = error,
   ipairs      = ipairs,
   next        = next,
   print       = print,
   pairs       = pairs,
   print       = fawkes.logprint.print_info,
   printf      = fawkes.logprint.printf,
   print_debug = fawkes.logprint.print_debug,
   print_info  = fawkes.logprint.print_info,
   print_warn  = fawkes.logprint.print_warn,
   print_error = fawkes.logprint.print_error,
   select      = select,
   sinfo       = skill_info,
   skill_info  = skill_info,
   tostring    = tostring,
   type        = type,
   unpack      = unpack,
   xpcall      = xpcall
}


--- Initialize the given skill space.
-- This tries to load and initialize the given skill space and all its
-- skills.
-- @param skillspace skill space to initialize
function init(skillspace)
   skill_space = skillspace

   skill_env_template.interfaces = interfaces

   if interfaces.writing.skiller then
      interfaces.writing.skiller:set_error("")
      interfaces.writing.skiller:write()
   end

   if interfaces.writing.skdbg then
      interfaces.writing.skdbg:set_graph_fsm("")
      interfaces.writing.skdbg:set_graph("")
      interfaces.writing.skdbg:write()
   end

   require("skills." .. SKILLSPACE)
end

--- Generate a sandbox for skill execution.
-- The sandbox is used in the execution thread to create a new safe environment each
-- time a skill string is executed.
-- @return table suitable to be used with setfenv
function gensandbox()
   local rv = {}
   for k,v in pairs(skill_env_template) do
      rv[k] = v
   end
   for k,v in pairs(skillstati) do
      if string.match(k, "^S_([%a_]+)$") then rv[k] = v end
   end
   for _, s in ipairs(skills) do
      assert(not rv[s.name], "Sandbox: Name " .. s.name .. " has already been registered")
      rv[s.name] = create_skill_functable(s, rv)
   end
   rv.dep_skills = {}
   for n, i in pairs(interfaces.reading) do
      assert(not rv[n], "Sandbox: Name " .. n .. " has already been registered")
      rv[n] = i
   end
   for n, i in pairs(interfaces.writing) do
      assert(not rv[n], "Sandbox: Name " .. n .. " has already been registered")
      rv[n] = i
   end

   return rv
end


--- Call reset functions.
-- This iterates over a given array of skill names or functions and executes the reset
-- function for each skill
-- @param t an array with skill names, like a member of skill_status
function reset_skills(t)
   for _,v in ipairs(t) do
      local m = get_skill_module(v)
      if m ~= nil then
	 m.reset()
	 if m.depends_skills ~= nil then
	    for _, s in ipairs(m.depends_skills) do
	       local sm = get_skill_module(s)
	       print_debug("Resetting sub-skill " .. s .. " of skill " .. v)
	       sm.reset()
	    end
	 end
      end
   end
end


--- Reset all.
-- Resets alls skills and the skill status.
function reset_skills_from_status(t)
   reset_skills(t.running)
   reset_skills(t.final)
   reset_skills(t.failed)
end


--- Get the FSM for the given skill, if any.
-- @return the FSM of a skill if it exists, or nil otherwise.
function get_skill_fsm(skill)
   local sm = get_skill_module(skill)
   if sm then
      return sm.fsm
   else
      return nil
   end
end


--- Reset loop internals.
-- This function is called after every loop, no matter if skills are executed
-- continuous or one-show and independent of the status.
function reset_loop()
   predlib.reset()
end


--- Get active skills.
-- The active skills are the last executed skills which have not been resetted, yet.
-- If the skill string executes multiple skills then the skills are listed in the
-- order of their execution. Note that if "the" active skill is discussed this
-- means the skill executed first.

-- The active skills are the skills currently in use by any skill string
-- @return unpacked array of names of active skills
function get_active_skills()
   return unpack(set.to_list(active_skills))
end


--- Write FSM graph to skiller interface.
-- The graph is only written if the FSM has been marked as changed. If
-- fsm is nil the graph is reset to the empty string.
-- @param fsm FSM to get graph from
-- @param interface skiller interface
function write_fsm_graph(fsm, interface)
   assert(interface, "skillenv.write_fsm_graph: no interface!")
   if fsm then
      if fsm:changed() then
	 --print_warn("Writing graph %s to interface", fsm.name)
	 --interface:set_graph_fsm(fsm.name)
	 local graph = fsm:graph()
	 if #graph > interface:maxlenof_graph() then
	    print_error("%s's graph exceeds maximum size (%d vs. %d)",
			fsm.name, #graph, interface:maxlenof_graph())
	 end
	 interface:set_graph(graph)
	 interface:write()
      end
   else
      if interface:graph() ~= "" then
	 --interface:set_graph_fsm("")
	 interface:set_graph("")
	 interface:write()
      end
   end
end


--- Write error to skiller interface from FSM.
-- @param fsm FSM to get the error string from
-- @param interface skiller interface to write to
function write_fsm_error(fsm, interface)
   assert(interface, "skillenv.write_fsm_error: no interface!")
   if fsm and fsm.error and #fsm.error > 0 then
      --print_warn("Writing error to interface")
      interface:set_error(fsm.error)
      interface:write()
      return true
   else
      return false
   end
end


function write_skill_list(skdbg)
   if skdbg:graph_fsm() ~= "LIST" then
      local list = ""
      for _, s in ipairs(skills) do
	 list = list .. string.format("%s\n", s.name)
      end

      skdbg:set_graph_fsm("LIST")
      skdbg:set_graph(list)
      skdbg:write()
   end
end

function update_grapher_config(skdbg, graphdir, colored)
   local params_changed = false

   if graphdir ~= nil then
      local cur_graphdir = grapher.get_rankdir()
      if cur_graphdir ~= graphdir then
	 grapher.set_rankdir(graphdir)
	 if graphdir == "BT" then     skdbg:set_graph_dir(skdbg.GD_BOTTOM_TOP)
	 elseif graphdir == "LR" then skdbg:set_graph_dir(skdbg.GD_LEFT_RIGHT)
	 elseif graphdir == "RL" then skdbg:set_graph_dir(skdbg.GD_RIGHT_LEFT)
	 else skdbg:set_graph_dir(skdbg.GD_TOP_BOTTOM) end
	 params_changed = true
      end
   end
   if colored ~= nil then
      local cur_colored = grapher.get_colored()
      if cur_colored ~= colored then
	 grapher.set_colored(colored)
	 skdbg:set_graph_colored(colored)
	 params_changed = true
      end
   end

   return params_changed
end

function write_skiller_debug(skdbg, what, graphdir, colored)
   local skdbg = skdbg or interfaces.writing.skdbg
   assert(skdbg, "write_skiler_debug: No SkillerDebugInterface given")

   local cur_what = skdbg:graph_fsm()

   if what == "LIST" then
      write_skill_list(skdbg)
   elseif graphing_enabled then
      local sname = what
      if what == "ACTIVE" then
	 sname = get_active_skills()
      end

      local fsm = get_skill_fsm(sname)
      if fsm then
	 local params_changed = update_grapher_config(skdbg, graphdir, colored)

	 if what ~= cur_what or params_changed then
	    fsm:mark_changed()
	    skdbg:set_graph_fsm(what)
	 end

	 write_fsm_graph(fsm, skdbg)
      else
	 if what ~= cur_what then
	    print_warn("Could not write FSM graph, FSM for %s not found", what)
	    skdbg:set_graph_fsm(what)
	    skdbg:set_graph("")
	    skdbg:write()
	 end
      end
   end
end

--- Remove skills from active skills set
-- @param skills the set of names of skills to be removed from the active
-- skills set
function deactivate_skills(skills)
   active_skills = set.difference(active_skills, skills)
end

--- Check skill dependencies
-- This function takes a skill module and checks if the skill is already an
-- active skill. If so an error is thrown. Otherwise all skills the module
-- depends on are also recursively checked. If no error occurs a set that
-- contains the name of all checked skills is returned.
-- @param skill the skill module to check
-- @param dep_skills the skills already checked (in recursive calls)
-- @return a set with the names of all checked skills
function skill_depcheck(skill, dep_skills)
   assert(not active_skills[skill.name],
	  skill.name.." is already an active skill")

   local dep_skills = dep_skills or {}
   assert(not dep_skills[skill.name],
	  "Circular skill dependency in " .. skill.name .. " detected")

   if not skill.stateless then
      set.insert(dep_skills, skill.name)

      if skill.depends_skills then
	 for _,s in ipairs(skill.depends_skills) do
	    dep_skills = set.union(dep_skills,
				   skill_depcheck(get_skill_module(s), dep_skills))
	 end
      end
   end

   return dep_skills
end

--- Create skill wrapper function.
-- This wraps the skill's execute() function into an anonymous functions that
-- does some house keeping and executes the skill. The function expects the module
-- table as the first argument. It is suitable for the create_skill_wrapper()
-- function for generating a functable.
function create_skill_wrapper_func(sandbox)
   return function(skill, ...)
	     -- pre-exec actions
	     if not sandbox.dep_skills[skill.name] then
		sandbox.dep_skills = skill_depcheck(skill)
		active_skills = set.union(active_skills, sandbox.dep_skills)
	     end

	     rv = {skill.execute(...)}

	     -- post-exec actions
	     if rv[1] == skillstati.S_FINAL then
		table.insert(sandbox.__skill_status.final, skill.name)
	     elseif rv[1] == skillstati.S_RUNNING then
		table.insert(sandbox.__skill_status.running, skill.name)
	     else
		table.insert(sandbox.__skill_status.failed, skill.name)
	     end
	     return unpack(rv)
	  end
end


-- Create a skill wrapper functable.
-- Skills are wrapped for the sandbox. They are put into a functable for easy
-- execution, while maintaining access to all of the skills public variables and
-- functions and preventing (accidental) modification of the skill module.
-- @param skill_module module table of the skill
-- 
function create_skill_functable(skill_module, sandbox)
   local t = {}
   local mt = { __call  = create_skill_wrapper_func(sandbox),
		__index = skill_module }
   setmetatable(t, mt)
   return t
end


--- Creates skill FSM execution function.
-- @return function that parses parameters, assigns them properly to the
-- fsm.vars table and then executes the FSM loop and checks the resulting
-- state afterwards. If it's the exit states the function returns S_FINAL, if
-- it's the fail_state it returns S_FAILED, otherwise S_RUNNING.
function skill_fsm_execute_wrapper(fsm)
   return function (...)
	     if not fsm.vars.__set__ then
		local t = ...
		if type(t) == "table" then
		   -- named arguments
		   for k,v in pairs(t) do
		      fsm.vars[k] = v
		   end
		else
		   -- positional arguments
		   if ... then
		      for i,v in ipairs({...}) do
			 fsm.vars[i] = v
		      end
		   end
		end
		fsm.vars.__set__ = true
	     end

	     fsm:loop()
	     if fsm.current.name == fsm.exit_state then
		return skillstati.S_FINAL
	     elseif fsm.fail_state and fsm.current.name == fsm.fail_state then
		return skillstati.S_FAILED
	     else
		return skillstati.S_RUNNING
	     end
	  end
end


--- Creates skill FSM reset function.
-- @return function that resets the fsm appropriately
function skill_fsm_reset_wrapper(fsm)
   return function ()
	     fsm:reset()
	  end
end

--- Simple tostring method for skill modules.
-- @param m skill module
-- @return name of skill
function skill_module_tostring(m)
   return m.name
end

--- Add skill to skill space.
-- This loads and initializes the given skill. If the skill is not found or
-- initialization fails an error is thrown.
-- @param module_name Lua module name of the skill
function use_skill(module_name)
   --printf("Loading skill from module %s", module_name)
   local m = require(module_name)

   assert(m, "Skill module " .. module_name .. " not found")

   -- Must do this here because functions are not defined during
   -- module_init() or skill_module()
   if not m.execute or type(m.execute) ~= "function" then
      -- no execute function, check if has fsm
      assert(m.fsm and m.fsm.exit_state,
	     "Skill " .. module_name .. " does neither provide execute() " ..
	     "function nor FSM with valid exit state")

      m.execute = skill_fsm_execute_wrapper(m.fsm)
   else
      m.stateless = true
   end
   if not m.reset or type(m.reset) ~= "function" then
      -- no execute function, check if has fsm
      assert(m.fsm and m.fsm.exit_state,
	     "Skill " .. module_name .. " does neither provide reset() " ..
	     "function nor FSM with valid exit state")

      m.reset = skill_fsm_reset_wrapper(m.fsm)
   end

   --assert(m.reset and type(m.reset) == "function",
   --       "Skill reset() function not defined or not a function")

   local mt = getmetatable(m)
   assert(mt, "Skill module metatable not set, forgot to call " ..
	  "skill_module() for skill " .. m.name)
   mt.__call = function(t, ...) return m.execute(...) end

   printf("Trying to add skill %s", m.name)

   assert(get_skill_module(m.name) == nil, "A skill with the name " .. m.name .. " already exists")
   m.wrapped_function = create_skill_wrapper_func(m)
   --m.wrapped_table = create_skill_wrapper(m)

   if m.init then
      m.init()
   elseif m.fsm then
      m.fsm:reset()
   end

   table.insert(skills, m)
   printf("Successfully added skill %s to current skill space", m.name)
end


--- Initialize skill module.
-- Exports some basic symbols to the module like SkillHSM, JumpState,
-- SkillJumpState etc.
-- @param m module to initialize
function module_init(m)
   fawkes.modinit.module_init(m)
   m.SkillHSM          = shsmmod.SkillHSM
   m.JumpState         = shsmmod.JumpState
   m.SkillJumpState    = shsmmod.SkillJumpState
   m.SubFSMJumpState   = shsmmod.SubFSMJumpState
   m.WaitState         = wsmod.WaitState
end


-- Initialize a skill module.
-- @param m table of the module to initialize
function skill_module(module_name)
   local m = require(module_name)

   assert(m.name and type(m.name) == "string", "Skill name not set or not a string")
   assert(m.documentation and type(m.documentation) == "string",
	  "Skill documentation missing or not a string")

   local mt = getmetatable(m)
   if mt == nil then
      mt = {}
   else
      assert(mt.__index == nil, "Module metatable already has an __index function/table.")
      assert(mt.__call == nil, "Module metatable already has an __call function/table.")
   end

   local indextable = {}

   for k,v in pairs(skillstati) do
      if string.match(k, "^S_([%a_]+)$") then indextable[k] = v end
   end

   if m.depends_skills then
      assert(type(m.depends_skills) == "table", "Type of depends_skills not table")
      for _,t in ipairs(m.depends_skills) do
	 assert(type(t) == "string", "Type of element in depends_skills is not string")
	 local sm = get_skill_module(t)
	 assert(sm, "Skill " .. t .. " has not been added, dependencies for " .. m.name .. " cannot be met.")
	 indextable[sm.name] = sm
      end
   end

   if m.depends_interfaces then
      depinit.init_interfaces(m.name, m.depends_interfaces, indextable)
   end

   mt.__index    = indextable
   --mt.__tostring = skill_module_tostring

   setmetatable(m, mt)
end
