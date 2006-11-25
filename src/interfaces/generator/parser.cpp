 
/***************************************************************************
 *  parser.cpp - Interface config parser
 *
 *  Generated: Tue Oct 10 17:41:13 2006
 *  Copyright  2006  Tim Niemueller [www.niemueller.de]
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <interfaces/generator/parser.h>
#include <interfaces/generator/exceptions.h>

#include <iostream>
#include <vector>

#include <libxml++/libxml++.h>

using namespace std;
using namespace xmlpp;


/** @class InterfaceParser interfaces/generator/parser.h
 * Parser used to get information out of interface template. Uses
 * XML parser internally.
 */


/** Constructor
 * @param config_filename file name of config (interface template)
 */
InterfaceParser::InterfaceParser(string config_filename)
{
  dom = new DomParser();
  dom->set_validate();
  dom->set_substitute_entities();
  dom->parse_file(config_filename);
  root = dom->get_document()->get_root_node();
  if ( root == NULL ) {
    throw InterfaceGeneratorInvalidDocumentException("root == NULL");
  }
}


/** Destructor. */
InterfaceParser::~InterfaceParser()
{
  delete dom;
}


/** Get parsed fields.
 * Get fields stored below the given node.
 * @param node root node where to start searching
 * @return vector of field representations.
 */
std::vector<InterfaceField>
InterfaceParser::getFields(xmlpp::Node *node)
{
  vector<InterfaceField> result;
  NodeSet set = node->find("field");
  for (NodeSet::iterator i = set.begin(); i != set.end(); ++i) {
    // Get constant name
    NodeSet nameset = (*i)->find("text()");
    if ( nameset.size() == 0 ) {
      throw InterfaceGeneratorInvalidContentException("no name for constant");
    }
    const TextNode *comment_node = dynamic_cast<const TextNode *>(nameset[0]);
    if ( ! comment_node ) {
      throw InterfaceGeneratorInvalidContentException("comment node not text node for constant");
    }
    std::string field_comment = comment_node->get_content();
    //std::cout << "Field name: " << field_name << std::endl;
    

    InterfaceField f;
    f.setComment(field_comment);
    const Element * el = dynamic_cast<const Element *>(*i);
    if ( el ) {
      // valid element
      const Element::AttributeList& attrs = el->get_attributes();
      for(Element::AttributeList::const_iterator iter = attrs.begin(); iter != attrs.end(); ++iter) {
	const Attribute* attr = *iter;
	//std::cout << "  Attribute " << attr->get_name() << " = " << attr->get_value() << std::endl;
	f.setAttribute(attr->get_name(), attr->get_value());
      }
    } else {
      throw InterfaceGeneratorInvalidContentException("constant is not an element");
    }

    try {
      f.valid();
      result.push_back(f);
    } catch ( Exception &e ) {
      e.printTrace();
    }
  }
  for (vector<InterfaceField>::iterator i = result.begin(); i != result.end(); ++i) {
    for (vector<InterfaceField>::iterator j = i + 1; j != result.end(); ++j) {
      if ( (*i).getName() == (*j).getName() ) {
	throw InterfaceGeneratorAmbiguousNameException((*i).getName().c_str(), "field");
      }
    }
  }

  return result;
}


/** Print fields.
 * Print fields to stdout.
 * @param fields fields to print
 */
void
InterfaceParser::printFields(vector<InterfaceField> &fields)
{
  for (vector<InterfaceField>::iterator i = fields.begin(); i != fields.end(); ++i) {
    cout << "  Field: name=" << (*i).getName() << "  type=" << (*i).getType();
    if ( (*i).getLength() != "" ) {
      cout << "  length=" << (*i).getLength();
    }
    if ( (*i).getBits() != "" ) {
      cout << "  bits=" << (*i).getBits();
    }
    if ( (*i).getValidFor() != "" ) {
      cout << "  validfor=" << (*i).getValidFor();
    }
    if ( (*i).getDefaultValue() != "" ) {
      cout << "  default=" << (*i).getDefaultValue();
    }
    vector<string> flags = (*i).getFlags();
    if ( flags.size() > 0 ) {
      cout << "  flags=";
      vector<string>::iterator j = flags.begin();
      while (j != flags.end()) {
	cout << *j;
	++j;
	if ( j != flags.end()) {
	  cout << ",";
	}
      }
    }
    cout << endl;
  }
}


