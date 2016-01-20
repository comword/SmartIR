/* Copyright 2016 Henorvell Ge
*
* This file is a part of CampusExpBot
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
class xml_helper;
class golinker
{
public:
  golinker(const char* go_so_path);
  virtual ~golinker();
  char* (*InitBinding) (char*);
private:
  void *goso_handle;
};
