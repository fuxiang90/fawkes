
------------------------------------------------------------------------
--  grapher.lua - FSM DOT Grapher
--
--  Created: Tue Dec 23 00:02:34 2008
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

--- Module to create DOT graphs.
-- @author Tim Niemueller
module(..., fawkes.modinit.module_init)
local gmod = require("fawkes.dotgraph")

local colored_output = true
local rankdir = "TB"
local integrated_subfsm = true


--- Enable/disable colored output.
-- @param colored true to enable colored output, false to disable
function set_colored(colored)
   colored_output = colored
end

--- Set rank direction.
-- Set one of "TB", "LR", "BT", "RL", corresponding to directed graphs drawn
-- from top to bottom, from left to right, from bottom to top, and from right
-- to left, respectively (cf. rankdir attribute of dot graphs in Graphviz
-- documenation).
-- @param new_rankdir new rank direction
function set_rankdir(new_rankdir)
   rankdir = new_rankdir
end

--- Get rankdir.
-- @return rankdir of generated graphs
function get_rankdir()
   return rankdir
end

--- Get flag if colored output is enabled.
-- @return true if colored output is enabled, false otherwise
function get_colored()
   if colored_output then return true else return false end
end

--- Enable/disable integrated sub-FSM mode.
-- If enabled, the state employing a sub-fsm will have an edge leading to the
-- initial node of the sub-FSM and the exit and fail states of the sub-FSM will
-- have edges to the fail_to and exit_to statesof the sub-FSM state. Otherwise
-- the employing node only has a "uses" connection to the subgraph cluster of
-- the sub-FSM and direction connections to the fail_to and exit_to nodes.
-- @param enable true to enable subgraph integration, false to disable
function set_integrated_subfsm(integrated)
   integrated_subfsm = integrated
end