/** Print parsed config.
 * @param constants parsed constants
 * @param enum_constants parsed enum_constants
 * @param data_fields parsed data fields
 * @param messages parsed messages.
 */
void
InterfaceParser::printParsed(vector<InterfaceConstant> &     constants,
			     vector<InterfaceEnumConstant> & enum_constants,
			     vector<InterfaceField> &        data_fields,
			     vector<InterfaceMessage> &      messages)
{
  cout << "Constants" << endl;
  for (vector<InterfaceConstant>::iterator i = constants.begin(); i != constants.end(); ++i) {
    cout << "  Constant: name=" << (*i).getName() << "  type=" << (*i).getType()
	 << "  value=" << (*i).getValue() << endl;
  }
  
  cout << "EnumConstants" << endl;
  for (vector<InterfaceEnumConstant>::iterator i = enum_constants.begin(); i != enum_constants.end(); ++i) {
    cout << "  EnumConstant: name=" << (*i).getName() << endl;
    vector< pair<string,string> > items = (*i).getItems();
    for (vector< pair<string,string> >::iterator j = items.begin(); j != items.end(); ++j) {
	cout << "    Item: " << (*j).first << "(" << (*j).second << ")" << endl;
    }
  }
    
  cout << "Data block" << endl;
  printFields(data_fields);
  for (vector<InterfaceMessage>::iterator i = messages.begin(); i != messages.end(); ++i) {
    cout << "Message: name=" << (*i).getName() << endl;
    vector<InterfaceField> msg_fields = (*i).getFields();
    printFields(msg_fields);
  }
}


/** Print parsed data. */
void
InterfaceParser::print()
{
  printParsed(constants, enum_constants, data_fields, messages);
}


