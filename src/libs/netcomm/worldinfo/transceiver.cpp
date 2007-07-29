
/***************************************************************************
 *  transceiver.h - World Info Transceiver
 *
 *  Created: Sun Jan 21 14:15:32 2007
 *  Copyright  2006-2007  Tim Niemueller [www.niemueller.de]
 *
 *  $Id$
 *
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
 */

#include <core/exceptions/system.h>

#include <netcomm/worldinfo/transceiver.h>
#include <netcomm/worldinfo/messages.h>
#include <netcomm/worldinfo/encrypt.h>
#include <netcomm/worldinfo/decrypt.h>

#include <netcomm/socket/datagram_multicast.h>
#include <netcomm/utils/resolver.h>

#include <utils/logging/liblogger.h>

#include <netinet/in.h>


/** @class WorldInfoException <netcomm/worldinfo/transceiver.h>
 * Thrown on critical errors in world info handling.
 * @ingroup NetComm
 */

/** Constructor.
 * @param msg message
 */
WorldInfoException::WorldInfoException(const char *msg)
  : Exception(msg)
{
}


/** @class WorldInfoTransceiver netcomm/worldinfo/transceiver.h
 * Class to send and receive world information.
 * An important point in a domain of cooperating soccer robots is transmitting
 * and receiving a robot's belief of its surrounding. The world info
 * transceiver does exactly that. It allows for sending information about the
 * robot's pose and velocity and its perception of the ball and other robots
 * on the field.
 *
 * The unit for distances and positions is meter (m), speed is given in
 * meter per second (m/s), angles are given in radiant (rad). Angles can be in
 * the range 0 to 2 * PI or -PI to PI. Since they can be converted easily
 * between these ranges without further information users of such information
 * shall be able to process both.
 *
 * Coordinates are given in a right-handed coordinate system with the origin in
 * center of the field, X pointing towards the opponent goal, Y to the right
 * and Z downwards.
 *
 * Information is transmitted with a simple protocol via UDP Multicast packets.
 *
 * A call to send() will reset all information, thus all opponents are removed
 * from the list to be sent, positions of robot and ball are marked invalid.
 * You have to call the appropriate set methods before the information is sent.
 * You can thus call send() at any time but only changed information
 * (information set since last send() call) is transmitted over the network.
 *
 * @ingroup NetComm
 * @author Tim Niemueller
 */


/** Constructor.
 * @param addr multicast address to send information to and receive from
 * @param port UDP port to send information to and receive from
 * @param key encryption key
 * @param iv encryption initialisation vector
 * @param resolver An initialized network resolver, is NULL is supplied
 * an internal resolver will be created without mDNS support.
 * @exception OutOfMemoryException thrown if internal buffers cannot be created
 */
WorldInfoTransceiver::WorldInfoTransceiver(const char *addr, unsigned short port,
					   const char *key, const char *iv,
					   NetworkNameResolver *resolver)
{
  try {
    s = new MulticastDatagramSocket(addr, port);
    s->bind();
  } catch (SocketException &e) {
    e.append("WorldInfoTransceiver cannot instantiate socket for %s:%u", addr, port);
    throw;
  }

  in_buffer = malloc(WORLDINFO_MTU);
  out_buffer = malloc(WORLDINFO_MTU);
  if (! in_buffer || ! out_buffer) {
    throw OutOfMemoryException();
  }
  encryptor = new WorldInfoMessageEncryptor((const unsigned char *)key, (const unsigned char *)iv);
  decryptor = new WorldInfoMessageDecryptor((const unsigned char *)key, (const unsigned char *)iv);

  // set maximum size buffer to get valid results from encryptor
  encryptor->set_plain_buffer(out_buffer, WORLDINFO_MTU);

  crypt_buffer_size  = encryptor->recommended_crypt_buffer_size();
  crypted_out_buffer = malloc(crypt_buffer_size);
  crypted_in_buffer  = malloc(crypt_buffer_size);

  if (! crypted_in_buffer || ! crypted_out_buffer) {
    throw OutOfMemoryException();
  }

  encryptor->set_crypt_buffer(crypted_out_buffer, crypt_buffer_size);

  decryptor->set_plain_buffer(in_buffer, WORLDINFO_MTU);

  if ( resolver == NULL ) {
    this->resolver = new NetworkNameResolver();
    resolver_delete = true;
  } else {
    this->resolver = resolver;
    resolver_delete = false;
  }

  out_seq = 0;
}


