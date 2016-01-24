/* Copyright 2015 Henorvell Ge
 *
 * This file is a part of CampusExpBot
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#include "xmlhandler.h"

#include <fstream>
#include <string>
#include <map>
#include <stdlib.h>
#include <stdexcept>
//xml_helper method
xml_helper::xml_helper(const char *file_path)
{
	std::string exp;
	std::ifstream fin(file_path);
	if (!fin){
		exp = "xmlhandler.cpp:xml_helper INIT Failed: No such file or directory.";
		throw std::runtime_error(exp);
	}
	doc=new TiXmlDocument(file_path);
	if(!(doc->LoadFile() == true)){
		exp = "xmlhandler.cpp:xml_helper doc.LoadFile() Failed.";
		throw std::runtime_error(exp);
	}
}
xml_helper::~xml_helper()
{
	this->unload_sys_conf();
	delete doc;
}
int xml_helper::load_sys_config()
{
	TiXmlHandle docHandle(doc);
	TiXmlElement *Tree = docHandle.FirstChild("DataBase").ToElement();
	this -> database_path = Tree -> Attribute("path");
	Tree = docHandle.FirstChild("wiringPi").ToElement();
	this -> wiringPi_path = Tree -> Attribute("path");
	return 0;
}
void xml_helper::unload_sys_conf()
{

}
const char *xml_helper::get_database_path()
{
	return this -> database_path;
}
const char *xml_helper::get_wiringPi_path()
{
	return this -> wiringPi_path;
}
