/* Copyright 2015 Henorvell Ge
 *
 * This file is a part of CampusExpBot
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#include "Defines.h"
#include "tinyxml.h"

class xml_helper
{
	public:
		// Initialization
		xml_helper(const char *file_path);
		~xml_helper();
		int load_sys_config();
		void unload_sys_conf();
		const char *get_database_path();
	protected:
		TiXmlDocument *doc;
		const char *database_path;
		const char *python_path;
};
//only one xml class
extern xml_helper *conf;