/** Parse config. */
void
InterfaceParser::parse()
{
  NodeSet set;

  constants.clear();
  enum_constants.clear();
  data_fields.clear();
  messages.clear();

  /*
   * Name and author
   *
   */
  const Element * el = dynamic_cast<const Element *>(root);
  if ( el ) {
    // valid element
    Attribute *attr;
    attr = el->get_attribute("name");
    if ( ! attr ) {
      throw InterfaceGeneratorInvalidContentException("no name for interface");
    }
    name = attr->get_value();

    attr = el->get_attribute("author");
    if ( attr ) {
      author = attr->get_value();
    }
  } else {
    throw InterfaceGeneratorInvalidContentException("root is not an element");
  }

  /*
   * constants
   *
   */
  NodeSet constants_set = root->find("/interface/constants");
  if ( constants_set.size() > 1 ) {
    throw InterfaceGeneratorInvalidContentException("more than one constants block");
  }
  if ( constants_set.size() == 1 ) {
    // there are actually constants
    set = constants_set[0]->find("constant");
    for (NodeSet::iterator i = set.begin(); i != set.end(); ++i) {

      // Get constant name
      NodeSet nameset = (*i)->find("text()");
      if ( nameset.size() == 0 ) {
	throw InterfaceGeneratorInvalidContentException("no name for constant");
      }
      const TextNode *comment_node = dynamic_cast<const TextNode *>(nameset[0]);
      if ( ! comment_node ) {
	throw InterfaceGeneratorInvalidContentException("name node not text node for constant");
    }
      std::string const_comment = comment_node->get_content();
      //std::cout << "Constant name: " << const_name << std::endl;
      
      // Get attributes
      std::string type;
      std::string value;
      std::string const_name;
    
      const Element * el = dynamic_cast<const Element *>(*i);
      if ( el ) {
	// valid element
	Attribute *attr;
	attr = el->get_attribute("type");
	if ( ! attr ) {
	  throw InterfaceGeneratorInvalidContentException("no type for constant");
	}
	type = attr->get_value();

	attr = el->get_attribute("name");
	if ( ! attr ) {
	  throw InterfaceGeneratorInvalidContentException("no name for constant");
	}
	const_name = attr->get_value();

	attr = el->get_attribute("value");
	if ( ! attr ) {
	  throw InterfaceGeneratorInvalidContentException("no value for constant");
	}
	value = attr->get_value();
      } else {
	throw InterfaceGeneratorInvalidContentException("constant is not an element");
      }

      // Generate constant object
      try {
	InterfaceConstant constant(const_name, type, value, const_comment);
	constants.push_back(constant);
      } catch (InterfaceGeneratorInvalidTypeException &e) {
	e.printTrace();
      } catch (InterfaceGeneratorInvalidValueException &e) {
	e.printTrace();
      }
    }
    for (vector<InterfaceConstant>::iterator i = constants.begin(); i != constants.end(); ++i) {
      for (vector<InterfaceConstant>::iterator j = i + 1; j != constants.end(); ++j) {
	if ( (*i).getName() == (*j).getName() ) {
	  throw InterfaceGeneratorAmbiguousNameException((*i).getName().c_str(), "constant");
	}
      }
    }

    /*
     * enums
     *
     */
    set = constants_set[0]->find("enum");
    for (NodeSet::iterator i = set.begin(); i != set.end(); ++i) {

      std::string enum_comment;
      NodeSet comment_set = (*i)->find("comment/text()");
      if ( comment_set.size() == 0 ) {
	throw InterfaceGeneratorInvalidContentException("no comment for enum");
      } else {
	const TextNode *comment_node = dynamic_cast<const TextNode *>(comment_set[0]);
	if ( comment_node ) {
	  enum_comment = comment_node->get_content();
	} else {
	  throw InterfaceGeneratorInvalidContentException("enum comment not a text node");
	}
      }

      string enum_name;
      const Element * el = dynamic_cast<const Element *>(*i);
      if ( el ) {
	// valid element
	Attribute *attr;
	attr = el->get_attribute("name");
	if ( ! attr ) {
	  throw InterfaceGeneratorInvalidContentException("no name for enum");
	}
	enum_name = attr->get_value();

      } else {
	throw InterfaceGeneratorInvalidContentException("enum is not an element");
      }

      InterfaceEnumConstant enum_constant(enum_name, enum_comment);

      // Get constant name
      NodeSet items = (*i)->find("item");
      if ( items.size() == 0 ) {
	throw InterfaceGeneratorInvalidContentException("no items for enum");
      }

      for (NodeSet::iterator j = items.begin(); j != items.end(); ++j) {

	std::string item_name;
	const Element * el = dynamic_cast<const Element *>(*j);
	if ( el ) {
	  // valid element
	  Attribute *attr;
	  attr = el->get_attribute("name");
	  if ( ! attr ) {
	    throw InterfaceGeneratorInvalidContentException("no name for enum item");
	  }
	  item_name = attr->get_value();
	  
	} else {
	  throw InterfaceGeneratorInvalidContentException("enum item is not an element");
	}
	
	NodeSet comment_set = (*j)->find("text()");
	if ( comment_set.size() == 0) {
	  throw InterfaceGeneratorInvalidContentException("enum item without comment");
	}
	const TextNode *comment_node = dynamic_cast<const TextNode *>(comment_set[0]);
	if ( comment_node ) {
	  enum_constant.addItem(item_name, comment_node->get_content());
	} else {
	  throw InterfaceGeneratorInvalidContentException("enum comment not a text node");
	}
      }

      enum_constants.push_back(enum_constant);
    }
    for (vector<InterfaceEnumConstant>::iterator i = enum_constants.begin(); i != enum_constants.end(); ++i) {
      for (vector<InterfaceEnumConstant>::iterator j = i + 1; j != enum_constants.end(); ++j) {
	if ( (*i).getName() == (*j).getName() ) {
	  throw InterfaceGeneratorAmbiguousNameException((*i).getName().c_str(), "enum constant");
	}
      }
    }
  }

  /*
   * data
   *
   */
  set = root->find("/interface/data");
  if ( set.size() > 1 ) {
    throw InterfaceGeneratorInvalidContentException("more than one data block");
  } else if ( set.size() == 0 ) {
    throw InterfaceGeneratorInvalidContentException("no data block");
  }

  data_fields = getFields(set[0]);
  if ( data_fields.size() == 0 ) {
    throw InterfaceGeneratorInvalidContentException("data block contains no field");
  }

  NodeSet comment_set = root->find("/interface/data/comment/text()");
  if ( comment_set.size() == 0) {
    throw InterfaceGeneratorInvalidContentException("data block without comment");
  }
  const TextNode *comment_node = dynamic_cast<const TextNode *>(comment_set[0]);
  if ( comment_node ) {
    data_comment = comment_node->get_content();
  } else {
    throw InterfaceGeneratorInvalidContentException("data block comment not a text node");
  }

  /*
   * Messages
   *
   */
  set = root->find("/interface/message");
  for (NodeSet::iterator i = set.begin(); i != set.end(); ++i) {
    std::string msg_name;
    std::string msg_comment;

    const Element * el = dynamic_cast<const Element *>(*i);
    if ( el ) {
      Attribute *attr;
      attr = el->get_attribute("name");
      if ( ! attr ) {
	throw InterfaceGeneratorInvalidContentException("no name for message");
      }
      msg_name = attr->get_value();
    } else {
      throw InterfaceGeneratorInvalidContentException("message is not an element");
    }

    NodeSet comment_set = (*i)->find("text()");
    if ( comment_set.size() == 0) {
      throw InterfaceGeneratorInvalidContentException("message without comment");
    }
    const TextNode *comment_node = dynamic_cast<const TextNode *>(comment_set[0]);
    if ( comment_node ) {
      msg_comment = comment_node->get_content();
    } else {
      throw InterfaceGeneratorInvalidContentException("message comment not a text node");
    }


    vector<InterfaceField> msg_fields = getFields(*i);

    NodeSet ref_nodes = (*i)->find("ref/text()");
    for (NodeSet::iterator r = ref_nodes.begin(); r != ref_nodes.end(); ++r) {
      const TextNode *text_node = dynamic_cast<const TextNode *>(*r);
      if ( text_node ) {
	// find field in data fields
	bool found = false;
	for (vector<InterfaceField>::iterator j = data_fields.begin(); j != data_fields.end(); ++j) {
	  if ( (*j).getName() == text_node->get_content() ) {
	    // field found
	    msg_fields.push_back(*j);
	    found = true;
	    break;
	  }
	}
	if (! found) {
	  throw InterfaceGeneratorInvalidContentException("reference to non-existing data field");
	}	  
      } else {
	throw InterfaceGeneratorInvalidContentException("message ref not a text node");
      }
    }
    for (vector<InterfaceField>::iterator i = msg_fields.begin(); i != msg_fields.end(); ++i) {
      for (vector<InterfaceField>::iterator j = i + 1; j != msg_fields.end(); ++j) {
	if ( (*i).getName() == (*j).getName() ) {
	  throw InterfaceGeneratorAmbiguousNameException((*i).getName().c_str(), "message field");
	}
      }
    }

    InterfaceMessage msg(msg_name, msg_comment);
    msg.setFields(msg_fields);

    messages.push_back(msg);
  }

}


/** Get interface name.
 * Only valid after parse().
 * @return interface name.
 */
std::string
InterfaceParser::getInterfaceName()
{
  return name;
}


/** Get interface author.
 * Only valid after parse().
 * @return interface author.
 */
std::string
InterfaceParser::getInterfaceAuthor()
{
  return author;
}


/** Get constants.
 * Only valid after parse().
 * @return constants.
 */
std::vector<InterfaceConstant>
InterfaceParser::getConstants()
{
  return constants;
}


/** Get enum constants.
 * Only valid after parse().
 * @return enum constants.
 */
std::vector<InterfaceEnumConstant>
InterfaceParser::getEnumConstants()
{
  return enum_constants;
}


/** Get data fields.
 * Only valid after parse().
 * @return data fields.
 */
std::vector<InterfaceField>
InterfaceParser::getDataFields()
{
  return data_fields;
}


/** Get data comment.
 * Only valid after parse().
 * @return data comment.
 */
std::string
InterfaceParser::getDataComment()
{
  return data_comment;
}


/** Get messages.
 * Only valid after parse().
 * @return messages.
 */
std::vector<InterfaceMessage>
InterfaceParser::getMessages()
{
  return messages;
}