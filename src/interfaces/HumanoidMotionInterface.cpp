
/***************************************************************************
 *  HumanoidMotionInterface.cpp - Fawkes BlackBoard Interface - HumanoidMotionInterface
 *
 *  Templated created:   Thu Oct 12 10:49:19 2006
 *  Copyright  2008  Tim Niemueller
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. A runtime exception applies to
 *  this software (see LICENSE.GPL_WRE file mentioned below for details).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL_WRE file in the doc directory.
 */

#include <interfaces/HumanoidMotionInterface.h>

#include <core/exceptions/software.h>

#include <cstring>
#include <cstdlib>

namespace fawkes {

/** @class HumanoidMotionInterface <interfaces/HumanoidMotionInterface.h>
 * HumanoidMotionInterface Fawkes BlackBoard Interface.
 * 
      This interface provides acces to basic humanoid motion patterns.
    
 * @ingroup FawkesInterfaces
 */



/** Constructor */
HumanoidMotionInterface::HumanoidMotionInterface() : Interface()
{
  data_size = sizeof(HumanoidMotionInterface_data_t);
  data_ptr  = malloc(data_size);
  data      = (HumanoidMotionInterface_data_t *)data_ptr;
  data_ts   = (interface_data_ts_t *)data_ptr;
  memset(data_ptr, 0, data_size);
  add_fieldinfo(IFT_BOOL, "walking", 1, &data->walking);
  add_fieldinfo(IFT_ENUM, "supporting_leg", 1, &data->supporting_leg, "LegEnum");
  add_fieldinfo(IFT_FLOAT, "max_step_length", 1, &data->max_step_length);
  add_fieldinfo(IFT_FLOAT, "max_step_height", 1, &data->max_step_height);
  add_fieldinfo(IFT_FLOAT, "max_step_side", 1, &data->max_step_side);
  add_fieldinfo(IFT_FLOAT, "max_step_turn", 1, &data->max_step_turn);
  add_fieldinfo(IFT_FLOAT, "zmp_offset_forward", 1, &data->zmp_offset_forward);
  add_fieldinfo(IFT_FLOAT, "zmp_offset_sideward", 1, &data->zmp_offset_sideward);
  add_fieldinfo(IFT_FLOAT, "l_hip_roll_compensation", 1, &data->l_hip_roll_compensation);
  add_fieldinfo(IFT_FLOAT, "r_hip_roll_compensation", 1, &data->r_hip_roll_compensation);
  add_fieldinfo(IFT_FLOAT, "hip_height", 1, &data->hip_height);
  add_fieldinfo(IFT_FLOAT, "torso_sideward_orientation", 1, &data->torso_sideward_orientation);
  add_fieldinfo(IFT_BOOL, "arms_enabled", 1, &data->arms_enabled);
  add_fieldinfo(IFT_FLOAT, "shoulder_pitch_median", 1, &data->shoulder_pitch_median);
  add_fieldinfo(IFT_FLOAT, "shoulder_pitch_amplitude", 1, &data->shoulder_pitch_amplitude);
  add_fieldinfo(IFT_FLOAT, "elbow_roll_median", 1, &data->elbow_roll_median);
  add_fieldinfo(IFT_FLOAT, "elbow_roll_amplitude", 1, &data->elbow_roll_amplitude);
  add_fieldinfo(IFT_UINT32, "msgid", 1, &data->msgid);
  add_messageinfo("SetWalkParamsMessage");
  add_messageinfo("SetWalkArmsParamsMessage");
  add_messageinfo("StopMessage");
  add_messageinfo("WalkStraightMessage");
  add_messageinfo("WalkSidewaysMessage");
  add_messageinfo("WalkArcMessage");
  add_messageinfo("WalkMessage");
  add_messageinfo("TurnMessage");
  add_messageinfo("KickMessage");
  add_messageinfo("ParkMessage");
  add_messageinfo("GetUpMessage");
  add_messageinfo("StandupMessage");
  add_messageinfo("YawPitchHeadMessage");
  add_messageinfo("SetStiffnessParamsMessage");
  unsigned char tmp_hash[] = {0xd2, 0x56, 0xf, 0x18, 0x96, 0xce, 0x31, 0xaa, 0xd1, 0x85, 0x95, 0xca, 0xe2, 0x1b, 0x64, 0x6};
  set_hash(tmp_hash);
}

/** Destructor */
HumanoidMotionInterface::~HumanoidMotionInterface()
{
  free(data_ptr);
}
/** Convert LegEnum constant to string.
 * @param value value to convert to string
 * @return constant value as string.
 */
const char *
HumanoidMotionInterface::tostring_LegEnum(LegEnum value) const
{
  switch (value) {
  case LEG_LEFT: return "LEG_LEFT";
  case LEG_RIGHT: return "LEG_RIGHT";
  default: return "UNKNOWN";
  }
}
/** Convert StandupEnum constant to string.
 * @param value value to convert to string
 * @return constant value as string.
 */
const char *
HumanoidMotionInterface::tostring_StandupEnum(StandupEnum value) const
{
  switch (value) {
  case STANDUP_DETECT: return "STANDUP_DETECT";
  case STANDUP_BACK: return "STANDUP_BACK";
  case STANDUP_FRONT: return "STANDUP_FRONT";
  default: return "UNKNOWN";
  }
}
/** Convert StiffnessMotionPatternEnum constant to string.
 * @param value value to convert to string
 * @return constant value as string.
 */
const char *
HumanoidMotionInterface::tostring_StiffnessMotionPatternEnum(StiffnessMotionPatternEnum value) const
{
  switch (value) {
  case WALK: return "WALK";
  case KICK: return "KICK";
  default: return "UNKNOWN";
  }
}
/* Methods */
/** Get walking value.
 * True if the robot is currently moving.
 * @return walking value
 */
bool
HumanoidMotionInterface::is_walking() const
{
  return data->walking;
}

/** Get maximum length of walking value.
 * @return length of walking value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_walking() const
{
  return 1;
}

/** Set walking value.
 * True if the robot is currently moving.
 * @param new_walking new walking value
 */
void
HumanoidMotionInterface::set_walking(const bool new_walking)
{
  data->walking = new_walking;
  data_changed = true;
}

/** Get supporting_leg value.
 * Marks the supporting leg
 * @return supporting_leg value
 */
HumanoidMotionInterface::LegEnum
HumanoidMotionInterface::supporting_leg() const
{
  return data->supporting_leg;
}

/** Get maximum length of supporting_leg value.
 * @return length of supporting_leg value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_supporting_leg() const
{
  return 1;
}

/** Set supporting_leg value.
 * Marks the supporting leg
 * @param new_supporting_leg new supporting_leg value
 */
void
HumanoidMotionInterface::set_supporting_leg(const LegEnum new_supporting_leg)
{
  data->supporting_leg = new_supporting_leg;
  data_changed = true;
}

/** Get max_step_length value.
 * 
      Maximum length of a footstep in m.
    
 * @return max_step_length value
 */
float
HumanoidMotionInterface::max_step_length() const
{
  return data->max_step_length;
}

/** Get maximum length of max_step_length value.
 * @return length of max_step_length value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_max_step_length() const
{
  return 1;
}

/** Set max_step_length value.
 * 
      Maximum length of a footstep in m.
    
 * @param new_max_step_length new max_step_length value
 */
void
HumanoidMotionInterface::set_max_step_length(const float new_max_step_length)
{
  data->max_step_length = new_max_step_length;
  data_changed = true;
}

/** Get max_step_height value.
 * 
      Maxium height of a footstep cycloid in m.
    
 * @return max_step_height value
 */
float
HumanoidMotionInterface::max_step_height() const
{
  return data->max_step_height;
}

/** Get maximum length of max_step_height value.
 * @return length of max_step_height value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_max_step_height() const
{
  return 1;
}

/** Set max_step_height value.
 * 
      Maxium height of a footstep cycloid in m.
    
 * @param new_max_step_height new max_step_height value
 */
void
HumanoidMotionInterface::set_max_step_height(const float new_max_step_height)
{
  data->max_step_height = new_max_step_height;
  data_changed = true;
}

/** Get max_step_side value.
 * 
      Maximum length of side step in m.
    
 * @return max_step_side value
 */
float
HumanoidMotionInterface::max_step_side() const
{
  return data->max_step_side;
}

/** Get maximum length of max_step_side value.
 * @return length of max_step_side value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_max_step_side() const
{
  return 1;
}

/** Set max_step_side value.
 * 
      Maximum length of side step in m.
    
 * @param new_max_step_side new max_step_side value
 */
void
HumanoidMotionInterface::set_max_step_side(const float new_max_step_side)
{
  data->max_step_side = new_max_step_side;
  data_changed = true;
}

/** Get max_step_turn value.
 * 
      Maximum change around vertical axis on radians per footstep.
    
 * @return max_step_turn value
 */
float
HumanoidMotionInterface::max_step_turn() const
{
  return data->max_step_turn;
}

/** Get maximum length of max_step_turn value.
 * @return length of max_step_turn value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_max_step_turn() const
{
  return 1;
}

/** Set max_step_turn value.
 * 
      Maximum change around vertical axis on radians per footstep.
    
 * @param new_max_step_turn new max_step_turn value
 */
void
HumanoidMotionInterface::set_max_step_turn(const float new_max_step_turn)
{
  data->max_step_turn = new_max_step_turn;
  data_changed = true;
}

/** Get zmp_offset_forward value.
 * 
      Zero moment point offset in forward direction in m.
    
 * @return zmp_offset_forward value
 */
float
HumanoidMotionInterface::zmp_offset_forward() const
{
  return data->zmp_offset_forward;
}

/** Get maximum length of zmp_offset_forward value.
 * @return length of zmp_offset_forward value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_zmp_offset_forward() const
{
  return 1;
}

/** Set zmp_offset_forward value.
 * 
      Zero moment point offset in forward direction in m.
    
 * @param new_zmp_offset_forward new zmp_offset_forward value
 */
void
HumanoidMotionInterface::set_zmp_offset_forward(const float new_zmp_offset_forward)
{
  data->zmp_offset_forward = new_zmp_offset_forward;
  data_changed = true;
}

/** Get zmp_offset_sideward value.
 * 
      Zero moment point offset in sideward direction in m.
    
 * @return zmp_offset_sideward value
 */
float
HumanoidMotionInterface::zmp_offset_sideward() const
{
  return data->zmp_offset_sideward;
}

/** Get maximum length of zmp_offset_sideward value.
 * @return length of zmp_offset_sideward value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_zmp_offset_sideward() const
{
  return 1;
}

/** Set zmp_offset_sideward value.
 * 
      Zero moment point offset in sideward direction in m.
    
 * @param new_zmp_offset_sideward new zmp_offset_sideward value
 */
void
HumanoidMotionInterface::set_zmp_offset_sideward(const float new_zmp_offset_sideward)
{
  data->zmp_offset_sideward = new_zmp_offset_sideward;
  data_changed = true;
}

/** Get l_hip_roll_compensation value.
 * 
      Amplitude in degrees of backlash compensation for left hip roll.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @return l_hip_roll_compensation value
 */
float
HumanoidMotionInterface::l_hip_roll_compensation() const
{
  return data->l_hip_roll_compensation;
}

/** Get maximum length of l_hip_roll_compensation value.
 * @return length of l_hip_roll_compensation value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_l_hip_roll_compensation() const
{
  return 1;
}

/** Set l_hip_roll_compensation value.
 * 
      Amplitude in degrees of backlash compensation for left hip roll.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @param new_l_hip_roll_compensation new l_hip_roll_compensation value
 */
void
HumanoidMotionInterface::set_l_hip_roll_compensation(const float new_l_hip_roll_compensation)
{
  data->l_hip_roll_compensation = new_l_hip_roll_compensation;
  data_changed = true;
}

/** Get r_hip_roll_compensation value.
 * 
      Amplitude in degrees of backlash compensation for left hip roll.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @return r_hip_roll_compensation value
 */
float
HumanoidMotionInterface::r_hip_roll_compensation() const
{
  return data->r_hip_roll_compensation;
}

/** Get maximum length of r_hip_roll_compensation value.
 * @return length of r_hip_roll_compensation value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_r_hip_roll_compensation() const
{
  return 1;
}

/** Set r_hip_roll_compensation value.
 * 
      Amplitude in degrees of backlash compensation for left hip roll.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @param new_r_hip_roll_compensation new r_hip_roll_compensation value
 */
void
HumanoidMotionInterface::set_r_hip_roll_compensation(const float new_r_hip_roll_compensation)
{
  data->r_hip_roll_compensation = new_r_hip_roll_compensation;
  data_changed = true;
}

/** Get hip_height value.
 * 
      Height of hip during walk process.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @return hip_height value
 */
float
HumanoidMotionInterface::hip_height() const
{
  return data->hip_height;
}

/** Get maximum length of hip_height value.
 * @return length of hip_height value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_hip_height() const
{
  return 1;
}

/** Set hip_height value.
 * 
      Height of hip during walk process.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @param new_hip_height new hip_height value
 */
void
HumanoidMotionInterface::set_hip_height(const float new_hip_height)
{
  data->hip_height = new_hip_height;
  data_changed = true;
}

/** Get torso_sideward_orientation value.
 * 
      Torso orientation in degrees in sideward direction during walking.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @return torso_sideward_orientation value
 */
float
HumanoidMotionInterface::torso_sideward_orientation() const
{
  return data->torso_sideward_orientation;
}

/** Get maximum length of torso_sideward_orientation value.
 * @return length of torso_sideward_orientation value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_torso_sideward_orientation() const
{
  return 1;
}

/** Set torso_sideward_orientation value.
 * 
      Torso orientation in degrees in sideward direction during walking.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @param new_torso_sideward_orientation new torso_sideward_orientation value
 */
void
HumanoidMotionInterface::set_torso_sideward_orientation(const float new_torso_sideward_orientation)
{
  data->torso_sideward_orientation = new_torso_sideward_orientation;
  data_changed = true;
}

/** Get arms_enabled value.
 * 
      If true the arms are controlled during walking for balancing.
    
 * @return arms_enabled value
 */
bool
HumanoidMotionInterface::is_arms_enabled() const
{
  return data->arms_enabled;
}

/** Get maximum length of arms_enabled value.
 * @return length of arms_enabled value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_arms_enabled() const
{
  return 1;
}

/** Set arms_enabled value.
 * 
      If true the arms are controlled during walking for balancing.
    
 * @param new_arms_enabled new arms_enabled value
 */
void
HumanoidMotionInterface::set_arms_enabled(const bool new_arms_enabled)
{
  data->arms_enabled = new_arms_enabled;
  data_changed = true;
}

/** Get shoulder_pitch_median value.
 * 
      Median in radians of the shoulder pitch during walking.
    
 * @return shoulder_pitch_median value
 */
float
HumanoidMotionInterface::shoulder_pitch_median() const
{
  return data->shoulder_pitch_median;
}

/** Get maximum length of shoulder_pitch_median value.
 * @return length of shoulder_pitch_median value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_shoulder_pitch_median() const
{
  return 1;
}

/** Set shoulder_pitch_median value.
 * 
      Median in radians of the shoulder pitch during walking.
    
 * @param new_shoulder_pitch_median new shoulder_pitch_median value
 */
void
HumanoidMotionInterface::set_shoulder_pitch_median(const float new_shoulder_pitch_median)
{
  data->shoulder_pitch_median = new_shoulder_pitch_median;
  data_changed = true;
}

/** Get shoulder_pitch_amplitude value.
 * 
      Amplitude of the shoulder pitch movement during walking.
    
 * @return shoulder_pitch_amplitude value
 */
float
HumanoidMotionInterface::shoulder_pitch_amplitude() const
{
  return data->shoulder_pitch_amplitude;
}

/** Get maximum length of shoulder_pitch_amplitude value.
 * @return length of shoulder_pitch_amplitude value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_shoulder_pitch_amplitude() const
{
  return 1;
}

/** Set shoulder_pitch_amplitude value.
 * 
      Amplitude of the shoulder pitch movement during walking.
    
 * @param new_shoulder_pitch_amplitude new shoulder_pitch_amplitude value
 */
void
HumanoidMotionInterface::set_shoulder_pitch_amplitude(const float new_shoulder_pitch_amplitude)
{
  data->shoulder_pitch_amplitude = new_shoulder_pitch_amplitude;
  data_changed = true;
}

/** Get elbow_roll_median value.
 * 
      Median in radians of the elbow roll during walking.
    
 * @return elbow_roll_median value
 */
float
HumanoidMotionInterface::elbow_roll_median() const
{
  return data->elbow_roll_median;
}

/** Get maximum length of elbow_roll_median value.
 * @return length of elbow_roll_median value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_elbow_roll_median() const
{
  return 1;
}

/** Set elbow_roll_median value.
 * 
      Median in radians of the elbow roll during walking.
    
 * @param new_elbow_roll_median new elbow_roll_median value
 */
void
HumanoidMotionInterface::set_elbow_roll_median(const float new_elbow_roll_median)
{
  data->elbow_roll_median = new_elbow_roll_median;
  data_changed = true;
}

/** Get elbow_roll_amplitude value.
 * 
      Amplitude of the elbow roll movement during walking.
    
 * @return elbow_roll_amplitude value
 */
float
HumanoidMotionInterface::elbow_roll_amplitude() const
{
  return data->elbow_roll_amplitude;
}

/** Get maximum length of elbow_roll_amplitude value.
 * @return length of elbow_roll_amplitude value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_elbow_roll_amplitude() const
{
  return 1;
}

/** Set elbow_roll_amplitude value.
 * 
      Amplitude of the elbow roll movement during walking.
    
 * @param new_elbow_roll_amplitude new elbow_roll_amplitude value
 */
void
HumanoidMotionInterface::set_elbow_roll_amplitude(const float new_elbow_roll_amplitude)
{
  data->elbow_roll_amplitude = new_elbow_roll_amplitude;
  data_changed = true;
}

/** Get msgid value.
 * 
      The ID of the message that is currently being
      processed, or 0 if no message is being processed.
    
 * @return msgid value
 */
uint32_t
HumanoidMotionInterface::msgid() const
{
  return data->msgid;
}

/** Get maximum length of msgid value.
 * @return length of msgid value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::maxlenof_msgid() const
{
  return 1;
}

/** Set msgid value.
 * 
      The ID of the message that is currently being
      processed, or 0 if no message is being processed.
    
 * @param new_msgid new msgid value
 */
void
HumanoidMotionInterface::set_msgid(const uint32_t new_msgid)
{
  data->msgid = new_msgid;
  data_changed = true;
}

/* =========== message create =========== */
Message *
HumanoidMotionInterface::create_message(const char *type) const
{
  if ( strncmp("SetWalkParamsMessage", type, __INTERFACE_MESSAGE_TYPE_SIZE) == 0 ) {
    return new SetWalkParamsMessage();
  } else if ( strncmp("SetWalkArmsParamsMessage", type, __INTERFACE_MESSAGE_TYPE_SIZE) == 0 ) {
    return new SetWalkArmsParamsMessage();
  } else if ( strncmp("StopMessage", type, __INTERFACE_MESSAGE_TYPE_SIZE) == 0 ) {
    return new StopMessage();
  } else if ( strncmp("WalkStraightMessage", type, __INTERFACE_MESSAGE_TYPE_SIZE) == 0 ) {
    return new WalkStraightMessage();
  } else if ( strncmp("WalkSidewaysMessage", type, __INTERFACE_MESSAGE_TYPE_SIZE) == 0 ) {
    return new WalkSidewaysMessage();
  } else if ( strncmp("WalkArcMessage", type, __INTERFACE_MESSAGE_TYPE_SIZE) == 0 ) {
    return new WalkArcMessage();
  } else if ( strncmp("WalkMessage", type, __INTERFACE_MESSAGE_TYPE_SIZE) == 0 ) {
    return new WalkMessage();
  } else if ( strncmp("TurnMessage", type, __INTERFACE_MESSAGE_TYPE_SIZE) == 0 ) {
    return new TurnMessage();
  } else if ( strncmp("KickMessage", type, __INTERFACE_MESSAGE_TYPE_SIZE) == 0 ) {
    return new KickMessage();
  } else if ( strncmp("ParkMessage", type, __INTERFACE_MESSAGE_TYPE_SIZE) == 0 ) {
    return new ParkMessage();
  } else if ( strncmp("GetUpMessage", type, __INTERFACE_MESSAGE_TYPE_SIZE) == 0 ) {
    return new GetUpMessage();
  } else if ( strncmp("StandupMessage", type, __INTERFACE_MESSAGE_TYPE_SIZE) == 0 ) {
    return new StandupMessage();
  } else if ( strncmp("YawPitchHeadMessage", type, __INTERFACE_MESSAGE_TYPE_SIZE) == 0 ) {
    return new YawPitchHeadMessage();
  } else if ( strncmp("SetStiffnessParamsMessage", type, __INTERFACE_MESSAGE_TYPE_SIZE) == 0 ) {
    return new SetStiffnessParamsMessage();
  } else {
    throw UnknownTypeException("The given type '%s' does not match any known "
                               "message type for this interface type.", type);
  }
}


/** Copy values from other interface.
 * @param other other interface to copy values from
 */
void
HumanoidMotionInterface::copy_values(const Interface *other)
{
  const HumanoidMotionInterface *oi = dynamic_cast<const HumanoidMotionInterface *>(other);
  if (oi == NULL) {
    throw TypeMismatchException("Can only copy values from interface of same type (%s vs. %s)",
                                type(), other->type());
  }
  memcpy(data, oi->data, sizeof(HumanoidMotionInterface_data_t));
}

const char *
HumanoidMotionInterface::enum_tostring(const char *enumtype, int val) const
{
  if (strcmp(enumtype, "LegEnum") == 0) {
    return tostring_LegEnum((LegEnum)val);
  }
  if (strcmp(enumtype, "StandupEnum") == 0) {
    return tostring_StandupEnum((StandupEnum)val);
  }
  if (strcmp(enumtype, "StiffnessMotionPatternEnum") == 0) {
    return tostring_StiffnessMotionPatternEnum((StiffnessMotionPatternEnum)val);
  }
  throw UnknownTypeException("Unknown enum type %s", enumtype);
}

/* =========== messages =========== */
/** @class HumanoidMotionInterface::SetWalkParamsMessage <interfaces/HumanoidMotionInterface.h>
 * SetWalkParamsMessage Fawkes BlackBoard Interface Message.
 * 
    
 */


/** Constructor with initial values.
 * @param ini_max_step_length initial value for max_step_length
 * @param ini_max_step_height initial value for max_step_height
 * @param ini_max_step_side initial value for max_step_side
 * @param ini_max_step_turn initial value for max_step_turn
 * @param ini_zmp_offset_forward initial value for zmp_offset_forward
 * @param ini_zmp_offset_sideward initial value for zmp_offset_sideward
 * @param ini_l_hip_roll_compensation initial value for l_hip_roll_compensation
 * @param ini_r_hip_roll_compensation initial value for r_hip_roll_compensation
 * @param ini_hip_height initial value for hip_height
 * @param ini_torso_sideward_orientation initial value for torso_sideward_orientation
 */
HumanoidMotionInterface::SetWalkParamsMessage::SetWalkParamsMessage(const float ini_max_step_length, const float ini_max_step_height, const float ini_max_step_side, const float ini_max_step_turn, const float ini_zmp_offset_forward, const float ini_zmp_offset_sideward, const float ini_l_hip_roll_compensation, const float ini_r_hip_roll_compensation, const float ini_hip_height, const float ini_torso_sideward_orientation) : Message("SetWalkParamsMessage")
{
  data_size = sizeof(SetWalkParamsMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (SetWalkParamsMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  data->max_step_length = ini_max_step_length;
  data->max_step_height = ini_max_step_height;
  data->max_step_side = ini_max_step_side;
  data->max_step_turn = ini_max_step_turn;
  data->zmp_offset_forward = ini_zmp_offset_forward;
  data->zmp_offset_sideward = ini_zmp_offset_sideward;
  data->l_hip_roll_compensation = ini_l_hip_roll_compensation;
  data->r_hip_roll_compensation = ini_r_hip_roll_compensation;
  data->hip_height = ini_hip_height;
  data->torso_sideward_orientation = ini_torso_sideward_orientation;
  add_fieldinfo(IFT_FLOAT, "max_step_length", 1, &data->max_step_length);
  add_fieldinfo(IFT_FLOAT, "max_step_height", 1, &data->max_step_height);
  add_fieldinfo(IFT_FLOAT, "max_step_side", 1, &data->max_step_side);
  add_fieldinfo(IFT_FLOAT, "max_step_turn", 1, &data->max_step_turn);
  add_fieldinfo(IFT_FLOAT, "zmp_offset_forward", 1, &data->zmp_offset_forward);
  add_fieldinfo(IFT_FLOAT, "zmp_offset_sideward", 1, &data->zmp_offset_sideward);
  add_fieldinfo(IFT_FLOAT, "l_hip_roll_compensation", 1, &data->l_hip_roll_compensation);
  add_fieldinfo(IFT_FLOAT, "r_hip_roll_compensation", 1, &data->r_hip_roll_compensation);
  add_fieldinfo(IFT_FLOAT, "hip_height", 1, &data->hip_height);
  add_fieldinfo(IFT_FLOAT, "torso_sideward_orientation", 1, &data->torso_sideward_orientation);
}
/** Constructor */
HumanoidMotionInterface::SetWalkParamsMessage::SetWalkParamsMessage() : Message("SetWalkParamsMessage")
{
  data_size = sizeof(SetWalkParamsMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (SetWalkParamsMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  add_fieldinfo(IFT_FLOAT, "max_step_length", 1, &data->max_step_length);
  add_fieldinfo(IFT_FLOAT, "max_step_height", 1, &data->max_step_height);
  add_fieldinfo(IFT_FLOAT, "max_step_side", 1, &data->max_step_side);
  add_fieldinfo(IFT_FLOAT, "max_step_turn", 1, &data->max_step_turn);
  add_fieldinfo(IFT_FLOAT, "zmp_offset_forward", 1, &data->zmp_offset_forward);
  add_fieldinfo(IFT_FLOAT, "zmp_offset_sideward", 1, &data->zmp_offset_sideward);
  add_fieldinfo(IFT_FLOAT, "l_hip_roll_compensation", 1, &data->l_hip_roll_compensation);
  add_fieldinfo(IFT_FLOAT, "r_hip_roll_compensation", 1, &data->r_hip_roll_compensation);
  add_fieldinfo(IFT_FLOAT, "hip_height", 1, &data->hip_height);
  add_fieldinfo(IFT_FLOAT, "torso_sideward_orientation", 1, &data->torso_sideward_orientation);
}

/** Destructor */
HumanoidMotionInterface::SetWalkParamsMessage::~SetWalkParamsMessage()
{
  free(data_ptr);
}

/** Copy constructor.
 * @param m message to copy from
 */
HumanoidMotionInterface::SetWalkParamsMessage::SetWalkParamsMessage(const SetWalkParamsMessage *m) : Message("SetWalkParamsMessage")
{
  data_size = m->data_size;
  data_ptr  = malloc(data_size);
  memcpy(data_ptr, m->data_ptr, data_size);
  data      = (SetWalkParamsMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/* Methods */
/** Get max_step_length value.
 * 
      Maximum length of a footstep in m.
    
 * @return max_step_length value
 */
float
HumanoidMotionInterface::SetWalkParamsMessage::max_step_length() const
{
  return data->max_step_length;
}

/** Get maximum length of max_step_length value.
 * @return length of max_step_length value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkParamsMessage::maxlenof_max_step_length() const
{
  return 1;
}

/** Set max_step_length value.
 * 
      Maximum length of a footstep in m.
    
 * @param new_max_step_length new max_step_length value
 */
void
HumanoidMotionInterface::SetWalkParamsMessage::set_max_step_length(const float new_max_step_length)
{
  data->max_step_length = new_max_step_length;
}

/** Get max_step_height value.
 * 
      Maxium height of a footstep cycloid in m.
    
 * @return max_step_height value
 */
float
HumanoidMotionInterface::SetWalkParamsMessage::max_step_height() const
{
  return data->max_step_height;
}

/** Get maximum length of max_step_height value.
 * @return length of max_step_height value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkParamsMessage::maxlenof_max_step_height() const
{
  return 1;
}

/** Set max_step_height value.
 * 
      Maxium height of a footstep cycloid in m.
    
 * @param new_max_step_height new max_step_height value
 */
void
HumanoidMotionInterface::SetWalkParamsMessage::set_max_step_height(const float new_max_step_height)
{
  data->max_step_height = new_max_step_height;
}

/** Get max_step_side value.
 * 
      Maximum length of side step in m.
    
 * @return max_step_side value
 */
float
HumanoidMotionInterface::SetWalkParamsMessage::max_step_side() const
{
  return data->max_step_side;
}

/** Get maximum length of max_step_side value.
 * @return length of max_step_side value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkParamsMessage::maxlenof_max_step_side() const
{
  return 1;
}

/** Set max_step_side value.
 * 
      Maximum length of side step in m.
    
 * @param new_max_step_side new max_step_side value
 */
void
HumanoidMotionInterface::SetWalkParamsMessage::set_max_step_side(const float new_max_step_side)
{
  data->max_step_side = new_max_step_side;
}

/** Get max_step_turn value.
 * 
      Maximum change around vertical axis on radians per footstep.
    
 * @return max_step_turn value
 */
float
HumanoidMotionInterface::SetWalkParamsMessage::max_step_turn() const
{
  return data->max_step_turn;
}

/** Get maximum length of max_step_turn value.
 * @return length of max_step_turn value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkParamsMessage::maxlenof_max_step_turn() const
{
  return 1;
}

/** Set max_step_turn value.
 * 
      Maximum change around vertical axis on radians per footstep.
    
 * @param new_max_step_turn new max_step_turn value
 */
void
HumanoidMotionInterface::SetWalkParamsMessage::set_max_step_turn(const float new_max_step_turn)
{
  data->max_step_turn = new_max_step_turn;
}

/** Get zmp_offset_forward value.
 * 
      Zero moment point offset in forward direction in m.
    
 * @return zmp_offset_forward value
 */
float
HumanoidMotionInterface::SetWalkParamsMessage::zmp_offset_forward() const
{
  return data->zmp_offset_forward;
}

/** Get maximum length of zmp_offset_forward value.
 * @return length of zmp_offset_forward value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkParamsMessage::maxlenof_zmp_offset_forward() const
{
  return 1;
}

/** Set zmp_offset_forward value.
 * 
      Zero moment point offset in forward direction in m.
    
 * @param new_zmp_offset_forward new zmp_offset_forward value
 */
void
HumanoidMotionInterface::SetWalkParamsMessage::set_zmp_offset_forward(const float new_zmp_offset_forward)
{
  data->zmp_offset_forward = new_zmp_offset_forward;
}

/** Get zmp_offset_sideward value.
 * 
      Zero moment point offset in sideward direction in m.
    
 * @return zmp_offset_sideward value
 */
float
HumanoidMotionInterface::SetWalkParamsMessage::zmp_offset_sideward() const
{
  return data->zmp_offset_sideward;
}

/** Get maximum length of zmp_offset_sideward value.
 * @return length of zmp_offset_sideward value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkParamsMessage::maxlenof_zmp_offset_sideward() const
{
  return 1;
}

/** Set zmp_offset_sideward value.
 * 
      Zero moment point offset in sideward direction in m.
    
 * @param new_zmp_offset_sideward new zmp_offset_sideward value
 */
void
HumanoidMotionInterface::SetWalkParamsMessage::set_zmp_offset_sideward(const float new_zmp_offset_sideward)
{
  data->zmp_offset_sideward = new_zmp_offset_sideward;
}

/** Get l_hip_roll_compensation value.
 * 
      Amplitude in degrees of backlash compensation for left hip roll.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @return l_hip_roll_compensation value
 */
float
HumanoidMotionInterface::SetWalkParamsMessage::l_hip_roll_compensation() const
{
  return data->l_hip_roll_compensation;
}

/** Get maximum length of l_hip_roll_compensation value.
 * @return length of l_hip_roll_compensation value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkParamsMessage::maxlenof_l_hip_roll_compensation() const
{
  return 1;
}

/** Set l_hip_roll_compensation value.
 * 
      Amplitude in degrees of backlash compensation for left hip roll.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @param new_l_hip_roll_compensation new l_hip_roll_compensation value
 */
void
HumanoidMotionInterface::SetWalkParamsMessage::set_l_hip_roll_compensation(const float new_l_hip_roll_compensation)
{
  data->l_hip_roll_compensation = new_l_hip_roll_compensation;
}

/** Get r_hip_roll_compensation value.
 * 
      Amplitude in degrees of backlash compensation for left hip roll.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @return r_hip_roll_compensation value
 */
float
HumanoidMotionInterface::SetWalkParamsMessage::r_hip_roll_compensation() const
{
  return data->r_hip_roll_compensation;
}

/** Get maximum length of r_hip_roll_compensation value.
 * @return length of r_hip_roll_compensation value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkParamsMessage::maxlenof_r_hip_roll_compensation() const
{
  return 1;
}

/** Set r_hip_roll_compensation value.
 * 
      Amplitude in degrees of backlash compensation for left hip roll.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @param new_r_hip_roll_compensation new r_hip_roll_compensation value
 */
void
HumanoidMotionInterface::SetWalkParamsMessage::set_r_hip_roll_compensation(const float new_r_hip_roll_compensation)
{
  data->r_hip_roll_compensation = new_r_hip_roll_compensation;
}

/** Get hip_height value.
 * 
      Height of hip during walk process.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @return hip_height value
 */
float
HumanoidMotionInterface::SetWalkParamsMessage::hip_height() const
{
  return data->hip_height;
}

/** Get maximum length of hip_height value.
 * @return length of hip_height value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkParamsMessage::maxlenof_hip_height() const
{
  return 1;
}

/** Set hip_height value.
 * 
      Height of hip during walk process.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @param new_hip_height new hip_height value
 */
void
HumanoidMotionInterface::SetWalkParamsMessage::set_hip_height(const float new_hip_height)
{
  data->hip_height = new_hip_height;
}

/** Get torso_sideward_orientation value.
 * 
      Torso orientation in degrees in sideward direction during walking.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @return torso_sideward_orientation value
 */
float
HumanoidMotionInterface::SetWalkParamsMessage::torso_sideward_orientation() const
{
  return data->torso_sideward_orientation;
}

/** Get maximum length of torso_sideward_orientation value.
 * @return length of torso_sideward_orientation value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkParamsMessage::maxlenof_torso_sideward_orientation() const
{
  return 1;
}

/** Set torso_sideward_orientation value.
 * 
      Torso orientation in degrees in sideward direction during walking.
      This is fitted to the Nao and is possibly not applicable to other robots.
    
 * @param new_torso_sideward_orientation new torso_sideward_orientation value
 */
void
HumanoidMotionInterface::SetWalkParamsMessage::set_torso_sideward_orientation(const float new_torso_sideward_orientation)
{
  data->torso_sideward_orientation = new_torso_sideward_orientation;
}

/** Clone this message.
 * Produces a message of the same type as this message and copies the
 * data to the new message.
 * @return clone of this message
 */
Message *
HumanoidMotionInterface::SetWalkParamsMessage::clone() const
{
  return new HumanoidMotionInterface::SetWalkParamsMessage(this);
}
/** @class HumanoidMotionInterface::SetWalkArmsParamsMessage <interfaces/HumanoidMotionInterface.h>
 * SetWalkArmsParamsMessage Fawkes BlackBoard Interface Message.
 * 
    
 */


/** Constructor with initial values.
 * @param ini_arms_enabled initial value for arms_enabled
 * @param ini_shoulder_pitch_median initial value for shoulder_pitch_median
 * @param ini_shoulder_pitch_amplitude initial value for shoulder_pitch_amplitude
 * @param ini_elbow_roll_median initial value for elbow_roll_median
 * @param ini_elbow_roll_amplitude initial value for elbow_roll_amplitude
 */
HumanoidMotionInterface::SetWalkArmsParamsMessage::SetWalkArmsParamsMessage(const bool ini_arms_enabled, const float ini_shoulder_pitch_median, const float ini_shoulder_pitch_amplitude, const float ini_elbow_roll_median, const float ini_elbow_roll_amplitude) : Message("SetWalkArmsParamsMessage")
{
  data_size = sizeof(SetWalkArmsParamsMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (SetWalkArmsParamsMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  data->arms_enabled = ini_arms_enabled;
  data->shoulder_pitch_median = ini_shoulder_pitch_median;
  data->shoulder_pitch_amplitude = ini_shoulder_pitch_amplitude;
  data->elbow_roll_median = ini_elbow_roll_median;
  data->elbow_roll_amplitude = ini_elbow_roll_amplitude;
  add_fieldinfo(IFT_BOOL, "arms_enabled", 1, &data->arms_enabled);
  add_fieldinfo(IFT_FLOAT, "shoulder_pitch_median", 1, &data->shoulder_pitch_median);
  add_fieldinfo(IFT_FLOAT, "shoulder_pitch_amplitude", 1, &data->shoulder_pitch_amplitude);
  add_fieldinfo(IFT_FLOAT, "elbow_roll_median", 1, &data->elbow_roll_median);
  add_fieldinfo(IFT_FLOAT, "elbow_roll_amplitude", 1, &data->elbow_roll_amplitude);
}
/** Constructor */
HumanoidMotionInterface::SetWalkArmsParamsMessage::SetWalkArmsParamsMessage() : Message("SetWalkArmsParamsMessage")
{
  data_size = sizeof(SetWalkArmsParamsMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (SetWalkArmsParamsMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  add_fieldinfo(IFT_BOOL, "arms_enabled", 1, &data->arms_enabled);
  add_fieldinfo(IFT_FLOAT, "shoulder_pitch_median", 1, &data->shoulder_pitch_median);
  add_fieldinfo(IFT_FLOAT, "shoulder_pitch_amplitude", 1, &data->shoulder_pitch_amplitude);
  add_fieldinfo(IFT_FLOAT, "elbow_roll_median", 1, &data->elbow_roll_median);
  add_fieldinfo(IFT_FLOAT, "elbow_roll_amplitude", 1, &data->elbow_roll_amplitude);
}

/** Destructor */
HumanoidMotionInterface::SetWalkArmsParamsMessage::~SetWalkArmsParamsMessage()
{
  free(data_ptr);
}

/** Copy constructor.
 * @param m message to copy from
 */
HumanoidMotionInterface::SetWalkArmsParamsMessage::SetWalkArmsParamsMessage(const SetWalkArmsParamsMessage *m) : Message("SetWalkArmsParamsMessage")
{
  data_size = m->data_size;
  data_ptr  = malloc(data_size);
  memcpy(data_ptr, m->data_ptr, data_size);
  data      = (SetWalkArmsParamsMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/* Methods */
/** Get arms_enabled value.
 * 
      If true the arms are controlled during walking for balancing.
    
 * @return arms_enabled value
 */
bool
HumanoidMotionInterface::SetWalkArmsParamsMessage::is_arms_enabled() const
{
  return data->arms_enabled;
}

/** Get maximum length of arms_enabled value.
 * @return length of arms_enabled value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkArmsParamsMessage::maxlenof_arms_enabled() const
{
  return 1;
}

/** Set arms_enabled value.
 * 
      If true the arms are controlled during walking for balancing.
    
 * @param new_arms_enabled new arms_enabled value
 */
void
HumanoidMotionInterface::SetWalkArmsParamsMessage::set_arms_enabled(const bool new_arms_enabled)
{
  data->arms_enabled = new_arms_enabled;
}

/** Get shoulder_pitch_median value.
 * 
      Median in radians of the shoulder pitch during walking.
    
 * @return shoulder_pitch_median value
 */
float
HumanoidMotionInterface::SetWalkArmsParamsMessage::shoulder_pitch_median() const
{
  return data->shoulder_pitch_median;
}

/** Get maximum length of shoulder_pitch_median value.
 * @return length of shoulder_pitch_median value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkArmsParamsMessage::maxlenof_shoulder_pitch_median() const
{
  return 1;
}

/** Set shoulder_pitch_median value.
 * 
      Median in radians of the shoulder pitch during walking.
    
 * @param new_shoulder_pitch_median new shoulder_pitch_median value
 */
void
HumanoidMotionInterface::SetWalkArmsParamsMessage::set_shoulder_pitch_median(const float new_shoulder_pitch_median)
{
  data->shoulder_pitch_median = new_shoulder_pitch_median;
}

/** Get shoulder_pitch_amplitude value.
 * 
      Amplitude of the shoulder pitch movement during walking.
    
 * @return shoulder_pitch_amplitude value
 */
float
HumanoidMotionInterface::SetWalkArmsParamsMessage::shoulder_pitch_amplitude() const
{
  return data->shoulder_pitch_amplitude;
}

/** Get maximum length of shoulder_pitch_amplitude value.
 * @return length of shoulder_pitch_amplitude value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkArmsParamsMessage::maxlenof_shoulder_pitch_amplitude() const
{
  return 1;
}

/** Set shoulder_pitch_amplitude value.
 * 
      Amplitude of the shoulder pitch movement during walking.
    
 * @param new_shoulder_pitch_amplitude new shoulder_pitch_amplitude value
 */
void
HumanoidMotionInterface::SetWalkArmsParamsMessage::set_shoulder_pitch_amplitude(const float new_shoulder_pitch_amplitude)
{
  data->shoulder_pitch_amplitude = new_shoulder_pitch_amplitude;
}

/** Get elbow_roll_median value.
 * 
      Median in radians of the elbow roll during walking.
    
 * @return elbow_roll_median value
 */
float
HumanoidMotionInterface::SetWalkArmsParamsMessage::elbow_roll_median() const
{
  return data->elbow_roll_median;
}

/** Get maximum length of elbow_roll_median value.
 * @return length of elbow_roll_median value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkArmsParamsMessage::maxlenof_elbow_roll_median() const
{
  return 1;
}

/** Set elbow_roll_median value.
 * 
      Median in radians of the elbow roll during walking.
    
 * @param new_elbow_roll_median new elbow_roll_median value
 */
void
HumanoidMotionInterface::SetWalkArmsParamsMessage::set_elbow_roll_median(const float new_elbow_roll_median)
{
  data->elbow_roll_median = new_elbow_roll_median;
}

/** Get elbow_roll_amplitude value.
 * 
      Amplitude of the elbow roll movement during walking.
    
 * @return elbow_roll_amplitude value
 */
float
HumanoidMotionInterface::SetWalkArmsParamsMessage::elbow_roll_amplitude() const
{
  return data->elbow_roll_amplitude;
}

/** Get maximum length of elbow_roll_amplitude value.
 * @return length of elbow_roll_amplitude value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetWalkArmsParamsMessage::maxlenof_elbow_roll_amplitude() const
{
  return 1;
}

/** Set elbow_roll_amplitude value.
 * 
      Amplitude of the elbow roll movement during walking.
    
 * @param new_elbow_roll_amplitude new elbow_roll_amplitude value
 */
void
HumanoidMotionInterface::SetWalkArmsParamsMessage::set_elbow_roll_amplitude(const float new_elbow_roll_amplitude)
{
  data->elbow_roll_amplitude = new_elbow_roll_amplitude;
}

/** Clone this message.
 * Produces a message of the same type as this message and copies the
 * data to the new message.
 * @return clone of this message
 */
Message *
HumanoidMotionInterface::SetWalkArmsParamsMessage::clone() const
{
  return new HumanoidMotionInterface::SetWalkArmsParamsMessage(this);
}
/** @class HumanoidMotionInterface::StopMessage <interfaces/HumanoidMotionInterface.h>
 * StopMessage Fawkes BlackBoard Interface Message.
 * 
    
 */


/** Constructor */
HumanoidMotionInterface::StopMessage::StopMessage() : Message("StopMessage")
{
  data_size = sizeof(StopMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (StopMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/** Destructor */
HumanoidMotionInterface::StopMessage::~StopMessage()
{
  free(data_ptr);
}

/** Copy constructor.
 * @param m message to copy from
 */
HumanoidMotionInterface::StopMessage::StopMessage(const StopMessage *m) : Message("StopMessage")
{
  data_size = m->data_size;
  data_ptr  = malloc(data_size);
  memcpy(data_ptr, m->data_ptr, data_size);
  data      = (StopMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/* Methods */
/** Clone this message.
 * Produces a message of the same type as this message and copies the
 * data to the new message.
 * @return clone of this message
 */
Message *
HumanoidMotionInterface::StopMessage::clone() const
{
  return new HumanoidMotionInterface::StopMessage(this);
}
/** @class HumanoidMotionInterface::WalkStraightMessage <interfaces/HumanoidMotionInterface.h>
 * WalkStraightMessage Fawkes BlackBoard Interface Message.
 * 
    
 */


/** Constructor with initial values.
 * @param ini_distance initial value for distance
 * @param ini_num_samples initial value for num_samples
 */
HumanoidMotionInterface::WalkStraightMessage::WalkStraightMessage(const float ini_distance, const uint32_t ini_num_samples) : Message("WalkStraightMessage")
{
  data_size = sizeof(WalkStraightMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (WalkStraightMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  data->distance = ini_distance;
  data->num_samples = ini_num_samples;
  add_fieldinfo(IFT_FLOAT, "distance", 1, &data->distance);
  add_fieldinfo(IFT_UINT32, "num_samples", 1, &data->num_samples);
}
/** Constructor */
HumanoidMotionInterface::WalkStraightMessage::WalkStraightMessage() : Message("WalkStraightMessage")
{
  data_size = sizeof(WalkStraightMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (WalkStraightMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  add_fieldinfo(IFT_FLOAT, "distance", 1, &data->distance);
  add_fieldinfo(IFT_UINT32, "num_samples", 1, &data->num_samples);
}

/** Destructor */
HumanoidMotionInterface::WalkStraightMessage::~WalkStraightMessage()
{
  free(data_ptr);
}

/** Copy constructor.
 * @param m message to copy from
 */
HumanoidMotionInterface::WalkStraightMessage::WalkStraightMessage(const WalkStraightMessage *m) : Message("WalkStraightMessage")
{
  data_size = m->data_size;
  data_ptr  = malloc(data_size);
  memcpy(data_ptr, m->data_ptr, data_size);
  data      = (WalkStraightMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/* Methods */
/** Get distance value.
 * Distance in m to walk.
 * @return distance value
 */
float
HumanoidMotionInterface::WalkStraightMessage::distance() const
{
  return data->distance;
}

/** Get maximum length of distance value.
 * @return length of distance value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::WalkStraightMessage::maxlenof_distance() const
{
  return 1;
}

/** Set distance value.
 * Distance in m to walk.
 * @param new_distance new distance value
 */
void
HumanoidMotionInterface::WalkStraightMessage::set_distance(const float new_distance)
{
  data->distance = new_distance;
}

/** Get num_samples value.
 * 
      Number of intermediate samples to use for walking.
    
 * @return num_samples value
 */
uint32_t
HumanoidMotionInterface::WalkStraightMessage::num_samples() const
{
  return data->num_samples;
}

/** Get maximum length of num_samples value.
 * @return length of num_samples value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::WalkStraightMessage::maxlenof_num_samples() const
{
  return 1;
}

/** Set num_samples value.
 * 
      Number of intermediate samples to use for walking.
    
 * @param new_num_samples new num_samples value
 */
void
HumanoidMotionInterface::WalkStraightMessage::set_num_samples(const uint32_t new_num_samples)
{
  data->num_samples = new_num_samples;
}

/** Clone this message.
 * Produces a message of the same type as this message and copies the
 * data to the new message.
 * @return clone of this message
 */
Message *
HumanoidMotionInterface::WalkStraightMessage::clone() const
{
  return new HumanoidMotionInterface::WalkStraightMessage(this);
}
/** @class HumanoidMotionInterface::WalkSidewaysMessage <interfaces/HumanoidMotionInterface.h>
 * WalkSidewaysMessage Fawkes BlackBoard Interface Message.
 * 
    
 */


/** Constructor with initial values.
 * @param ini_distance initial value for distance
 * @param ini_num_samples initial value for num_samples
 */
HumanoidMotionInterface::WalkSidewaysMessage::WalkSidewaysMessage(const float ini_distance, const uint32_t ini_num_samples) : Message("WalkSidewaysMessage")
{
  data_size = sizeof(WalkSidewaysMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (WalkSidewaysMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  data->distance = ini_distance;
  data->num_samples = ini_num_samples;
  add_fieldinfo(IFT_FLOAT, "distance", 1, &data->distance);
  add_fieldinfo(IFT_UINT32, "num_samples", 1, &data->num_samples);
}
/** Constructor */
HumanoidMotionInterface::WalkSidewaysMessage::WalkSidewaysMessage() : Message("WalkSidewaysMessage")
{
  data_size = sizeof(WalkSidewaysMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (WalkSidewaysMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  add_fieldinfo(IFT_FLOAT, "distance", 1, &data->distance);
  add_fieldinfo(IFT_UINT32, "num_samples", 1, &data->num_samples);
}

/** Destructor */
HumanoidMotionInterface::WalkSidewaysMessage::~WalkSidewaysMessage()
{
  free(data_ptr);
}

/** Copy constructor.
 * @param m message to copy from
 */
HumanoidMotionInterface::WalkSidewaysMessage::WalkSidewaysMessage(const WalkSidewaysMessage *m) : Message("WalkSidewaysMessage")
{
  data_size = m->data_size;
  data_ptr  = malloc(data_size);
  memcpy(data_ptr, m->data_ptr, data_size);
  data      = (WalkSidewaysMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/* Methods */
/** Get distance value.
 * Distance in m to walk.
 * @return distance value
 */
float
HumanoidMotionInterface::WalkSidewaysMessage::distance() const
{
  return data->distance;
}

/** Get maximum length of distance value.
 * @return length of distance value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::WalkSidewaysMessage::maxlenof_distance() const
{
  return 1;
}

/** Set distance value.
 * Distance in m to walk.
 * @param new_distance new distance value
 */
void
HumanoidMotionInterface::WalkSidewaysMessage::set_distance(const float new_distance)
{
  data->distance = new_distance;
}

/** Get num_samples value.
 * 
      Number of intermediate samples to use for strafing.
    
 * @return num_samples value
 */
uint32_t
HumanoidMotionInterface::WalkSidewaysMessage::num_samples() const
{
  return data->num_samples;
}

/** Get maximum length of num_samples value.
 * @return length of num_samples value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::WalkSidewaysMessage::maxlenof_num_samples() const
{
  return 1;
}

/** Set num_samples value.
 * 
      Number of intermediate samples to use for strafing.
    
 * @param new_num_samples new num_samples value
 */
void
HumanoidMotionInterface::WalkSidewaysMessage::set_num_samples(const uint32_t new_num_samples)
{
  data->num_samples = new_num_samples;
}

/** Clone this message.
 * Produces a message of the same type as this message and copies the
 * data to the new message.
 * @return clone of this message
 */
Message *
HumanoidMotionInterface::WalkSidewaysMessage::clone() const
{
  return new HumanoidMotionInterface::WalkSidewaysMessage(this);
}
/** @class HumanoidMotionInterface::WalkArcMessage <interfaces/HumanoidMotionInterface.h>
 * WalkArcMessage Fawkes BlackBoard Interface Message.
 * 
    
 */


/** Constructor with initial values.
 * @param ini_angle initial value for angle
 * @param ini_radius initial value for radius
 * @param ini_num_samples initial value for num_samples
 */
HumanoidMotionInterface::WalkArcMessage::WalkArcMessage(const float ini_angle, const float ini_radius, const uint32_t ini_num_samples) : Message("WalkArcMessage")
{
  data_size = sizeof(WalkArcMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (WalkArcMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  data->angle = ini_angle;
  data->radius = ini_radius;
  data->num_samples = ini_num_samples;
  add_fieldinfo(IFT_FLOAT, "angle", 1, &data->angle);
  add_fieldinfo(IFT_FLOAT, "radius", 1, &data->radius);
  add_fieldinfo(IFT_UINT32, "num_samples", 1, &data->num_samples);
}
/** Constructor */
HumanoidMotionInterface::WalkArcMessage::WalkArcMessage() : Message("WalkArcMessage")
{
  data_size = sizeof(WalkArcMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (WalkArcMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  add_fieldinfo(IFT_FLOAT, "angle", 1, &data->angle);
  add_fieldinfo(IFT_FLOAT, "radius", 1, &data->radius);
  add_fieldinfo(IFT_UINT32, "num_samples", 1, &data->num_samples);
}

/** Destructor */
HumanoidMotionInterface::WalkArcMessage::~WalkArcMessage()
{
  free(data_ptr);
}

/** Copy constructor.
 * @param m message to copy from
 */
HumanoidMotionInterface::WalkArcMessage::WalkArcMessage(const WalkArcMessage *m) : Message("WalkArcMessage")
{
  data_size = m->data_size;
  data_ptr  = malloc(data_size);
  memcpy(data_ptr, m->data_ptr, data_size);
  data      = (WalkArcMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/* Methods */
/** Get angle value.
 * Angle in radians to turn over the way.
 * @return angle value
 */
float
HumanoidMotionInterface::WalkArcMessage::angle() const
{
  return data->angle;
}

/** Get maximum length of angle value.
 * @return length of angle value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::WalkArcMessage::maxlenof_angle() const
{
  return 1;
}

/** Set angle value.
 * Angle in radians to turn over the way.
 * @param new_angle new angle value
 */
void
HumanoidMotionInterface::WalkArcMessage::set_angle(const float new_angle)
{
  data->angle = new_angle;
}

/** Get radius value.
 * Radius in m of the circle in m.
 * @return radius value
 */
float
HumanoidMotionInterface::WalkArcMessage::radius() const
{
  return data->radius;
}

/** Get maximum length of radius value.
 * @return length of radius value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::WalkArcMessage::maxlenof_radius() const
{
  return 1;
}

/** Set radius value.
 * Radius in m of the circle in m.
 * @param new_radius new radius value
 */
void
HumanoidMotionInterface::WalkArcMessage::set_radius(const float new_radius)
{
  data->radius = new_radius;
}

/** Get num_samples value.
 * 
      Number of intermediate samples to use for walking.
    
 * @return num_samples value
 */
uint32_t
HumanoidMotionInterface::WalkArcMessage::num_samples() const
{
  return data->num_samples;
}

/** Get maximum length of num_samples value.
 * @return length of num_samples value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::WalkArcMessage::maxlenof_num_samples() const
{
  return 1;
}

/** Set num_samples value.
 * 
      Number of intermediate samples to use for walking.
    
 * @param new_num_samples new num_samples value
 */
void
HumanoidMotionInterface::WalkArcMessage::set_num_samples(const uint32_t new_num_samples)
{
  data->num_samples = new_num_samples;
}

/** Clone this message.
 * Produces a message of the same type as this message and copies the
 * data to the new message.
 * @return clone of this message
 */
Message *
HumanoidMotionInterface::WalkArcMessage::clone() const
{
  return new HumanoidMotionInterface::WalkArcMessage(this);
}
/** @class HumanoidMotionInterface::WalkMessage <interfaces/HumanoidMotionInterface.h>
 * WalkMessage Fawkes BlackBoard Interface Message.
 * 
    
 */


/** Constructor with initial values.
 * @param ini_x initial value for x
 * @param ini_y initial value for y
 * @param ini_theta initial value for theta
 * @param ini_speed initial value for speed
 */
HumanoidMotionInterface::WalkMessage::WalkMessage(const float ini_x, const float ini_y, const float ini_theta, const float ini_speed) : Message("WalkMessage")
{
  data_size = sizeof(WalkMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (WalkMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  data->x = ini_x;
  data->y = ini_y;
  data->theta = ini_theta;
  data->speed = ini_speed;
  add_fieldinfo(IFT_FLOAT, "x", 1, &data->x);
  add_fieldinfo(IFT_FLOAT, "y", 1, &data->y);
  add_fieldinfo(IFT_FLOAT, "theta", 1, &data->theta);
  add_fieldinfo(IFT_FLOAT, "speed", 1, &data->speed);
}
/** Constructor */
HumanoidMotionInterface::WalkMessage::WalkMessage() : Message("WalkMessage")
{
  data_size = sizeof(WalkMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (WalkMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  add_fieldinfo(IFT_FLOAT, "x", 1, &data->x);
  add_fieldinfo(IFT_FLOAT, "y", 1, &data->y);
  add_fieldinfo(IFT_FLOAT, "theta", 1, &data->theta);
  add_fieldinfo(IFT_FLOAT, "speed", 1, &data->speed);
}

/** Destructor */
HumanoidMotionInterface::WalkMessage::~WalkMessage()
{
  free(data_ptr);
}

/** Copy constructor.
 * @param m message to copy from
 */
HumanoidMotionInterface::WalkMessage::WalkMessage(const WalkMessage *m) : Message("WalkMessage")
{
  data_size = m->data_size;
  data_ptr  = malloc(data_size);
  memcpy(data_ptr, m->data_ptr, data_size);
  data      = (WalkMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/* Methods */
/** Get x value.
 * Fraction of MaxStepX. Use negative for backwards. [-1.0 to 1.0]
 * @return x value
 */
float
HumanoidMotionInterface::WalkMessage::x() const
{
  return data->x;
}

/** Get maximum length of x value.
 * @return length of x value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::WalkMessage::maxlenof_x() const
{
  return 1;
}

/** Set x value.
 * Fraction of MaxStepX. Use negative for backwards. [-1.0 to 1.0]
 * @param new_x new x value
 */
void
HumanoidMotionInterface::WalkMessage::set_x(const float new_x)
{
  data->x = new_x;
}

/** Get y value.
 * Fraction of MaxStepY. Use negative for right. [-1.0 to 1.0]
 * @return y value
 */
float
HumanoidMotionInterface::WalkMessage::y() const
{
  return data->y;
}

/** Get maximum length of y value.
 * @return length of y value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::WalkMessage::maxlenof_y() const
{
  return 1;
}

/** Set y value.
 * Fraction of MaxStepY. Use negative for right. [-1.0 to 1.0]
 * @param new_y new y value
 */
void
HumanoidMotionInterface::WalkMessage::set_y(const float new_y)
{
  data->y = new_y;
}

/** Get theta value.
 * Fraction of MaxStepTheta. Use negative for clockwise [-1.0 to 1.0]
 * @return theta value
 */
float
HumanoidMotionInterface::WalkMessage::theta() const
{
  return data->theta;
}

/** Get maximum length of theta value.
 * @return length of theta value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::WalkMessage::maxlenof_theta() const
{
  return 1;
}

/** Set theta value.
 * Fraction of MaxStepTheta. Use negative for clockwise [-1.0 to 1.0]
 * @param new_theta new theta value
 */
void
HumanoidMotionInterface::WalkMessage::set_theta(const float new_theta)
{
  data->theta = new_theta;
}

/** Get speed value.
 * Fraction of MaxStepFrequency [0.0 to 1.0]
 * @return speed value
 */
float
HumanoidMotionInterface::WalkMessage::speed() const
{
  return data->speed;
}

/** Get maximum length of speed value.
 * @return length of speed value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::WalkMessage::maxlenof_speed() const
{
  return 1;
}

/** Set speed value.
 * Fraction of MaxStepFrequency [0.0 to 1.0]
 * @param new_speed new speed value
 */
void
HumanoidMotionInterface::WalkMessage::set_speed(const float new_speed)
{
  data->speed = new_speed;
}

/** Clone this message.
 * Produces a message of the same type as this message and copies the
 * data to the new message.
 * @return clone of this message
 */
Message *
HumanoidMotionInterface::WalkMessage::clone() const
{
  return new HumanoidMotionInterface::WalkMessage(this);
}
/** @class HumanoidMotionInterface::TurnMessage <interfaces/HumanoidMotionInterface.h>
 * TurnMessage Fawkes BlackBoard Interface Message.
 * 
    
 */


/** Constructor with initial values.
 * @param ini_angle initial value for angle
 * @param ini_num_samples initial value for num_samples
 */
HumanoidMotionInterface::TurnMessage::TurnMessage(const float ini_angle, const uint32_t ini_num_samples) : Message("TurnMessage")
{
  data_size = sizeof(TurnMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (TurnMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  data->angle = ini_angle;
  data->num_samples = ini_num_samples;
  add_fieldinfo(IFT_FLOAT, "angle", 1, &data->angle);
  add_fieldinfo(IFT_UINT32, "num_samples", 1, &data->num_samples);
}
/** Constructor */
HumanoidMotionInterface::TurnMessage::TurnMessage() : Message("TurnMessage")
{
  data_size = sizeof(TurnMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (TurnMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  add_fieldinfo(IFT_FLOAT, "angle", 1, &data->angle);
  add_fieldinfo(IFT_UINT32, "num_samples", 1, &data->num_samples);
}

/** Destructor */
HumanoidMotionInterface::TurnMessage::~TurnMessage()
{
  free(data_ptr);
}

/** Copy constructor.
 * @param m message to copy from
 */
HumanoidMotionInterface::TurnMessage::TurnMessage(const TurnMessage *m) : Message("TurnMessage")
{
  data_size = m->data_size;
  data_ptr  = malloc(data_size);
  memcpy(data_ptr, m->data_ptr, data_size);
  data      = (TurnMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/* Methods */
/** Get angle value.
 * Angle in radians to turn.
 * @return angle value
 */
float
HumanoidMotionInterface::TurnMessage::angle() const
{
  return data->angle;
}

/** Get maximum length of angle value.
 * @return length of angle value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::TurnMessage::maxlenof_angle() const
{
  return 1;
}

/** Set angle value.
 * Angle in radians to turn.
 * @param new_angle new angle value
 */
void
HumanoidMotionInterface::TurnMessage::set_angle(const float new_angle)
{
  data->angle = new_angle;
}

/** Get num_samples value.
 * 
      Number of intermediate samples to use for turning.
    
 * @return num_samples value
 */
uint32_t
HumanoidMotionInterface::TurnMessage::num_samples() const
{
  return data->num_samples;
}

/** Get maximum length of num_samples value.
 * @return length of num_samples value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::TurnMessage::maxlenof_num_samples() const
{
  return 1;
}

/** Set num_samples value.
 * 
      Number of intermediate samples to use for turning.
    
 * @param new_num_samples new num_samples value
 */
void
HumanoidMotionInterface::TurnMessage::set_num_samples(const uint32_t new_num_samples)
{
  data->num_samples = new_num_samples;
}

/** Clone this message.
 * Produces a message of the same type as this message and copies the
 * data to the new message.
 * @return clone of this message
 */
Message *
HumanoidMotionInterface::TurnMessage::clone() const
{
  return new HumanoidMotionInterface::TurnMessage(this);
}
/** @class HumanoidMotionInterface::KickMessage <interfaces/HumanoidMotionInterface.h>
 * KickMessage Fawkes BlackBoard Interface Message.
 * 
    
 */


/** Constructor with initial values.
 * @param ini_leg initial value for leg
 * @param ini_strength initial value for strength
 */
HumanoidMotionInterface::KickMessage::KickMessage(const LegEnum ini_leg, const float ini_strength) : Message("KickMessage")
{
  data_size = sizeof(KickMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (KickMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  data->leg = ini_leg;
  data->strength = ini_strength;
  add_fieldinfo(IFT_ENUM, "leg", 1, &data->leg, "LegEnum");
  add_fieldinfo(IFT_FLOAT, "strength", 1, &data->strength);
}
/** Constructor */
HumanoidMotionInterface::KickMessage::KickMessage() : Message("KickMessage")
{
  data_size = sizeof(KickMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (KickMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  add_fieldinfo(IFT_ENUM, "leg", 1, &data->leg, "LegEnum");
  add_fieldinfo(IFT_FLOAT, "strength", 1, &data->strength);
}

/** Destructor */
HumanoidMotionInterface::KickMessage::~KickMessage()
{
  free(data_ptr);
}

/** Copy constructor.
 * @param m message to copy from
 */
HumanoidMotionInterface::KickMessage::KickMessage(const KickMessage *m) : Message("KickMessage")
{
  data_size = m->data_size;
  data_ptr  = malloc(data_size);
  memcpy(data_ptr, m->data_ptr, data_size);
  data      = (KickMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/* Methods */
/** Get leg value.
 * Leg to kick with
 * @return leg value
 */
HumanoidMotionInterface::LegEnum
HumanoidMotionInterface::KickMessage::leg() const
{
  return data->leg;
}

/** Get maximum length of leg value.
 * @return length of leg value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::KickMessage::maxlenof_leg() const
{
  return 1;
}

/** Set leg value.
 * Leg to kick with
 * @param new_leg new leg value
 */
void
HumanoidMotionInterface::KickMessage::set_leg(const LegEnum new_leg)
{
  data->leg = new_leg;
}

/** Get strength value.
 * Kick strength
 * @return strength value
 */
float
HumanoidMotionInterface::KickMessage::strength() const
{
  return data->strength;
}

/** Get maximum length of strength value.
 * @return length of strength value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::KickMessage::maxlenof_strength() const
{
  return 1;
}

/** Set strength value.
 * Kick strength
 * @param new_strength new strength value
 */
void
HumanoidMotionInterface::KickMessage::set_strength(const float new_strength)
{
  data->strength = new_strength;
}

/** Clone this message.
 * Produces a message of the same type as this message and copies the
 * data to the new message.
 * @return clone of this message
 */
Message *
HumanoidMotionInterface::KickMessage::clone() const
{
  return new HumanoidMotionInterface::KickMessage(this);
}
/** @class HumanoidMotionInterface::ParkMessage <interfaces/HumanoidMotionInterface.h>
 * ParkMessage Fawkes BlackBoard Interface Message.
 * 
    
 */


/** Constructor with initial values.
 * @param ini_time_sec initial value for time_sec
 */
HumanoidMotionInterface::ParkMessage::ParkMessage(const float ini_time_sec) : Message("ParkMessage")
{
  data_size = sizeof(ParkMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (ParkMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  data->time_sec = ini_time_sec;
  add_fieldinfo(IFT_FLOAT, "time_sec", 1, &data->time_sec);
}
/** Constructor */
HumanoidMotionInterface::ParkMessage::ParkMessage() : Message("ParkMessage")
{
  data_size = sizeof(ParkMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (ParkMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  add_fieldinfo(IFT_FLOAT, "time_sec", 1, &data->time_sec);
}

/** Destructor */
HumanoidMotionInterface::ParkMessage::~ParkMessage()
{
  free(data_ptr);
}

/** Copy constructor.
 * @param m message to copy from
 */
HumanoidMotionInterface::ParkMessage::ParkMessage(const ParkMessage *m) : Message("ParkMessage")
{
  data_size = m->data_size;
  data_ptr  = malloc(data_size);
  memcpy(data_ptr, m->data_ptr, data_size);
  data      = (ParkMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/* Methods */
/** Get time_sec value.
 * Time in seconds when to reach the position.
 * @return time_sec value
 */
float
HumanoidMotionInterface::ParkMessage::time_sec() const
{
  return data->time_sec;
}

/** Get maximum length of time_sec value.
 * @return length of time_sec value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::ParkMessage::maxlenof_time_sec() const
{
  return 1;
}

/** Set time_sec value.
 * Time in seconds when to reach the position.
 * @param new_time_sec new time_sec value
 */
void
HumanoidMotionInterface::ParkMessage::set_time_sec(const float new_time_sec)
{
  data->time_sec = new_time_sec;
}

/** Clone this message.
 * Produces a message of the same type as this message and copies the
 * data to the new message.
 * @return clone of this message
 */
Message *
HumanoidMotionInterface::ParkMessage::clone() const
{
  return new HumanoidMotionInterface::ParkMessage(this);
}
/** @class HumanoidMotionInterface::GetUpMessage <interfaces/HumanoidMotionInterface.h>
 * GetUpMessage Fawkes BlackBoard Interface Message.
 * 
    
 */


/** Constructor with initial values.
 * @param ini_time_sec initial value for time_sec
 */
HumanoidMotionInterface::GetUpMessage::GetUpMessage(const float ini_time_sec) : Message("GetUpMessage")
{
  data_size = sizeof(GetUpMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (GetUpMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  data->time_sec = ini_time_sec;
  add_fieldinfo(IFT_FLOAT, "time_sec", 1, &data->time_sec);
}
/** Constructor */
HumanoidMotionInterface::GetUpMessage::GetUpMessage() : Message("GetUpMessage")
{
  data_size = sizeof(GetUpMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (GetUpMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  add_fieldinfo(IFT_FLOAT, "time_sec", 1, &data->time_sec);
}

/** Destructor */
HumanoidMotionInterface::GetUpMessage::~GetUpMessage()
{
  free(data_ptr);
}

/** Copy constructor.
 * @param m message to copy from
 */
HumanoidMotionInterface::GetUpMessage::GetUpMessage(const GetUpMessage *m) : Message("GetUpMessage")
{
  data_size = m->data_size;
  data_ptr  = malloc(data_size);
  memcpy(data_ptr, m->data_ptr, data_size);
  data      = (GetUpMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/* Methods */
/** Get time_sec value.
 * Time in seconds when to reach the position.
 * @return time_sec value
 */
float
HumanoidMotionInterface::GetUpMessage::time_sec() const
{
  return data->time_sec;
}

/** Get maximum length of time_sec value.
 * @return length of time_sec value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::GetUpMessage::maxlenof_time_sec() const
{
  return 1;
}

/** Set time_sec value.
 * Time in seconds when to reach the position.
 * @param new_time_sec new time_sec value
 */
void
HumanoidMotionInterface::GetUpMessage::set_time_sec(const float new_time_sec)
{
  data->time_sec = new_time_sec;
}

/** Clone this message.
 * Produces a message of the same type as this message and copies the
 * data to the new message.
 * @return clone of this message
 */
Message *
HumanoidMotionInterface::GetUpMessage::clone() const
{
  return new HumanoidMotionInterface::GetUpMessage(this);
}
/** @class HumanoidMotionInterface::StandupMessage <interfaces/HumanoidMotionInterface.h>
 * StandupMessage Fawkes BlackBoard Interface Message.
 * 
    
 */


/** Constructor with initial values.
 * @param ini_from_pos initial value for from_pos
 */
HumanoidMotionInterface::StandupMessage::StandupMessage(const StandupEnum ini_from_pos) : Message("StandupMessage")
{
  data_size = sizeof(StandupMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (StandupMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  data->from_pos = ini_from_pos;
  add_fieldinfo(IFT_ENUM, "from_pos", 1, &data->from_pos, "StandupEnum");
}
/** Constructor */
HumanoidMotionInterface::StandupMessage::StandupMessage() : Message("StandupMessage")
{
  data_size = sizeof(StandupMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (StandupMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  add_fieldinfo(IFT_ENUM, "from_pos", 1, &data->from_pos, "StandupEnum");
}

/** Destructor */
HumanoidMotionInterface::StandupMessage::~StandupMessage()
{
  free(data_ptr);
}

/** Copy constructor.
 * @param m message to copy from
 */
HumanoidMotionInterface::StandupMessage::StandupMessage(const StandupMessage *m) : Message("StandupMessage")
{
  data_size = m->data_size;
  data_ptr  = malloc(data_size);
  memcpy(data_ptr, m->data_ptr, data_size);
  data      = (StandupMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/* Methods */
/** Get from_pos value.
 * Position from where to standup.
 * @return from_pos value
 */
HumanoidMotionInterface::StandupEnum
HumanoidMotionInterface::StandupMessage::from_pos() const
{
  return data->from_pos;
}

/** Get maximum length of from_pos value.
 * @return length of from_pos value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::StandupMessage::maxlenof_from_pos() const
{
  return 1;
}

/** Set from_pos value.
 * Position from where to standup.
 * @param new_from_pos new from_pos value
 */
void
HumanoidMotionInterface::StandupMessage::set_from_pos(const StandupEnum new_from_pos)
{
  data->from_pos = new_from_pos;
}

/** Clone this message.
 * Produces a message of the same type as this message and copies the
 * data to the new message.
 * @return clone of this message
 */
Message *
HumanoidMotionInterface::StandupMessage::clone() const
{
  return new HumanoidMotionInterface::StandupMessage(this);
}
/** @class HumanoidMotionInterface::YawPitchHeadMessage <interfaces/HumanoidMotionInterface.h>
 * YawPitchHeadMessage Fawkes BlackBoard Interface Message.
 * 
    
 */


/** Constructor with initial values.
 * @param ini_yaw initial value for yaw
 * @param ini_pitch initial value for pitch
 * @param ini_time_sec initial value for time_sec
 */
HumanoidMotionInterface::YawPitchHeadMessage::YawPitchHeadMessage(const float ini_yaw, const float ini_pitch, const float ini_time_sec) : Message("YawPitchHeadMessage")
{
  data_size = sizeof(YawPitchHeadMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (YawPitchHeadMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  data->yaw = ini_yaw;
  data->pitch = ini_pitch;
  data->time_sec = ini_time_sec;
  add_fieldinfo(IFT_FLOAT, "yaw", 1, &data->yaw);
  add_fieldinfo(IFT_FLOAT, "pitch", 1, &data->pitch);
  add_fieldinfo(IFT_FLOAT, "time_sec", 1, &data->time_sec);
}
/** Constructor */
HumanoidMotionInterface::YawPitchHeadMessage::YawPitchHeadMessage() : Message("YawPitchHeadMessage")
{
  data_size = sizeof(YawPitchHeadMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (YawPitchHeadMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  add_fieldinfo(IFT_FLOAT, "yaw", 1, &data->yaw);
  add_fieldinfo(IFT_FLOAT, "pitch", 1, &data->pitch);
  add_fieldinfo(IFT_FLOAT, "time_sec", 1, &data->time_sec);
}

/** Destructor */
HumanoidMotionInterface::YawPitchHeadMessage::~YawPitchHeadMessage()
{
  free(data_ptr);
}

/** Copy constructor.
 * @param m message to copy from
 */
HumanoidMotionInterface::YawPitchHeadMessage::YawPitchHeadMessage(const YawPitchHeadMessage *m) : Message("YawPitchHeadMessage")
{
  data_size = m->data_size;
  data_ptr  = malloc(data_size);
  memcpy(data_ptr, m->data_ptr, data_size);
  data      = (YawPitchHeadMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/* Methods */
/** Get yaw value.
 * Desired yaw (horizontal orientation).
 * @return yaw value
 */
float
HumanoidMotionInterface::YawPitchHeadMessage::yaw() const
{
  return data->yaw;
}

/** Get maximum length of yaw value.
 * @return length of yaw value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::YawPitchHeadMessage::maxlenof_yaw() const
{
  return 1;
}

/** Set yaw value.
 * Desired yaw (horizontal orientation).
 * @param new_yaw new yaw value
 */
void
HumanoidMotionInterface::YawPitchHeadMessage::set_yaw(const float new_yaw)
{
  data->yaw = new_yaw;
}

/** Get pitch value.
 * Desired pitch (vertical orientation).
 * @return pitch value
 */
float
HumanoidMotionInterface::YawPitchHeadMessage::pitch() const
{
  return data->pitch;
}

/** Get maximum length of pitch value.
 * @return length of pitch value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::YawPitchHeadMessage::maxlenof_pitch() const
{
  return 1;
}

/** Set pitch value.
 * Desired pitch (vertical orientation).
 * @param new_pitch new pitch value
 */
void
HumanoidMotionInterface::YawPitchHeadMessage::set_pitch(const float new_pitch)
{
  data->pitch = new_pitch;
}

/** Get time_sec value.
 * Time in seconds when to reach the target.
 * @return time_sec value
 */
float
HumanoidMotionInterface::YawPitchHeadMessage::time_sec() const
{
  return data->time_sec;
}

/** Get maximum length of time_sec value.
 * @return length of time_sec value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::YawPitchHeadMessage::maxlenof_time_sec() const
{
  return 1;
}

/** Set time_sec value.
 * Time in seconds when to reach the target.
 * @param new_time_sec new time_sec value
 */
void
HumanoidMotionInterface::YawPitchHeadMessage::set_time_sec(const float new_time_sec)
{
  data->time_sec = new_time_sec;
}

/** Clone this message.
 * Produces a message of the same type as this message and copies the
 * data to the new message.
 * @return clone of this message
 */
Message *
HumanoidMotionInterface::YawPitchHeadMessage::clone() const
{
  return new HumanoidMotionInterface::YawPitchHeadMessage(this);
}
/** @class HumanoidMotionInterface::SetStiffnessParamsMessage <interfaces/HumanoidMotionInterface.h>
 * SetStiffnessParamsMessage Fawkes BlackBoard Interface Message.
 * 
    
 */


/** Constructor with initial values.
 * @param ini_motion_pattern initial value for motion_pattern
 * @param ini_head_yaw initial value for head_yaw
 * @param ini_head_pitch initial value for head_pitch
 * @param ini_l_shoulder_pitch initial value for l_shoulder_pitch
 * @param ini_l_shoulder_roll initial value for l_shoulder_roll
 * @param ini_l_elbow_yaw initial value for l_elbow_yaw
 * @param ini_l_elbow_roll initial value for l_elbow_roll
 * @param ini_l_hip_yaw_pitch initial value for l_hip_yaw_pitch
 * @param ini_l_hip_roll initial value for l_hip_roll
 * @param ini_l_hip_pitch initial value for l_hip_pitch
 * @param ini_l_knee_pitch initial value for l_knee_pitch
 * @param ini_l_ankle_pitch initial value for l_ankle_pitch
 * @param ini_l_ankle_roll initial value for l_ankle_roll
 * @param ini_r_hip_yaw_pitch initial value for r_hip_yaw_pitch
 * @param ini_r_hip_roll initial value for r_hip_roll
 * @param ini_r_hip_pitch initial value for r_hip_pitch
 * @param ini_r_knee_pitch initial value for r_knee_pitch
 * @param ini_r_ankle_pitch initial value for r_ankle_pitch
 * @param ini_r_ankle_roll initial value for r_ankle_roll
 * @param ini_r_shoulder_pitch initial value for r_shoulder_pitch
 * @param ini_r_shoulder_roll initial value for r_shoulder_roll
 * @param ini_r_elbow_yaw initial value for r_elbow_yaw
 * @param ini_r_elbow_roll initial value for r_elbow_roll
 */
HumanoidMotionInterface::SetStiffnessParamsMessage::SetStiffnessParamsMessage(const StiffnessMotionPatternEnum ini_motion_pattern, const float ini_head_yaw, const float ini_head_pitch, const float ini_l_shoulder_pitch, const float ini_l_shoulder_roll, const float ini_l_elbow_yaw, const float ini_l_elbow_roll, const float ini_l_hip_yaw_pitch, const float ini_l_hip_roll, const float ini_l_hip_pitch, const float ini_l_knee_pitch, const float ini_l_ankle_pitch, const float ini_l_ankle_roll, const float ini_r_hip_yaw_pitch, const float ini_r_hip_roll, const float ini_r_hip_pitch, const float ini_r_knee_pitch, const float ini_r_ankle_pitch, const float ini_r_ankle_roll, const float ini_r_shoulder_pitch, const float ini_r_shoulder_roll, const float ini_r_elbow_yaw, const float ini_r_elbow_roll) : Message("SetStiffnessParamsMessage")
{
  data_size = sizeof(SetStiffnessParamsMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (SetStiffnessParamsMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  data->motion_pattern = ini_motion_pattern;
  data->head_yaw = ini_head_yaw;
  data->head_pitch = ini_head_pitch;
  data->l_shoulder_pitch = ini_l_shoulder_pitch;
  data->l_shoulder_roll = ini_l_shoulder_roll;
  data->l_elbow_yaw = ini_l_elbow_yaw;
  data->l_elbow_roll = ini_l_elbow_roll;
  data->l_hip_yaw_pitch = ini_l_hip_yaw_pitch;
  data->l_hip_roll = ini_l_hip_roll;
  data->l_hip_pitch = ini_l_hip_pitch;
  data->l_knee_pitch = ini_l_knee_pitch;
  data->l_ankle_pitch = ini_l_ankle_pitch;
  data->l_ankle_roll = ini_l_ankle_roll;
  data->r_hip_yaw_pitch = ini_r_hip_yaw_pitch;
  data->r_hip_roll = ini_r_hip_roll;
  data->r_hip_pitch = ini_r_hip_pitch;
  data->r_knee_pitch = ini_r_knee_pitch;
  data->r_ankle_pitch = ini_r_ankle_pitch;
  data->r_ankle_roll = ini_r_ankle_roll;
  data->r_shoulder_pitch = ini_r_shoulder_pitch;
  data->r_shoulder_roll = ini_r_shoulder_roll;
  data->r_elbow_yaw = ini_r_elbow_yaw;
  data->r_elbow_roll = ini_r_elbow_roll;
  add_fieldinfo(IFT_ENUM, "motion_pattern", 1, &data->motion_pattern, "StiffnessMotionPatternEnum");
  add_fieldinfo(IFT_FLOAT, "head_yaw", 1, &data->head_yaw);
  add_fieldinfo(IFT_FLOAT, "head_pitch", 1, &data->head_pitch);
  add_fieldinfo(IFT_FLOAT, "l_shoulder_pitch", 1, &data->l_shoulder_pitch);
  add_fieldinfo(IFT_FLOAT, "l_shoulder_roll", 1, &data->l_shoulder_roll);
  add_fieldinfo(IFT_FLOAT, "l_elbow_yaw", 1, &data->l_elbow_yaw);
  add_fieldinfo(IFT_FLOAT, "l_elbow_roll", 1, &data->l_elbow_roll);
  add_fieldinfo(IFT_FLOAT, "l_hip_yaw_pitch", 1, &data->l_hip_yaw_pitch);
  add_fieldinfo(IFT_FLOAT, "l_hip_roll", 1, &data->l_hip_roll);
  add_fieldinfo(IFT_FLOAT, "l_hip_pitch", 1, &data->l_hip_pitch);
  add_fieldinfo(IFT_FLOAT, "l_knee_pitch", 1, &data->l_knee_pitch);
  add_fieldinfo(IFT_FLOAT, "l_ankle_pitch", 1, &data->l_ankle_pitch);
  add_fieldinfo(IFT_FLOAT, "l_ankle_roll", 1, &data->l_ankle_roll);
  add_fieldinfo(IFT_FLOAT, "r_hip_yaw_pitch", 1, &data->r_hip_yaw_pitch);
  add_fieldinfo(IFT_FLOAT, "r_hip_roll", 1, &data->r_hip_roll);
  add_fieldinfo(IFT_FLOAT, "r_hip_pitch", 1, &data->r_hip_pitch);
  add_fieldinfo(IFT_FLOAT, "r_knee_pitch", 1, &data->r_knee_pitch);
  add_fieldinfo(IFT_FLOAT, "r_ankle_pitch", 1, &data->r_ankle_pitch);
  add_fieldinfo(IFT_FLOAT, "r_ankle_roll", 1, &data->r_ankle_roll);
  add_fieldinfo(IFT_FLOAT, "r_shoulder_pitch", 1, &data->r_shoulder_pitch);
  add_fieldinfo(IFT_FLOAT, "r_shoulder_roll", 1, &data->r_shoulder_roll);
  add_fieldinfo(IFT_FLOAT, "r_elbow_yaw", 1, &data->r_elbow_yaw);
  add_fieldinfo(IFT_FLOAT, "r_elbow_roll", 1, &data->r_elbow_roll);
}
/** Constructor */
HumanoidMotionInterface::SetStiffnessParamsMessage::SetStiffnessParamsMessage() : Message("SetStiffnessParamsMessage")
{
  data_size = sizeof(SetStiffnessParamsMessage_data_t);
  data_ptr  = malloc(data_size);
  memset(data_ptr, 0, data_size);
  data      = (SetStiffnessParamsMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
  add_fieldinfo(IFT_ENUM, "motion_pattern", 1, &data->motion_pattern, "StiffnessMotionPatternEnum");
  add_fieldinfo(IFT_FLOAT, "head_yaw", 1, &data->head_yaw);
  add_fieldinfo(IFT_FLOAT, "head_pitch", 1, &data->head_pitch);
  add_fieldinfo(IFT_FLOAT, "l_shoulder_pitch", 1, &data->l_shoulder_pitch);
  add_fieldinfo(IFT_FLOAT, "l_shoulder_roll", 1, &data->l_shoulder_roll);
  add_fieldinfo(IFT_FLOAT, "l_elbow_yaw", 1, &data->l_elbow_yaw);
  add_fieldinfo(IFT_FLOAT, "l_elbow_roll", 1, &data->l_elbow_roll);
  add_fieldinfo(IFT_FLOAT, "l_hip_yaw_pitch", 1, &data->l_hip_yaw_pitch);
  add_fieldinfo(IFT_FLOAT, "l_hip_roll", 1, &data->l_hip_roll);
  add_fieldinfo(IFT_FLOAT, "l_hip_pitch", 1, &data->l_hip_pitch);
  add_fieldinfo(IFT_FLOAT, "l_knee_pitch", 1, &data->l_knee_pitch);
  add_fieldinfo(IFT_FLOAT, "l_ankle_pitch", 1, &data->l_ankle_pitch);
  add_fieldinfo(IFT_FLOAT, "l_ankle_roll", 1, &data->l_ankle_roll);
  add_fieldinfo(IFT_FLOAT, "r_hip_yaw_pitch", 1, &data->r_hip_yaw_pitch);
  add_fieldinfo(IFT_FLOAT, "r_hip_roll", 1, &data->r_hip_roll);
  add_fieldinfo(IFT_FLOAT, "r_hip_pitch", 1, &data->r_hip_pitch);
  add_fieldinfo(IFT_FLOAT, "r_knee_pitch", 1, &data->r_knee_pitch);
  add_fieldinfo(IFT_FLOAT, "r_ankle_pitch", 1, &data->r_ankle_pitch);
  add_fieldinfo(IFT_FLOAT, "r_ankle_roll", 1, &data->r_ankle_roll);
  add_fieldinfo(IFT_FLOAT, "r_shoulder_pitch", 1, &data->r_shoulder_pitch);
  add_fieldinfo(IFT_FLOAT, "r_shoulder_roll", 1, &data->r_shoulder_roll);
  add_fieldinfo(IFT_FLOAT, "r_elbow_yaw", 1, &data->r_elbow_yaw);
  add_fieldinfo(IFT_FLOAT, "r_elbow_roll", 1, &data->r_elbow_roll);
}

/** Destructor */
HumanoidMotionInterface::SetStiffnessParamsMessage::~SetStiffnessParamsMessage()
{
  free(data_ptr);
}

/** Copy constructor.
 * @param m message to copy from
 */
HumanoidMotionInterface::SetStiffnessParamsMessage::SetStiffnessParamsMessage(const SetStiffnessParamsMessage *m) : Message("SetStiffnessParamsMessage")
{
  data_size = m->data_size;
  data_ptr  = malloc(data_size);
  memcpy(data_ptr, m->data_ptr, data_size);
  data      = (SetStiffnessParamsMessage_data_t *)data_ptr;
  data_ts   = (message_data_ts_t *)data_ptr;
}

/* Methods */
/** Get motion_pattern value.
 * the motion pattern to update
 * @return motion_pattern value
 */
HumanoidMotionInterface::StiffnessMotionPatternEnum
HumanoidMotionInterface::SetStiffnessParamsMessage::motion_pattern() const
{
  return data->motion_pattern;
}

/** Get maximum length of motion_pattern value.
 * @return length of motion_pattern value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_motion_pattern() const
{
  return 1;
}

/** Set motion_pattern value.
 * the motion pattern to update
 * @param new_motion_pattern new motion_pattern value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_motion_pattern(const StiffnessMotionPatternEnum new_motion_pattern)
{
  data->motion_pattern = new_motion_pattern;
}

/** Get head_yaw value.
 * head_yaw
 * @return head_yaw value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::head_yaw() const
{
  return data->head_yaw;
}

/** Get maximum length of head_yaw value.
 * @return length of head_yaw value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_head_yaw() const
{
  return 1;
}

/** Set head_yaw value.
 * head_yaw
 * @param new_head_yaw new head_yaw value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_head_yaw(const float new_head_yaw)
{
  data->head_yaw = new_head_yaw;
}

/** Get head_pitch value.
 * head_pitch
 * @return head_pitch value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::head_pitch() const
{
  return data->head_pitch;
}

/** Get maximum length of head_pitch value.
 * @return length of head_pitch value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_head_pitch() const
{
  return 1;
}

/** Set head_pitch value.
 * head_pitch
 * @param new_head_pitch new head_pitch value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_head_pitch(const float new_head_pitch)
{
  data->head_pitch = new_head_pitch;
}

/** Get l_shoulder_pitch value.
 * l_shoulder_pitch
 * @return l_shoulder_pitch value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::l_shoulder_pitch() const
{
  return data->l_shoulder_pitch;
}

/** Get maximum length of l_shoulder_pitch value.
 * @return length of l_shoulder_pitch value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_l_shoulder_pitch() const
{
  return 1;
}

/** Set l_shoulder_pitch value.
 * l_shoulder_pitch
 * @param new_l_shoulder_pitch new l_shoulder_pitch value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_l_shoulder_pitch(const float new_l_shoulder_pitch)
{
  data->l_shoulder_pitch = new_l_shoulder_pitch;
}

/** Get l_shoulder_roll value.
 * l_shoulder_roll
 * @return l_shoulder_roll value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::l_shoulder_roll() const
{
  return data->l_shoulder_roll;
}

/** Get maximum length of l_shoulder_roll value.
 * @return length of l_shoulder_roll value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_l_shoulder_roll() const
{
  return 1;
}

/** Set l_shoulder_roll value.
 * l_shoulder_roll
 * @param new_l_shoulder_roll new l_shoulder_roll value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_l_shoulder_roll(const float new_l_shoulder_roll)
{
  data->l_shoulder_roll = new_l_shoulder_roll;
}

/** Get l_elbow_yaw value.
 * l_elbow_yaw
 * @return l_elbow_yaw value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::l_elbow_yaw() const
{
  return data->l_elbow_yaw;
}

/** Get maximum length of l_elbow_yaw value.
 * @return length of l_elbow_yaw value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_l_elbow_yaw() const
{
  return 1;
}

/** Set l_elbow_yaw value.
 * l_elbow_yaw
 * @param new_l_elbow_yaw new l_elbow_yaw value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_l_elbow_yaw(const float new_l_elbow_yaw)
{
  data->l_elbow_yaw = new_l_elbow_yaw;
}

/** Get l_elbow_roll value.
 * l_elbow_roll
 * @return l_elbow_roll value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::l_elbow_roll() const
{
  return data->l_elbow_roll;
}

/** Get maximum length of l_elbow_roll value.
 * @return length of l_elbow_roll value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_l_elbow_roll() const
{
  return 1;
}

/** Set l_elbow_roll value.
 * l_elbow_roll
 * @param new_l_elbow_roll new l_elbow_roll value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_l_elbow_roll(const float new_l_elbow_roll)
{
  data->l_elbow_roll = new_l_elbow_roll;
}

/** Get l_hip_yaw_pitch value.
 * l_hip_yaw_pitch
 * @return l_hip_yaw_pitch value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::l_hip_yaw_pitch() const
{
  return data->l_hip_yaw_pitch;
}

/** Get maximum length of l_hip_yaw_pitch value.
 * @return length of l_hip_yaw_pitch value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_l_hip_yaw_pitch() const
{
  return 1;
}

/** Set l_hip_yaw_pitch value.
 * l_hip_yaw_pitch
 * @param new_l_hip_yaw_pitch new l_hip_yaw_pitch value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_l_hip_yaw_pitch(const float new_l_hip_yaw_pitch)
{
  data->l_hip_yaw_pitch = new_l_hip_yaw_pitch;
}

/** Get l_hip_roll value.
 * l_hip_roll
 * @return l_hip_roll value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::l_hip_roll() const
{
  return data->l_hip_roll;
}

/** Get maximum length of l_hip_roll value.
 * @return length of l_hip_roll value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_l_hip_roll() const
{
  return 1;
}

/** Set l_hip_roll value.
 * l_hip_roll
 * @param new_l_hip_roll new l_hip_roll value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_l_hip_roll(const float new_l_hip_roll)
{
  data->l_hip_roll = new_l_hip_roll;
}

/** Get l_hip_pitch value.
 * l_hip_pitch
 * @return l_hip_pitch value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::l_hip_pitch() const
{
  return data->l_hip_pitch;
}

/** Get maximum length of l_hip_pitch value.
 * @return length of l_hip_pitch value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_l_hip_pitch() const
{
  return 1;
}

/** Set l_hip_pitch value.
 * l_hip_pitch
 * @param new_l_hip_pitch new l_hip_pitch value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_l_hip_pitch(const float new_l_hip_pitch)
{
  data->l_hip_pitch = new_l_hip_pitch;
}

/** Get l_knee_pitch value.
 * l_knee_pitch
 * @return l_knee_pitch value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::l_knee_pitch() const
{
  return data->l_knee_pitch;
}

/** Get maximum length of l_knee_pitch value.
 * @return length of l_knee_pitch value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_l_knee_pitch() const
{
  return 1;
}

/** Set l_knee_pitch value.
 * l_knee_pitch
 * @param new_l_knee_pitch new l_knee_pitch value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_l_knee_pitch(const float new_l_knee_pitch)
{
  data->l_knee_pitch = new_l_knee_pitch;
}

/** Get l_ankle_pitch value.
 * l_ankle_pitch
 * @return l_ankle_pitch value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::l_ankle_pitch() const
{
  return data->l_ankle_pitch;
}

/** Get maximum length of l_ankle_pitch value.
 * @return length of l_ankle_pitch value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_l_ankle_pitch() const
{
  return 1;
}

/** Set l_ankle_pitch value.
 * l_ankle_pitch
 * @param new_l_ankle_pitch new l_ankle_pitch value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_l_ankle_pitch(const float new_l_ankle_pitch)
{
  data->l_ankle_pitch = new_l_ankle_pitch;
}

/** Get l_ankle_roll value.
 * l_ankle_roll
 * @return l_ankle_roll value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::l_ankle_roll() const
{
  return data->l_ankle_roll;
}

/** Get maximum length of l_ankle_roll value.
 * @return length of l_ankle_roll value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_l_ankle_roll() const
{
  return 1;
}

/** Set l_ankle_roll value.
 * l_ankle_roll
 * @param new_l_ankle_roll new l_ankle_roll value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_l_ankle_roll(const float new_l_ankle_roll)
{
  data->l_ankle_roll = new_l_ankle_roll;
}

/** Get r_hip_yaw_pitch value.
 * r_hip_yaw_pitch
 * @return r_hip_yaw_pitch value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::r_hip_yaw_pitch() const
{
  return data->r_hip_yaw_pitch;
}

/** Get maximum length of r_hip_yaw_pitch value.
 * @return length of r_hip_yaw_pitch value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_r_hip_yaw_pitch() const
{
  return 1;
}

/** Set r_hip_yaw_pitch value.
 * r_hip_yaw_pitch
 * @param new_r_hip_yaw_pitch new r_hip_yaw_pitch value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_r_hip_yaw_pitch(const float new_r_hip_yaw_pitch)
{
  data->r_hip_yaw_pitch = new_r_hip_yaw_pitch;
}

/** Get r_hip_roll value.
 * r_hip_roll
 * @return r_hip_roll value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::r_hip_roll() const
{
  return data->r_hip_roll;
}

/** Get maximum length of r_hip_roll value.
 * @return length of r_hip_roll value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_r_hip_roll() const
{
  return 1;
}

/** Set r_hip_roll value.
 * r_hip_roll
 * @param new_r_hip_roll new r_hip_roll value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_r_hip_roll(const float new_r_hip_roll)
{
  data->r_hip_roll = new_r_hip_roll;
}

/** Get r_hip_pitch value.
 * r_hip_pitch
 * @return r_hip_pitch value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::r_hip_pitch() const
{
  return data->r_hip_pitch;
}

/** Get maximum length of r_hip_pitch value.
 * @return length of r_hip_pitch value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_r_hip_pitch() const
{
  return 1;
}

/** Set r_hip_pitch value.
 * r_hip_pitch
 * @param new_r_hip_pitch new r_hip_pitch value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_r_hip_pitch(const float new_r_hip_pitch)
{
  data->r_hip_pitch = new_r_hip_pitch;
}

/** Get r_knee_pitch value.
 * r_knee_pitch
 * @return r_knee_pitch value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::r_knee_pitch() const
{
  return data->r_knee_pitch;
}

/** Get maximum length of r_knee_pitch value.
 * @return length of r_knee_pitch value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_r_knee_pitch() const
{
  return 1;
}

/** Set r_knee_pitch value.
 * r_knee_pitch
 * @param new_r_knee_pitch new r_knee_pitch value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_r_knee_pitch(const float new_r_knee_pitch)
{
  data->r_knee_pitch = new_r_knee_pitch;
}

/** Get r_ankle_pitch value.
 * r_ankle_pitch
 * @return r_ankle_pitch value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::r_ankle_pitch() const
{
  return data->r_ankle_pitch;
}

/** Get maximum length of r_ankle_pitch value.
 * @return length of r_ankle_pitch value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_r_ankle_pitch() const
{
  return 1;
}

/** Set r_ankle_pitch value.
 * r_ankle_pitch
 * @param new_r_ankle_pitch new r_ankle_pitch value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_r_ankle_pitch(const float new_r_ankle_pitch)
{
  data->r_ankle_pitch = new_r_ankle_pitch;
}

/** Get r_ankle_roll value.
 * r_ankle_roll
 * @return r_ankle_roll value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::r_ankle_roll() const
{
  return data->r_ankle_roll;
}

/** Get maximum length of r_ankle_roll value.
 * @return length of r_ankle_roll value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_r_ankle_roll() const
{
  return 1;
}

/** Set r_ankle_roll value.
 * r_ankle_roll
 * @param new_r_ankle_roll new r_ankle_roll value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_r_ankle_roll(const float new_r_ankle_roll)
{
  data->r_ankle_roll = new_r_ankle_roll;
}

/** Get r_shoulder_pitch value.
 * r_shoulder_pitch
 * @return r_shoulder_pitch value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::r_shoulder_pitch() const
{
  return data->r_shoulder_pitch;
}

/** Get maximum length of r_shoulder_pitch value.
 * @return length of r_shoulder_pitch value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_r_shoulder_pitch() const
{
  return 1;
}

/** Set r_shoulder_pitch value.
 * r_shoulder_pitch
 * @param new_r_shoulder_pitch new r_shoulder_pitch value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_r_shoulder_pitch(const float new_r_shoulder_pitch)
{
  data->r_shoulder_pitch = new_r_shoulder_pitch;
}

/** Get r_shoulder_roll value.
 * r_shoulder_roll
 * @return r_shoulder_roll value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::r_shoulder_roll() const
{
  return data->r_shoulder_roll;
}

/** Get maximum length of r_shoulder_roll value.
 * @return length of r_shoulder_roll value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_r_shoulder_roll() const
{
  return 1;
}

/** Set r_shoulder_roll value.
 * r_shoulder_roll
 * @param new_r_shoulder_roll new r_shoulder_roll value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_r_shoulder_roll(const float new_r_shoulder_roll)
{
  data->r_shoulder_roll = new_r_shoulder_roll;
}

/** Get r_elbow_yaw value.
 * r_elbow_yaw
 * @return r_elbow_yaw value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::r_elbow_yaw() const
{
  return data->r_elbow_yaw;
}

/** Get maximum length of r_elbow_yaw value.
 * @return length of r_elbow_yaw value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_r_elbow_yaw() const
{
  return 1;
}

/** Set r_elbow_yaw value.
 * r_elbow_yaw
 * @param new_r_elbow_yaw new r_elbow_yaw value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_r_elbow_yaw(const float new_r_elbow_yaw)
{
  data->r_elbow_yaw = new_r_elbow_yaw;
}

/** Get r_elbow_roll value.
 * r_elbow_roll
 * @return r_elbow_roll value
 */
float
HumanoidMotionInterface::SetStiffnessParamsMessage::r_elbow_roll() const
{
  return data->r_elbow_roll;
}

/** Get maximum length of r_elbow_roll value.
 * @return length of r_elbow_roll value, can be length of the array or number of 
 * maximum number of characters for a string
 */
size_t
HumanoidMotionInterface::SetStiffnessParamsMessage::maxlenof_r_elbow_roll() const
{
  return 1;
}

/** Set r_elbow_roll value.
 * r_elbow_roll
 * @param new_r_elbow_roll new r_elbow_roll value
 */
void
HumanoidMotionInterface::SetStiffnessParamsMessage::set_r_elbow_roll(const float new_r_elbow_roll)
{
  data->r_elbow_roll = new_r_elbow_roll;
}

/** Clone this message.
 * Produces a message of the same type as this message and copies the
 * data to the new message.
 * @return clone of this message
 */
Message *
HumanoidMotionInterface::SetStiffnessParamsMessage::clone() const
{
  return new HumanoidMotionInterface::SetStiffnessParamsMessage(this);
}
/** Check if message is valid and can be enqueued.
 * @param message Message to check
 */
bool
HumanoidMotionInterface::message_valid(const Message *message) const
{
  const SetWalkParamsMessage *m0 = dynamic_cast<const SetWalkParamsMessage *>(message);
  if ( m0 != NULL ) {
    return true;
  }
  const SetWalkArmsParamsMessage *m1 = dynamic_cast<const SetWalkArmsParamsMessage *>(message);
  if ( m1 != NULL ) {
    return true;
  }
  const StopMessage *m2 = dynamic_cast<const StopMessage *>(message);
  if ( m2 != NULL ) {
    return true;
  }
  const WalkStraightMessage *m3 = dynamic_cast<const WalkStraightMessage *>(message);
  if ( m3 != NULL ) {
    return true;
  }
  const WalkSidewaysMessage *m4 = dynamic_cast<const WalkSidewaysMessage *>(message);
  if ( m4 != NULL ) {
    return true;
  }
  const WalkArcMessage *m5 = dynamic_cast<const WalkArcMessage *>(message);
  if ( m5 != NULL ) {
    return true;
  }
  const WalkMessage *m6 = dynamic_cast<const WalkMessage *>(message);
  if ( m6 != NULL ) {
    return true;
  }
  const TurnMessage *m7 = dynamic_cast<const TurnMessage *>(message);
  if ( m7 != NULL ) {
    return true;
  }
  const KickMessage *m8 = dynamic_cast<const KickMessage *>(message);
  if ( m8 != NULL ) {
    return true;
  }
  const ParkMessage *m9 = dynamic_cast<const ParkMessage *>(message);
  if ( m9 != NULL ) {
    return true;
  }
  const GetUpMessage *m10 = dynamic_cast<const GetUpMessage *>(message);
  if ( m10 != NULL ) {
    return true;
  }
  const StandupMessage *m11 = dynamic_cast<const StandupMessage *>(message);
  if ( m11 != NULL ) {
    return true;
  }
  const YawPitchHeadMessage *m12 = dynamic_cast<const YawPitchHeadMessage *>(message);
  if ( m12 != NULL ) {
    return true;
  }
  const SetStiffnessParamsMessage *m13 = dynamic_cast<const SetStiffnessParamsMessage *>(message);
  if ( m13 != NULL ) {
    return true;
  }
  return false;
}

/// @cond INTERNALS
EXPORT_INTERFACE(HumanoidMotionInterface)
/// @endcond


} // end namespace fawkes