/** Destructor. */
WorldInfoTransceiver::~WorldInfoTransceiver()
{
  free(out_buffer);
  free(in_buffer);
  free(crypted_out_buffer);
  free(crypted_in_buffer);
  delete s;
  delete encryptor;
  if ( resolver_delete ) {
    delete resolver;
  }
}


/** Set loopback of sent packets.
 * This sets whether packets should be looped back to local sockets for multicast
 * communication.
 * @param loop true to deliver sent packets to local sockets, false prevent delivering
 * @see MulticastDatagramSocket::set_loop()
 */
void
WorldInfoTransceiver::set_loop(bool loop)
{
  s->set_loop( loop );
}

/** Add a handler for world information.
 * World information will be dispatched to all registered handlers as soon it
 * is received.
 * @param h handler to register
 */
void
WorldInfoTransceiver::add_handler(WorldInfoHandler *h)
{
  handlers.lock();
  handlers.push_back(h);
  handlers.sort();
  handlers.unique();
  handlers.unlock();
}


/** Remove handler for world information.
 * The handler is removed from the list of handlers that incoming information
 * is dispatched to. No error is thrown if the handler was never registered
 * so it is safe to call this for any handler.
 * @param h handler to remove from subscriber list
 */
void
WorldInfoTransceiver::rem_handler(WorldInfoHandler *h)
{
  handlers.remove_locked(h);
}


/** Flush sequence numbers conditionally.
 * This will conditionally flush the sequence numbers stored per sender. The
 * sequence numbers are stored per IP. With this method you can flush the
 * sequence numbers that have been inactive for a specified time. A recommended
 * value is 10 seconds. You may NOT call this concurrently to recv()!
 * @param sec number of seconds since that must have passed without a message
 * to remove a specific IP from sequence list
 */
void
WorldInfoTransceiver::flush_sequence_numbers(unsigned int sec)
{
  time_t limit = time(NULL) - sec;
  
  std::map<uint32_t, time_t>::iterator   lrtit2;
  lrtit = last_received_time.begin();
  while (lrtit != last_received_time.end()) {
    if ( (*lrtit).second < limit ) {
      sequence_numbers.erase((*lrtit).first);
      lrtit2 = lrtit;
      ++lrtit;
      last_received_time.erase(lrtit2);
    } else {
      ++lrtit;
    }
  }
}


/** Set global pose of robot.
 * Global pose of sensing robot (x, y, theta) with the origin in the
 * middle of the field, right handed coordinate system (y to opponent goal,
 * x to the right, z pointing upwards, same as in simulation league).
 * Theta points in y direction (theta = 0 iff robot front points to opponent
 * goal).
 * The confidence about the robot's pose is transmitted as a 3x3 covariance
 * matrix.
 * @param x x position of robot
 * @param y y position of robot
 * @param theta rotation of robot
 * @param covariance covariance matrix with 9 entries, ordered as three concatenated
 * rows (first row, three floats, second row, three floats, third row, three
 * floats). No length check or whatsoever is done. This will crash if c is not
 * long enough! c will not be copied but referenced so it has to exist when
 * send() is called!
 */
void
WorldInfoTransceiver::set_pose(float x, float y, float theta, float *covariance)
{
  pose_x          = x;
  pose_y          = y;
  pose_theta      = theta;
  pose_covariance = covariance;
  pose_changed    = true;
}


/** Set velocity of the robot.
 * Set the current velocity of the robot.
 * @param vel_x velocity in x direction
 * @param vel_y velocity in y direction
 * @param vel_theta rotational velocity, positive velocity means clockwise
 * rotation, negative velocity means counter-clockwise.
 * @param covariance covariance matrix with 9 entries, ordered as three concatenated
 * rows (first row, three floats, second row, three floats, third row, three
 * floats). No length check or whatsoever is done. This will crash if c is not
 * long enough! c will not be copied but referenced so it has to exist when
 * send() is called!
 */
