/* Copyright 2015 Henorvell Ge
 *
 * This file is a part of CampusExpBot
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#include "Defines.h"
#include "tinyxml.h"

#include <map>
#include <memory>
struct IRsDef;
using IRsMap = std::map<int,IRsDef *>;
class xml_helper
{
	public:
		// Initialization
		xml_helper(const char *file_path);
		~xml_helper();
		int load_sys_config();
		void unload_sys_conf();
		const char *get_gohttp_so();
		IRsMap get_IRs_conf();
	protected:
		TiXmlDocument *doc;
		const char *gohttp_so;
		std::unique_ptr<IRsMap> IRs_config;
};
//only one xml class
extern xml_helper *conf;