local function generate_dotgraph(fsm, g, subgraph_name)
   assert(fsm, "Grapher requires valid FSM")
   assert(fsm.states, "No states table, not an FSM?")

   --if not gv_load_success then return end
   local is_subgraph = (g ~= nil)

   local g = g or gmod.digraph(fsm.name)

   local defnode = gmod.get_current_default_node(g)
   local defedge = gmod.get_current_default_edge(g)

   local subfsm_num = 1

   if is_subgraph then
      gmod.setv(g, "label", fsm.name)
      -- Currently, this is hardcoded in the SkillGUI because of a Graphviz bug
      -- that prevents us from properly setting bgcolor and pencolor at the same
      -- time.
      --gmod.setv(g, "pencolor", "#aaaaff")
      gmod.setv(g, "style", "filled")
      gmod.setv(g, "bgcolor", "#f4f4f4")
      gmod.setv(g, "fontweight", "bold")
      gmod.setv(g, "labeljust", "r")
   else
      gmod.setv(g, "rankdir", rankdir)
      gmod.setv(g, "penwidth", "2.0")
      if not integrated_subfsm then 
	 gmod.setv(g, "compound")
      end

      gmod.setv(defnode, "penwidth", "2.0")
      gmod.setv(defnode, "shape", "rect")
      gmod.setv(defnode, "style", "rounded,filled")

      gmod.setv(defedge, "penwidth", "2.0")

      if colored_output then
	 gmod.setv(defnode, "color", "#cacaff")
	 gmod.setv(defnode, "fillcolor", "#e6e6ff")

	 gmod.setv(defedge, "color", "#8080ff")
      end
   end

   -- Put the initial node always first, to have the desired output!
   assert(fsm.start and fsm.states[fsm.start], "FSM grapher: No start node set " ..
	  "for FSM or start node not created")
   local start_state = fsm.states[fsm.start]
   local snn = is_subgraph and subgraph_name .. "_" .. fsm.start or fsm.start
   local start_node = gmod.node(g, snn)
   gmod.setv(start_node, "penwidth", "4.0")
   if colored_output then
      gmod.setv(start_node, "color", "#ffc080")
   end
   if is_subgraph then
      gmod.setv(start_node, "label", fsm.start)
   end
   if fsm.current and fsm.current == start_state then
      if colored_output then
	 gmod.setv(start_node, "fillcolor", "#ffc080")
      else
	 gmod.setv(start_node, "style", "dotted")
      end
   end
   gmod.setvl(start_node, start_state.dotattr)

   for name, state in pairs(fsm.states) do
      --print("*** Adding state " .. name)
      if name ~= fsm.start then
	 local nn = is_subgraph and subgraph_name .. "_" .. name or name
	 local n = gmod.node(g, nn)
	 gmod.setvl(n, state.dotattr)

	 local current_name = fsm.current and fsm.current.name or ""

	 if is_subgraph then
	    gmod.setv(n, "label", name)
	 end

	 -- Check if this is an exit node
	 if name == fsm.exit_state or name == fsm.fail_state then
	    gmod.setv(n, "shape", "doubleoctagon")
	    gmod.setv(n, "style", "solid,filled")
	    gmod.setv(n, "penwidth", "1.0")
	 end

	 if fsm.current and name == current_name then
	    if colored_output then
	       if current_name == fsm.exit_state then
		  gmod.setv(n, "fillcolor", "#ccffcc")
	       elseif current_name == fsm.fail_state then
		  gmod.setv(n, "fillcolor", "#ffcccc")
	       else
		  gmod.setv(n, "fillcolor", "#ffc080")
	       end
	    else
	       gmod.setv(n, "style", "dotted")
	    end
	 elseif fsm.tracing and fsm:traced_state(state) then
	    if colored_output then
	       gmod.setv(n, "color", "#ff8000")
	    else
	       gmod.setv(n, "style", "dashed,rounded")
	    end
	 end

	 if colored_output then
	    if name == fsm.exit_state then
	       gmod.setv(n, "color", "#6fdd6f") -- 80ff80
	    elseif name == fsm.fail_state then
	       gmod.setv(n, "color", "#ff8080")
	    end
	 end
      end

      if state.subfsm then
	 local subfsm   = state.subfsm
	 local name     = subfsm.name
	 local subgraph_name = "g" .. tostring(subfsm_num)
	 if not string.match(name, "^cluster") then
	    name = "cluster_" .. tostring(subfsm_num)
	    subfsm_num = subfsm_num + 1
	 end
	 local subgraph = gmod.subgraph(g, name)
	 generate_dotgraph(subfsm, subgraph, subgraph_name)
	 local e = gmod.edge(g, state.name, subgraph_name .. "_" .. subfsm.start)
	 if not integrated_subfsm then
	    gmod.setv(e, "lhead", name)
	    gmod.setv(e, "arrowhead", "dot")
	    gmod.setv(e, "label", "uses")
	    gmod.setv(e, "style", "dashed")
	 else
	    if state.exit_to and subfsm.exit_state then
	       gmod.edge(g, subgraph_name .. "_" .. subfsm.exit_state, state.exit_to)
	    end
	    if state.fail_to and subfsm.fail_state then
	       gmod.edge(g, subgraph_name .. "_" .. subfsm.fail_state, state.fail_to)
	    end
	 end
	 if fsm.current and state_name == fsm.current.name then
	    gmod.setv(subgraph, "active")
	 end
      end

      if state.transitions then
	 for _, tr in ipairs(state.transitions) do
	    if not state.subfsm or not integrated_subfsm or
	       state.final_transition ~= tr and state.failure_transition ~= tr then

	       --print("*** Adding transition " .. name .. " -> " .. tr.state.name .. "(" .. tr.description .. ")")
	       local from = is_subgraph and subgraph_name .. "_" .. name or name
	       local to = is_subgraph and subgraph_name .. "_" .. tr.state.name or tr.state.name

	       local e = gmod.edge(g, from, to)
	       if tr.description then
		  gmod.setv(e, "label", tostring(tr.description))
	       end
	       gmod.setvl(e, tr.dotattr)

	       if fsm.fail_state and tr.state.name == fsm.fail_state then
		  if colored_output then
		     gmod.setv(e, "style", "dotted")
		  end
	       end
	       if fsm.exit_state and name == fsm.exit_state or
		  fsm.fail_state and name == fsm.fail_state then
		  if colored_output then
		     gmod.setv(e, "color", "red3")
		  end
		  gmod.setv(e, "style", "dashed")
	       end
	       if fsm.tracing then
		  local traced, traces = fsm:traced_trans(tr)
		  if traced then
		     local sorted_indexes = {}
		     for i,_ in pairs(traces) do
			table.insert(sorted_indexes, i)
		     end
		     table.sort(sorted_indexes)
		     if next(sorted_indexes) then
			local s = tostring(sorted_indexes[1])
			for i=2,#sorted_indexes do
			   s = s .. "," .. tostring(sorted_indexes[i])
			end
			gmod.setv(e, "taillabel", s)
			if colored_output then
			   gmod.setv(e, "labelfontcolor", "#ff8000")
			end
		     end
		     if colored_output then
			gmod.setv(e, "color", "#ff8000")
		     else
			gmod.setv(e, "style", "dashed")
		     end
		  end
	       end
	    end
	 end
      end
   end
   return g
end

--- Generate DOT graph.
-- @param fsm FSM to produce the graph for
-- @return string containing the graph in the DOT language
function dotgraph(fsm)
   local g = generate_dotgraph(fsm)
   return gmod.generate(g)
end