void
WorldInfoTransceiver::set_velocity(float vel_x, float vel_y, float vel_theta, float *covariance)
{
  this->vel_x          = vel_x;
  this->vel_y          = vel_y;
  this->vel_theta      = vel_theta;
  this->vel_covariance = covariance;
  this->vel_changed    = true;
}


/** Set ball position.
 * Set the ball perception relative to the current robot position.
 * Note that the ball position is given in polar coordinates in
 * 3D space!
 * The confidence about the ball position is transmitted as a 3x3 covariance
 * matrix.
 * @param dist distance to ball in meters
 * @param pitch pitch angle to ball
 * @param yaw yaw angle to ball
 * @param covariance covariance matrix with 9 entries, ordered as three concatenated
 * rows (first row, three floats, second row, three floats, third row, three
 * floats). No length check or whatsoever is done. This will crash if c is not
 * long enough! c will not be copied but referenced so it has to exist when
 * send() is called!
 */
void
WorldInfoTransceiver::set_ball_pos(float dist, float pitch, float yaw, float *covariance)
{
  ball_dist       = dist;
  ball_pitch      = pitch;
  ball_yaw        = yaw;
  ball_covariance = covariance;
  ball_changed    = true;
}


/** Set ball velocity.
 * Set the current velocity of the robot.
 * @param vel_x velocity in x direction
 * @param vel_y velocity in y direction
 * @param vel_z velocity in z direction
 * @param covariance covariance matrix with 9 entries, ordered as three concatenated
 * rows (first row, three floats, second row, three floats, third row, three
 * floats). No length check or whatsoever is done. This will crash if c is not
 * long enough! c will not be copied but referenced so it has to exist when
 * send() is called!
 */
void
WorldInfoTransceiver::set_ball_velocity(float vel_x, float vel_y, float vel_z, float *covariance)
{
  ball_vel_x          = vel_x;
  ball_vel_y          = vel_y;
  ball_vel_z          = vel_z;
  ball_vel_covariance = covariance;
  ball_vel_changed    = true;
}


/** Clear opponents list.
 * Clear the list of opponents that has to be transmitted. This is done
 * implicitly in send().
 */
void
WorldInfoTransceiver::clear_opponents()
{
  opponents.clear();
}


/** Add opponent to transmit list.
 * Add an opponent to the list of opponents to be transmitted on next send()
 * call. Opponents are given in a 2D polar coordinate system (assumption is that
 * robots don't fly in the soccer domain).
 * @param distance to opponent
 * @param angle angle to opponent (angle is zero if opponent is in front of robot,
 * positive if right of robot, negative if left of robot).
 * @param covariance covariance matrix with 4 entries, ordered as two concatenated
 * rows (first row, two floats, second row, two floats. No length check or
 * whatsoever is done. This will crash if c is not
 * long enough! c will not be copied but referenced so it has to exist when
 * send() is called!
 */
void
WorldInfoTransceiver::add_opponent(float distance, float angle, float *covariance)
{
  opponent_t o = { distance, angle, covariance };
  opponents.push_back(o);
}


/** Append packet to outbound buffer.
 * @param msg_type message type
 * @param msg message buffer
 * @param msg_size size of message buffer
 * @exception OutOfMemoryException thrown if message is too big or if the
 * remaining size in the outbound buffer is not big enough
 */
void
WorldInfoTransceiver::append_outbound(uint16_t msg_type,
				      void *msg, uint16_t msg_size)
{
  worldinfo_message_header_t mh;

  if ( (outbound_bytes + sizeof(mh) + msg_size ) > WORLDINFO_MTU ) {
    throw OutOfMemoryException();
  }

  // per message header
  mh.type = htons(msg_type);
  mh.size = htons(msg_size);
  memcpy(outbound_buffer, &mh, sizeof(mh));

  outbound_bytes  += sizeof(mh);
  outbound_buffer += sizeof(mh);
  
  // message body
  memcpy(outbound_buffer, msg, msg_size);
  outbound_bytes  += msg_size;
  outbound_buffer += msg_size;
  ++outbound_num_msgs;
}


/** Reset outbound buffer.
 */
void
WorldInfoTransceiver::reset_outbound()
{
  worldinfo_header_t *header = (worldinfo_header_t *)out_buffer;
  header->beef = htons(0xBEEF);
  header->version  = WORLDINFO_VERSION;

  outbound_buffer   = (unsigned char *)out_buffer + sizeof(worldinfo_header_t);
  outbound_bytes    = sizeof(worldinfo_header_t);
  outbound_num_msgs = 0;
}


/** Send information.
 * All information that has been set since last call is sent over the network.
 * This implicitly resets all information and flushes the opponent list.
 */
void
WorldInfoTransceiver::send()
{
  worldinfo_header_t *header = (worldinfo_header_t *)out_buffer;

  reset_outbound();

  worldinfo_fat_message_t fatmsg;

  if ( pose_changed ) {
    worldinfo_pose_message_t pm;
    pm.x = pose_x;
    pm.y = pose_y;
    pm.theta = pose_theta;
    memcpy(&(pm.covariance), pose_covariance, sizeof(pm.covariance));
    pose_changed = false;

    append_outbound(WORLDINFO_MSGTYPE_POSE, &pm, sizeof(pm));

    // fill fat msg
    memcpy(&(fatmsg.pose), &pm, sizeof(pm));
    fatmsg.valid_pose = 1;
  } else {
    fatmsg.valid_pose = 0;
  }



  if ( vel_changed ) {
    worldinfo_velocity_message_t vm;
    vm.vel_x     = vel_x;
    vm.vel_y     = vel_y;
    vm.vel_theta = vel_theta;
    memcpy(&(vm.covariance), vel_covariance, sizeof(vm.covariance));
    vel_changed = false;

    append_outbound(WORLDINFO_MSGTYPE_VELO, &vm, sizeof(vm));

    // fill fat msg
    memcpy(&(fatmsg.velo), &vm, sizeof(vm));
    fatmsg.valid_velo = 1;
  } else {
    fatmsg.valid_velo = 0;
  }

  if ( ball_changed ) {
    worldinfo_relballpos_message_t bm;
    bm.dist  = ball_dist;
    bm.pitch = ball_pitch;
    bm.yaw   = ball_yaw;
    memcpy(&(bm.covariance), ball_covariance, sizeof(bm.covariance));
    ball_changed = false;

    append_outbound(WORLDINFO_MSGTYPE_RELBALL, &bm, sizeof(bm));

    // fill fat msg
    memcpy(&(fatmsg.relball_pos), &bm, sizeof(bm));
    fatmsg.valid_relball_pos = 1;
  } else {
    fatmsg.valid_relball_pos = 0;
  }

  if ( ball_vel_changed ) {
    worldinfo_relballvelo_message_t rbvm;
    rbvm.vel_x = ball_vel_x;
    rbvm.vel_y = ball_vel_y;
    rbvm.vel_z = ball_vel_z;
    memcpy(&(rbvm.covariance), ball_vel_covariance, sizeof(rbvm.covariance));
    ball_vel_changed = false;

    append_outbound(WORLDINFO_MSGTYPE_RELBALLVELO, &rbvm, sizeof(rbvm));

    // fill fat msg
    memcpy(&(fatmsg.relball_velo), &rbvm, sizeof(rbvm));
    fatmsg.valid_relball_velo = 1;
  } else {
    fatmsg.valid_relball_velo = 0;
  }

  // Append opponents
  unsigned int num_opponents = 0;
  for ( oppit = opponents.begin(); oppit != opponents.end(); ++oppit) {
    worldinfo_opppose_message_t opm;
    opm.dist  = (*oppit).distance;
    opm.angle = (*oppit).angle;
    memcpy(&(opm.covariance), (*oppit).covariance, sizeof(opm.covariance));

    append_outbound(WORLDINFO_MSGTYPE_OPP_POSE, &opm, sizeof(opm));

    if ( num_opponents < WORLDINFO_FATMSG_NUMOPPS ) {
      // fill fat msg
      memcpy(&(fatmsg.opponents[num_opponents]), &opm, sizeof(opm));
      ++num_opponents;
    }
  }
  fatmsg.num_opponents = num_opponents;
  opponents.clear();

  if ( outbound_num_msgs > 0 ) {
    // send slim msgs
    header->seq      = htonl(out_seq++);

    encryptor->set_plain_buffer(out_buffer, outbound_bytes);
    crypted_out_bytes = encryptor->encrypt();

    s->send(crypted_out_buffer, crypted_out_bytes);

    // send fat msg
    header->seq      = htonl(out_seq++);

    encryptor->set_plain_buffer(&fatmsg, sizeof(fatmsg));
    crypted_out_bytes = encryptor->encrypt();

    s->send(crypted_out_buffer, crypted_out_bytes);
  }

}


/** Receive information.
 * This checks if there is information on the network waiting to be received
 * and if so receives and processes the information and dispatches it to all
 * registered handlers. If you order it to block this method will block until
 * information has been received and dispatched (useful if running in a
 * thread).
 *
 * Received packets will be ignored if
 * - they do not start with 0xBEEF
 * - they are of an incompatible version
 * - the sequence number is smaller or equal to an already received packet
 * They will only be partially handled if
 * - a packet has been truncated (truncated message is ignored)
 * - an unknown message type is encountered (message is ignored)
 * - a message size does not match the expected size for a given type (message is ignored)
 *
 * @param block set to true for blocking operation, in this case recv() will
 * block until data is available, false for non-blocking operation where recv()
 * will immediately return if there is no data available
 * @param max_num_msgs maximum number of messages to process in a single
 * call to recv(). Set to 0 for an unlimited number of messages per call (this
 * can block for an infinite time if messages are coming in fast).
 */
void
WorldInfoTransceiver::recv(bool block, unsigned int max_num_msgs)
{
  if ( ! block ) {
    if ( ! s->available() ) {
      return;
    }
  }

  handlers.lock();

  unsigned int num_msgs = (max_num_msgs == 0 ? 0 : 1);
  do {
    struct sockaddr_in from;
    socklen_t addr_len = sizeof(from);
    size_t bytes = crypt_buffer_size;

    if ( max_num_msgs != 0 )  ++num_msgs;

    bytes = s->recv(crypted_in_buffer, bytes, (struct sockaddr *)&from, &addr_len);

    // decryptor decrypts to in_buffer, see constructor
    decryptor->set_crypt_buffer(crypted_in_buffer, bytes);
    try {
      inbound_bytes = decryptor->decrypt();
    } catch (MessageDecryptionException &e) {
      e.printTrace();
      LibLogger::log_warn("WorldInfoTransceiver", "Message decryption failed, ignoring");
      continue;
    }

    /*
    cout << "Plain:";
    for (size_t i = 0; i < inbound_bytes; ++i) {
      unsigned int u = *((unsigned char *)in_buffer + i); 
      printf("%02x ", u);
    }
    cout << endl;
    */

    // Process
    worldinfo_header_t *header = (worldinfo_header_t *)in_buffer;
    if ( ntohs(header->beef) != 0xBEEF ) {
      // throw WorldInfoException("Incorrect message received, wrong key?");
      LibLogger::log_warn("WorldInfoTransceiver", "Invalid message received (no 0xBEEF), ignoring");
      continue;
    }

    if ( header->version != WORLDINFO_VERSION ) {
      LibLogger::log_warn("WorldInfoTransceiver", "Unsupported version of world info data received, ignoring");
      continue;
    }
    
    // Sequence number handling per client, IPv4 only, for IPv6 in the pre-128-bit era
    // we would need a custom compare function
    unsigned int cseq = ntohl(header->seq);
    if ( sequence_numbers.find(from.sin_addr.s_addr) != sequence_numbers.end() ) {
      if ( cseq <= sequence_numbers[from.sin_addr.s_addr] ) {
	// Already received (loop) or replay attack, just ignore
	LibLogger::log_warn("WorldInfoTransceiver", "Received packet twice, ignoring");
	continue;
      }
    }
    sequence_numbers[from.sin_addr.s_addr] = cseq;
    last_received_time[from.sin_addr.s_addr] = time(NULL);

    inbound_bytes -= sizeof(worldinfo_header_t);
    inbound_buffer = (unsigned char *)in_buffer + sizeof(worldinfo_header_t);

    char *hostname = NULL;
    if ( ! resolver->resolve_address((struct sockaddr *)&from, sizeof(from), &hostname) ) {
      hostname = "unknown";
    }

    // Go through messages
    while ( inbound_bytes > 0 ) {
      worldinfo_message_header_t *msgh = (worldinfo_message_header_t *)inbound_buffer;
      inbound_bytes  -= sizeof(worldinfo_message_header_t);
      inbound_buffer += sizeof(worldinfo_message_header_t);
      uint16_t msg_type = ntohs(msgh->type);
      uint16_t msg_size = ntohs(msgh->size);
      //cout << "Message type: " << msg_type << "  size: " << msg_size
      //     << "  ntype: " << msgh->type << "  nsize: " << msgh->size << endl;
      if ( inbound_bytes < msg_size ) {
	LibLogger::log_warn("WorldInfoTransceiver", "Truncated packet received or protocol "
			    "error, ignoring rest of packet (got %lu bytes, but expected "
			    "%lu bytes)", inbound_bytes, msg_size);
	break;
      }
      switch ( msg_type ) {
      case WORLDINFO_MSGTYPE_POSE:
	if ( msg_size == sizeof(worldinfo_pose_message_t) ) {
	  worldinfo_pose_message_t *pose_msg = (worldinfo_pose_message_t *)inbound_buffer;
	  for ( hit = handlers.begin(); hit != handlers.end(); ++hit ) {
	    (*hit)->pose_rcvd(hostname,
			      pose_msg->x, pose_msg->y, pose_msg->theta,
			      pose_msg->covariance);
	  }
	} else {
	  LibLogger::log_warn("WorldInfoTransceiver", "Invalid pose message received "
			      "(got %lu bytes but expected %lu bytes), ignoring",
			      msg_size, sizeof(worldinfo_pose_message_t));
	}
	break;

      case WORLDINFO_MSGTYPE_VELO:
	if ( msg_size == sizeof(worldinfo_velocity_message_t) ) {
	  worldinfo_velocity_message_t *velo_msg = (worldinfo_velocity_message_t *)inbound_buffer;
	  for ( hit = handlers.begin(); hit != handlers.end(); ++hit ) {
	    (*hit)->velocity_rcvd(hostname,
				  velo_msg->vel_x, velo_msg->vel_y, velo_msg->vel_theta,
				  velo_msg->covariance);
	  }
	} else {
	  LibLogger::log_warn("WorldInfoTransceiver", "Invalid velocity message received "
			      "(got %lu bytes but expected %lu bytes), ignoring",
			      msg_size, sizeof(worldinfo_velocity_message_t));
	}
	break;

      case WORLDINFO_MSGTYPE_RELBALL:
	if ( msg_size == sizeof(worldinfo_relballpos_message_t) ) {
	  worldinfo_relballpos_message_t *ball_msg = (worldinfo_relballpos_message_t *)inbound_buffer;
	  for ( hit = handlers.begin(); hit != handlers.end(); ++hit ) {
	    (*hit)->ball_pos_rcvd(hostname,
				  ball_msg->dist, ball_msg->pitch, ball_msg->yaw,
				  ball_msg->covariance);
	  }
	} else {
	  LibLogger::log_warn("WorldInfoTransceiver", "Invalid relative ball pos message received "
			      "(got %lu bytes but expected %lu bytes), ignoring",
			      msg_size, sizeof(worldinfo_relballpos_message_t));
	}
	break;

      case WORLDINFO_MSGTYPE_RELBALLVELO:
	if ( msg_size == sizeof(worldinfo_relballvelo_message_t) ) {
	  worldinfo_relballvelo_message_t *bvel_msg = (worldinfo_relballvelo_message_t *)inbound_buffer;
	  for ( hit = handlers.begin(); hit != handlers.end(); ++hit ) {
	    (*hit)->ball_velocity_rcvd(hostname,
				       bvel_msg->vel_x, bvel_msg->vel_y, bvel_msg->vel_z,
				       bvel_msg->covariance);
	  }
	} else {
	  LibLogger::log_warn("WorldInfoTransceiver", "Invalid relative ball velocity message received "
			      "(got %lu bytes but expected %lu bytes), ignoring",
			      msg_size, sizeof(worldinfo_relballvelo_message_t));
	}
	break;

      case WORLDINFO_MSGTYPE_OPP_POSE:
	if ( msg_size == sizeof(worldinfo_opppose_message_t) ) {
	  worldinfo_opppose_message_t *oppp_msg = (worldinfo_opppose_message_t *)inbound_buffer;
	  for ( hit = handlers.begin(); hit != handlers.end(); ++hit ) {
	    (*hit)->opponent_pose_rcvd(hostname,
				       oppp_msg->dist, oppp_msg->angle,
				       oppp_msg->covariance);
	  }
	} else {
	  LibLogger::log_warn("WorldInfoTransceiver", "Invalid opponent pose message received "
			      "(got %lu bytes but expected %lu bytes), ignoring",
			      msg_size, sizeof(worldinfo_opppose_message_t));
	}
	break;

      case WORLDINFO_MSGTYPE_FAT_WORLDINFO:
	if ( msg_size == sizeof(worldinfo_fat_message_t) ) {
	  worldinfo_fat_message_t *fat_msg = (worldinfo_fat_message_t *)inbound_buffer;
	  for ( hit = handlers.begin(); hit != handlers.end(); ++hit ) {
	    if ( fat_msg->valid_pose ) {
	      (*hit)->pose_rcvd(hostname,
				fat_msg->pose.x, fat_msg->pose.y, fat_msg->pose.theta,
				fat_msg->pose.covariance);
	    }

	    if ( fat_msg->valid_velo ) {
	      (*hit)->velocity_rcvd(hostname,
				    fat_msg->velo.vel_x, fat_msg->velo.vel_y,
				    fat_msg->velo.vel_theta, fat_msg->velo.covariance);
	    }
	    if ( fat_msg->valid_relball_pos ) {
	      (*hit)->ball_pos_rcvd(hostname,
				    fat_msg->relball_pos.dist, fat_msg->relball_pos.pitch,
				    fat_msg->relball_pos.yaw, fat_msg->relball_pos.covariance);
	    }
	    if ( fat_msg->valid_relball_velo ) {
	      (*hit)->ball_velocity_rcvd(hostname,
					 fat_msg->relball_velo.vel_x,
					 fat_msg->relball_velo.vel_y,
					 fat_msg->relball_velo.vel_z,
					 fat_msg->relball_velo.covariance);
	    }

	    if ( fat_msg->num_opponents > WORLDINFO_FATMSG_NUMOPPS ) {
	      // We can't handle this
	      LibLogger::log_warn("WorldInfoTransceiver", "Too many opponents marked valid in message "
				  "(got %lu but expected a maximum of %lu), ignoring",
				  fat_msg->num_opponents, WORLDINFO_FATMSG_NUMOPPS);
	    } else {
	      for ( unsigned int i = 0; i < fat_msg->num_opponents; ++i ) {
		(*hit)->opponent_pose_rcvd(hostname,
					   fat_msg->opponents[i].dist,
					   fat_msg->opponents[i].angle,
					   fat_msg->opponents[i].covariance);
	      }
	    }
	  } // end for each handler
	} else {
	  LibLogger::log_warn("WorldInfoTransceiver", "Invalid fat message received "
			      "(got %lu bytes but expected %lu bytes), ignoring",
			      msg_size, sizeof(worldinfo_fat_message_t));
	}
	break;


      default:
	  LibLogger::log_warn("WorldInfoTransceiver", "Unknown message type %u received "
			      ", ignoring", msg_type);
      }
      // there is more to process
      inbound_bytes  -= msg_size;
      inbound_buffer += msg_size;
    }

  } while ( s->available() && (num_msgs <= max_num_msgs) );

  handlers.unlock();
}


/** Get last sent plain buffer.
 * This method is meant to be used for debugging and testing purposes only.
 * @return last plain text message buffer
 */
void *
WorldInfoTransceiver::last_sent_plain_buffer()
{
  return out_buffer;
}


/** Get last sent plain buffer size.
 * This method is meant to be used for debugging and testing purposes only.
 * @return last plain text message buffer size
 */
size_t
WorldInfoTransceiver::last_sent_plain_buffer_size()
{
  return outbound_bytes;
}


/** Get last sent crypted buffer.
 * This method is meant to be used for debugging and testing purposes only.
 * @return last crytped message buffer
 */
void *
WorldInfoTransceiver::last_sent_crypted_buffer()
{
  return crypted_out_buffer;
}


/** Get last sent crypted buffer size.
 * This method is meant to be used for debugging and testing purposes only.
 * @return last crypted message buffer size
 */
size_t
WorldInfoTransceiver::last_sent_crypted_buffer_size()
{
  return crypted_out_bytes;
}
